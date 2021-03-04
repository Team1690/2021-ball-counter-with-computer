import serial
ser = serial.Serial('COM3', 9600)
ser.reset_input_buffer()

ser.write(b'H')
ser.close()
