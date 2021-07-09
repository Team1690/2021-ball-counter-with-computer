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

def get_on_ws_open_callback(connection):
    def on_ws_open(ws):
        print("Connected to FMS")

        def run(*args):
            while(True):
                goal_char = get_goal_char(connection)
                print(f'Info: recieved "{goal_char}"')

                if (goal_char in goal_char_msg_map):
                    print(f'Info: sent {get_msg_from_goal_char(goal_char)}')
                    ws.send(get_msg_from_goal_char(goal_char))
                else:
                    pass
                    # print('Error: unknown char recieved')

        thread.start_new_thread(run, ())
    
    return on_ws_open
    
def open_websocket(serial_connection, password, color):
    print("Open websocke")
    def reopen_websocket():
        open_websocket(serial_connection, color)

    while True:
        try:
            print("Posting")
            res = requests.post(f'http://{FMS_SERVER}/login'
                , data={'username': USERNAME, 'password': password}
                , allow_redirects=False, timeout=5
            )
            break
        except ConnectTimeout as e:
            pass

    print("Connection to websocke")
    ws = websocket.WebSocketApp(f'ws://{FMS_SERVER}/panels/scoring/{color}/websocket'
        , on_open=get_on_ws_open_callback(serial_connection)
        , on_close=reopen_websocket
        , cookie="; ".join(["%s=%s" %(i, j) for i, j in res.cookies.get_dict().items()])
    )

    ws.run_forever()

        
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

    connection = serial.Serial(find_arduino_port(), 9600)

    if (connection.is_open):
        print("Connected to arduino")
    
    open_websocket(connection, password, color)

main()
