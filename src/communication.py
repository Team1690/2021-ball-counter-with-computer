import serial
import json

bluetooth = serial.Serial('COM3', 9600)
bluetooth.reset_input_buffer()

# bluetooth.write(b'H')
input = bluetooth.read(22)
print(input)
input_json = json.loads(input)
print(input_json["value"])
print("------------")
print(input_json["value2"])

bluetooth.close()
