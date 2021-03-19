import serial
import json
import serial.tools.list_ports

def find_arduino_port(): #will return the first arduino uno it finds
    for port in serial.tools.list_ports.comports():
        if 'VID:PID=2341:0043' in port.hwid:   
            return port.device

def get_goal_char():
    counter = serial.Serial(find_arduino_port(), 9600)
    counter.reset_input_buffer()

    goal_char = counter.read(1).decode('UTF-8')
    return goal_char

# bluetooth.close()
