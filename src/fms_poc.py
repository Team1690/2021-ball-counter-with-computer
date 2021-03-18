import serial
import serial.tools.list_ports
import requests
import time
import _thread as thread
import websocket

ALLIANCE_COLOR = 'red' #change accordingly
USERNAME = 'admin'
PASSWORD = 'Password1'

def find_arduino_port(): #will return the first arduino uno it finds
    for port in serial.tools.list_ports.comports():
        if 'VID:PID=2341:0043' in port.hwid:   
            return port.device

counter = serial.Serial(find_arduino_port(), 9600)

def get_goal_char():
    counter.reset_input_buffer()

    goal_char = counter.read(1).decode('UTF-8')
    return goal_char


def on_open(ws):
    def run(*args):
        for i in range(3):
            time.sleep(1)
            ws.send('{"type":"S"}')
        time.sleep(1)
        ws.send(get_goal_char())
        ws.close()
        print("thread terminating...")
    thread.start_new_thread(run, ())

res = requests.post("http://localhost:8080/login", data={'username': USERNAME, 'password': PASSWORD}, allow_redirects=False)
ws = websocket.WebSocketApp("ws://localhost:8080/panels/scoring/%s/websocket" %ALLIANCE_COLOR, on_open=on_open, cookie="; ".join(["%s=%s" %(i, j) for i, j in res.cookies.get_dict().items()]))
ws.run_forever()