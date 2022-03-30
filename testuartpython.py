import serial
from time import sleep

ser = serial.Serial ("/dev/ttyAMA0", 9600)    #Open port with baud rate
print (ser.name)
print (ser.is_open)

def sendDataToScreen(data):
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(data.encode("utf-8"))
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(b'\xff')

sendDataToScreen("txtConn.aph=127");
sendDataToScreen("zx.val=100");
sendDataToScreen("zy.val=127");


"""
while True:
    received_data = ser.read()              #read serial port
    sleep(0.03)
    data_left = ser.inWaiting()             #check for remaining byte
    received_data += ser.read(data_left)
    print (received_data)
"""
