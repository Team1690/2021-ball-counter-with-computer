import sys
import serial
import serial.tools.list_ports
import requests
from requests.exceptions import ConnectTimeout
import time
import _thread as thread
import websocket

FMS_SERVER = "10.0.100.5:8080"
ALLIANCE_COLOR = 'red' # Change accordingly
USERNAME = 'admin'

goal_char_msg_map = {
    "I": '{ "type": "CI" }',
    "O": '{ "type": "CO" }',
    "L": '{ "type": "CL"}'
}

close_thread = False

# Return the first arduino uno connected to PC
def find_arduino_port(): 
    for port in serial.tools.list_ports.comports():
        if 'VID:PID=2341:0043' in port.hwid:   
            return port.device

# Retur na char recieved from arduino
def get_goal_char(connection):
    connection.reset_input_buffer()
    goal_char = connection.read(1).decode('UTF-8')
    return goal_char

def get_msg_from_goal_char(goal_char):
    return goal_char_msg_map[goal_char]

index = 0

def get_on_ws_open_callback(connection):
    global index

    index_ = index
    index = index + 1
    def on_ws_open(ws):
        print("Connected to FMS")

        def run(*args):
            global close_thread

            print("Running")
            close_thread = False
            while(not close_thread):
                goal_char = get_goal_char(connection)
                print(f'Info {index_}: recieved "{goal_char}"')

                if (goal_char in goal_char_msg_map):
                    print(f'Info {index_}: sent {get_msg_from_goal_char(goal_char)}')
                    ws.send(get_msg_from_goal_char(goal_char))
                else:
                    pass
                    # print('Error: unknown char recieved')
            print("Closing thread")

        thread.start_new_thread(run, ())
    
    return on_ws_open
    
def on_close(ws, close_status, close_msg):
    global close_thread

    close_thread = True

    ws.close()
    print("Connection to Chessy-Arena disconnected")

def on_error(ws, error):
    print("WS ERROR: ", error)

def open_websocket(serial_connection, password, color):
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
            print("Login to Cheesy-Arena failed: ", e)

        time.sleep(1)

    print("Connection to websocke")
    while True:
        ws = websocket.WebSocketApp(f'ws://{FMS_SERVER}/panels/scoring/{color}/websocket'
            , on_open=get_on_ws_open_callback(serial_connection)
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

    connection = serial.Serial(find_arduino_port(), 9600, timeout=1)

    if (connection.is_open):
        print("Connected to arduino")
    
    open_websocket(connection, password, color)

main()
