import RPi.GPIO as GPIO
import time
import RPi.GPIO as GPIO
import time
import sys
import serial
import serial.tools.list_ports
import requests
from requests.exceptions import ConnectTimeout
import time
import _thread as thread
import websocket
import json
from termcolor import colored

FMS_SERVER = "10.0.100.5:8080"
ALLIANCE_COLOR = 'red' # Change accordingly
USERNAME = 'admin'

S2_PIN = 19
S3_PIN = 13
OUTPUT_PIN = 26
CYCLES_TO_WAIT = 10

COLORS = {
    "blue": {"prev": "yellow", "next": "green"},
    "green": {"prev": "blue", "next": "red"},
    "red": {"prev": "green", "next": "yellow"},
    "yellow": {"prev": "red", "next": "blue"}
}


def hue_saturation_from_rgb(r, g, b):
    c_max = max(r, g, b)
    c_min = min(r, g, b)
    delta = c_max - c_min
    hue = 0
    saturation = 0

    if delta == 0:
        hue = 0
    elif c_max == r:
        hue = 60 * ((g - b) / delta % 6)
    elif c_max == g:
        hue = 60 * ((b - r) / delta + 2)
    elif c_max == b:
        hue = 60 * ((r - g) / delta + 4)
    if c_max != 0:
        saturation = delta / c_max

    return hue, saturation


# needs to be calibrated when changing lighting
def normalize_frequency(freq):
    return min(1, max(0, (freq - 250) / 4000))


def get_raw_frequency(S2_VALUE, S3_VALUE):
    GPIO.output(S2_PIN, S2_VALUE)
    GPIO.output(S3_PIN, S3_VALUE)
    time.sleep(0.001)
    GPIO.wait_for_edge(OUTPUT_PIN, GPIO.FALLING)
    start = time.time()

    for impulse_count in range(CYCLES_TO_WAIT):
        GPIO.wait_for_edge(OUTPUT_PIN, GPIO.FALLING)

    duration = time.time() - start  # seconds to run for loop
    frequency = CYCLES_TO_WAIT / duration  # in Hz
    return frequency


def send_data(curr_color, switched_color, stayed_for_2_sec):
    # TODO send data
    return curr_color, switched_color, stayed_for_2_sec


def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(OUTPUT_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(S2_PIN, GPIO.OUT)
    GPIO.setup(S3_PIN, GPIO.OUT)
    print("\n")

def get_on_ws_open_callback():
    def on_ws_open(ws):
        print("Connected to FMS")

        setup()

        def run(*args):
            curr_color = prev_color = send_color = prev_send_color = ""
            color_change_time = 0
            send_color_change_time = 0
            last_send_time = 0
            rotation = 0
            switched_color = False
            waited_2_seconds = False
            try:
                while True:
                    curr_time = time.time()

                    red_frequency = get_raw_frequency(GPIO.LOW, GPIO.LOW)
                    blue_frequency = get_raw_frequency(GPIO.LOW, GPIO.HIGH)
                    green_frequency = get_raw_frequency(GPIO.HIGH, GPIO.HIGH)
                    norm_red = normalize_frequency(red_frequency)
                    norm_blue = normalize_frequency(blue_frequency)
                    norm_green = normalize_frequency(green_frequency)
                    hue, saturation = hue_saturation_from_rgb(norm_red, norm_green, norm_blue)
                    if 250 < hue < 360 and curr_color != "red":
                        curr_color = "red"
                        color_change_time = curr_time
                    elif 155 < hue < 190 and curr_color != "green":
                        curr_color = "green"
                        color_change_time = curr_time
                    elif 200 < hue < 240 and curr_color != "blue":
                        curr_color = "blue"
                        color_change_time = curr_time
                    elif 10 < hue < 155 and curr_color != "yellow":
                        curr_color = "yellow"
                        color_change_time = curr_time

                    if curr_time - color_change_time >= 0.06 \
                        and (prev_send_color in COLORS.get(curr_color, dict()).values() \
                        or not prev_send_color):
                        send_color = curr_color
                        send_color_change_time = curr_time
                    try:
                        pass
                       # print(hue, saturation, colored(curr_color, curr_color), colored(send_color, send_color))
                    except:
                        pass
                        
                    if send_color and COLORS[send_color]["prev"] == prev_send_color:
                        rotation += 1
                    elif send_color and COLORS[send_color]["next"] == prev_send_color:
                        rotation -= 1

                    waited_2_seconds = (curr_time - send_color_change_time) >= 2
                    waited_5_seconds = (curr_time - send_color_change_time) >= 5
                    if curr_time - last_send_time >= 0.1:
                        data = json.dumps({"color": send_color, "rotation": rotation, "waited2": waited_2_seconds, "waited5": waited_5_seconds})
                        ws.send(json.dumps({"type": "WL", "data": data}))
                        rotation = 0
                        if send_color != prev_send_color:
                            print(f"sending: {data}")
                            pass
                        
                        prev_send_color = send_color

                    #if curr_color != prev_color:
                        #print(curr_color)

                    prev_color = curr_color
                    
            finally:
                GPIO.cleanup()

        thread.start_new_thread(run, ())
    
    return on_ws_open

def on_close(ws, close_status, close_msg):
    ws.close()
    
def on_error(ws, error):
    print("WS ERROR: ", error)

def open_websocket(password, color):
    print("Open websocket")

    while True:
        try:
            print("Posting")
            res = requests.post(f'http://{FMS_SERVER}/login'
                , data={'username': USERNAME, 'password': password}
                , allow_redirects=False, timeout=5
            )
            break
        except Exception as e:
            pass

    print("Connection to websocke")
    while True:
        ws = websocket.WebSocketApp(f'ws://{FMS_SERVER}/panels/scoring/{color}/websocket'
            , on_open=get_on_ws_open_callback()
            , on_close=on_close
            , on_error=on_error
            , cookie="; ".join(["%s=%s" %(i, j) for i, j in res.cookies.get_dict().items()])
        )

        ws.run_forever()
        time.sleep(1)

        
def main():
    HELP = f"USAGE: python3 {sys.argv[0]} fms_admin_password red/blue"
    if len(sys.argv) < 3:
        print(HELP)
        return

    password = sys.argv[1]
    color = sys.argv[2]
    if color not in ['red', 'blue']:
        print(HELP)
        return

    
    open_websocket(password, color)

main()
