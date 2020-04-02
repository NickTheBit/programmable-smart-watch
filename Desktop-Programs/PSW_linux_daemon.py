#! /usr/bin/env python3
# sudo rfcomm connect /dev/rfcomm0 CC:50:E3:B6:8C:0A
import serial
import re
import json

ser = serial.Serial('/dev/rfcomm0')  # open serial port
print(ser.name)         # check which port was really used
ser.write(b'Computer connected')     # write a string

buffer = 37
print ("Listenning...")
while True:
    data = ser.read_until(serial.LF)
    if data == "":
        pass
    else:
        print (data)
        
