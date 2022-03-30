#!/usr/bin/python
import smbus
import time
bus = smbus.SMBus(1)
address=0x8
res=0.0048828125
num=10

while 1:
    valAVG=0
    arrvals=[]
    '''for i in range(num):
        bus.write_byte_data(address, 1, 1)
        MSB8=bus.read_byte_data(address, 1)
        LSB=bus.read_byte_data(address, 1)
        MSB8=MSB8<<2
        val=MSB8|LSB
        val=val*res
        arrvals.append(val)
        valAVG+=val
        time.sleep(0.2)
    valAVG=valAVG/num
    '''
    bus.write_byte_data(address, 1, 1)
    time.sleep(0.02)
    MSB8=bus.read_byte_data(address, 1)
    LSB=bus.read_byte_data(address, 1)
    MSB8=MSB8<<2
    val=MSB8|LSB
    val=val*res
    print(("%0.2f V" % ((val))), "- %d dBm" % int( (((val))-1)*100))
    time.sleep(0.3)
    
