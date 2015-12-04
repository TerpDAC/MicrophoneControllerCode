#!/usr/bin/env python
import serial
import time

import espcfg

global ser

SERIAL_PORT, WIFI_AP_NAME, WIFI_AP_PASS = espcfg.init()

ser = serial.Serial(SERIAL_PORT, 115200, timeout=0.1)

def ATCMD(cmd, wait = 2):
    global ser
    print "Sending: %s" % (cmd)
    ser.write(cmd + "\r\n")
    time.sleep(wait)
    print "Result of %s:" % (cmd)
    print ser.read(100000)

def goFetch(wait = 2):
    global ser
    time.sleep(wait)
    print ser.read(100000)

bootmsg = ser.read(100)
print "Boot message:"
print bootmsg

ATCMD("AT")

x = raw_input("Do we need to connect? Type yes to connect to WiFi. >> ")
if x.lower() == "yes":
    ATCMD("AT+RST", 5)
    ATCMD('AT+CWJAP="%s","%s"' % (WIFI_AP_NAME, WIFI_AP_PASS), 10)

ATCMD('AT+CIPSTART="TCP","example.com",80', 2)

# 16: GET
# 23: HOST
# 4: \r\n
ATCMD('AT+CIPSEND=43', 3)
ser.write("GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n")

goFetch(3)

ATCMD('+IPD')

goFetch(5)

ATCMD('AT+CIPCLOSE')

ser.close()
