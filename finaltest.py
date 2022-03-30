#!/usr/bin/python
import smbus
import math
import time
import serial
from MPL3115A2 import MPL3115A2

ser = serial.Serial ("/dev/ttyAMA0", 57600)    #Open port with baud rate


def sendDataToScreen(data):
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(data.encode("utf-8"))
    ser.write(b'\xff')
    ser.write(b'\xff')
    ser.write(b'\xff')

# Register
power_mgmt_1 = 0x6b
power_mgmt_2 = 0x6c
 
def read_byte(reg):
    return bus.read_byte_data(address, reg)
 
def read_word(reg):
    h = bus.read_byte_data(address, reg)
    l = bus.read_byte_data(address, reg+1)
    value = (h << 8) + l
    return value
 
def read_word_2c(reg):
    val = read_word(reg)
    if (val >= 0x8000):
        return -((65535 - val) + 1)
    else:
        return val
 
def dist(a,b):
    return math.sqrt((a*a)+(b*b))
 
def get_y_rotation(x,y,z):
    radians = math.atan2(x, dist(y,z))
    return -math.degrees(radians)
 
def get_x_rotation(x,y,z):
    radians = math.atan2(y, dist(x,z))
    return math.degrees(radians)
    
def get_z_rotation(x,y,z):
    radians = math.atan2(z, dist(x,y))
    return math.degrees(radians)
bus = smbus.SMBus(1) # bus = smbus.SMBus(0) fuer Revision 1
address = 0x68       # via i2cdetect
 
# Aktivieren, um das Modul ansprechen zu koennen
bus.write_byte_data(address, power_mgmt_1, 0)

sendDataToScreen("txtConn.aph=127")
goodPointx=90
goodPointy=110
wide=5
goodHeight=0.25
Hwide=0.05
flag_goodPoint=0
sendDataToScreen("zxRange.val="+str(goodPointx))
sendDataToScreen("zyRange.val="+str(goodPointy))
time.sleep(0.5)
sendDataToScreen("zxRange.vvs0="+str(wide*2))
sendDataToScreen("zyRange.vvs0="+str(wide*2))

#---------------height----------------
mpl3115a2 = MPL3115A2()

mpl3115a2.control_alt_config()
mpl3115a2.data_config()
time.sleep(1)
alt = mpl3115a2.read_alt_temp()
print ("set this Altitude as offset : %.2f m"%(alt['a']))
offset_height=alt['a']

while 1:
	mpl3115a2.control_alt_config()
	mpl3115a2.data_config()
	#time.sleep(1)
	alt = mpl3115a2.read_alt_temp()
	h=alt['a']
	temp=alt['c']
	gyroskop_xout = read_word_2c(0x43)
	gyroskop_yout = read_word_2c(0x45)
	gyroskop_zout = read_word_2c(0x47)
	
	
	beschleunigung_xout = read_word_2c(0x3b)
	beschleunigung_yout = read_word_2c(0x3d)
	beschleunigung_zout = read_word_2c(0x3f)
	 
	beschleunigung_xout_skaliert = beschleunigung_xout / 16384.0
	beschleunigung_yout_skaliert = beschleunigung_yout / 16384.0
	beschleunigung_zout_skaliert = beschleunigung_zout / 16384.0
	Xrotation=int(get_x_rotation(beschleunigung_xout_skaliert, beschleunigung_yout_skaliert, beschleunigung_zout_skaliert))
	Yrotation=int(get_y_rotation(beschleunigung_xout_skaliert, beschleunigung_yout_skaliert, beschleunigung_zout_skaliert))
	# checking if it is a good point
	Xrotation=90+Xrotation
	Yrotation=90+Yrotation
	if Xrotation>goodPointx-wide and Xrotation< goodPointx+wide:
	    flag_goodPoint=1
	    sendDataToScreen('insX.txt="Good X"')
	elif Xrotation< goodPointx-wide:
	    sendDataToScreen('insX.txt="Move X UP"')
	    flag_goodPoint=0
	else :
	    sendDataToScreen('insX.txt="Move X DOWN"')
	    flag_goodPoint=0
	
	if Yrotation>goodPointy-wide and Yrotation< goodPointy+wide:
	    flag_goodPoint+=1
	    sendDataToScreen('insY.txt="Good Y"')
	elif Yrotation< goodPointy-wide:
	    sendDataToScreen('insY.txt="Move Y RIGHT"')
	    flag_goodPoint=0
	else :
	    sendDataToScreen('insY.txt="Move Y LEFT"')
	    flag_goodPoint=0
	    
	if flag_goodPoint==2:
	    sendDataToScreen("txtgood.aph=127")
	else:
	    sendDataToScreen("txtgood.aph=0")
	
	sendDataToScreen("zx.val="+str(Xrotation))
	sendDataToScreen("zy.val="+str(Yrotation))
	sendDataToScreen("add 11,0,"+str(Xrotation))
	sendDataToScreen("add 12,0,"+str(Yrotation))
	sendDataToScreen('txth.txt="'+str(h)+' M"')
	sendDataToScreen('txtTemp.txt="'+str(temp)[:5]+' C"')
	#time.sleep(0.1)

