#!/usr/bin/env python
import serial
import readline

import espcfg

SERIAL_PORT, WIFI_AP_NAME, WIFI_AP_PASS = espcfg.init()

print("Connecting...")
ser = serial.Serial(SERIAL_PORT, 115200, timeout=1)

print("Connected!")

bootmsg = ser.read(100)

print("Boot message:")
print(bootmsg)

cmd = ""
while cmd != exit:
    cmd = raw_input(">> ")
    if cmd:
        if cmd.split(" ")[0] == "rawmodecr":
            print("Entering RAW (carriage return) mode.")
            try:
                while 1:
                    ser.write(raw_input("RAW >>") + "\r\n")
            except:
                pass
            print("Exited RAW (carriage return) mode.")
        else:
            ser.write(cmd+"\r\n")
    
    output = ser.read(10000)
    
    print output

ser.close()
