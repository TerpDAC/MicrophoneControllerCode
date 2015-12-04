#!/usr/bin/env python
import serial

import espcfg

SERIAL_PORT, WIFI_AP_NAME, WIFI_AP_PASS = espcfg.init()

ser = serial.Serial(SERIAL_PORT, 115200, timeout=1)
bootmsg = ser.read(100)
print "Boot message:"
print bootmsg

ser.write("AT\r\n")
x = ser.read(100)

print "Result of AT:"
print x

ser.write("AT+CIPSTAMAC?\r\n")
x = ser.read(100)

print "Result of AT+CIPSTAMAC?:"
print x

ser.write("AT+CIPAPMAC?\r\n")
x = ser.read(100)

print "Result of AT+CIPAPMAC?:"
print x

ser.close()
