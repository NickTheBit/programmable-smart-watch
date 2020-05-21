#! /usr/bin/env python3

import json
import serial
import threading as th

# This is the library that establishes a connection to the watch
# sudo rfcomm connect /dev/rfcomm0 CC:50:E3:B6:8C:0A

class watch(self):
    def __init__(self,port='/dev/rfcomm0'):
        self.port = port
        self.gate = serial.Serial(port)  # open serial port
        print(self.gate.name)         # check which port was really used
        self.gate.write(b'Computer connected')     # write a string

    def transmit(self,data):
        self.gate.write(data)

    # This is the function that constatntly looks for transmissions
    def listener(self):
        while True:
            data = self.gate.read_until(serial.LF)
            print (data)

    # Listen function initiates a thread listening for watch trasmissions
    def listen(self):
        thread = th.Thread(target=self.listener)
        thread.start()



# This is the resource access library for the psw
# The libraries focus is to provide developers with an easy way to send
# data to the watch, display various thigs on the screen and recieve data
class function(watch):
    def __init__(self,name,content,action1,action2,action3):
        watch.__init__(self)
        self.name = name 
        self.content = content 
        self.action1 = action1
        self.action2 = action2
        self.action3 = action3

    