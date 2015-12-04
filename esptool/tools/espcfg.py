#!/usr/bin/env python
# Configuration module

import os
import sys
import getpass
import glob
import serial
import time
from ConfigParser import SafeConfigParser

# Source: http://stackoverflow.com/a/14224477
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def prune_ports_atcmd(ports):
    result = []
    for port in ports:
        try:
            s = serial.Serial(port, 115200, timeout = 1)
            s.write("AT\r\n")
            time.sleep(3)
            x = s.read(100)
            s.close()
            
            if "OK" in x:
                result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def serial_port_wizard():
    print("Searching for serial ports (this may take some time)...")
    s_ports = serial_ports()
    
    if len(s_ports) <= 0:
        print("No open serial ports found! Make sure you have enough permissions")
        print("to open a serial port, and/or the device is plugged in and")
        print("recognized.")
        sys.exit(1)
    
    at_ports = prune_ports_atcmd(s_ports)
    
    non_at_ports = [x for x in s_ports if x not in at_ports]
    
    all_ports = at_ports + non_at_ports
    
    i = 1
    
    print("Detected potential serial ports!\n")
    
    if len(at_ports) > 0:
        print("The following serial reports may be the ESP8266:")
        for x in at_ports:
            print "  %4i: %s **" % (i, x)
            i += 1
    
    if len(non_at_ports) > 0:
        print("The following serial reports did not respond, but are available:")
        for x in non_at_ports:
            print "  %4i: %s" % (i, x)
            i += 1
        
    i -= 1
    
    print("")
    
    while 1:
        if i > 1:
            select_prompt = "Please select a serial port (1-%i): " % i
        else:
            select_prompt = "Please select a serial port (1): "
        
        select_ser_i = raw_input(select_prompt)
        
        if not select_ser_i.isdigit():
            print("Invalid input - please enter a number!\n")
        else:
            select_ser_i = int(select_ser_i)
            if (select_ser_i > 0) and (select_ser_i <= i):
                break
            print("Invalid option!\n")
    
    selected_ser_port = all_ports[select_ser_i - 1]
    
    # All done with serial port selection!
    return selected_ser_port

def wifi_user_pass_wizard():
    print("Now we need to know what your WiFi name and password is!\n")
    ap_name = raw_input("What is your WiFi access point name? ")
    
    print("\nNow we will ask for your WiFi password. As you type your")
    print("WiFi password, the output will be hidden. You will see")
    print("nothing as you type - just type normally and press ENTER")
    print("when you're done!\n")
    
    while 1:
        ap_pass_first = getpass.getpass("What is your WiFi password? ")
        ap_pass_second = getpass.getpass("Type your WiFi password again: ")
        
        if ap_pass_first == ap_pass_second:
            ap_pass = ap_pass_first
            break
        
        print("The passwords you entered don't match! Try again...\n")
    
    print("")
    
    return ap_name, ap_pass

def init():
    homedir = os.path.expanduser("~")
    cfgpath = os.path.join(homedir, "TerpDAC-ESPCFG.ini")
    
    parser = SafeConfigParser()
    
    parser.read(cfgpath)
    
    serial_port = None
    wifi_name = None
    wifi_pass = None
    
    valid_config = False
    
    if parser.has_section("terpdac-espcfg"):
        if parser.has_option("terpdac-espcfg", "serial_port") and \
            parser.has_option("terpdac-espcfg", "wifi_name") and \
            parser.has_option("terpdac-espcfg", "wifi_pass"):
            
            valid_config = True
            
            # Extract data from config!
            serial_port = parser.get("terpdac-espcfg", "serial_port")
            wifi_name = parser.get("terpdac-espcfg", "wifi_name")
            wifi_pass = parser.get("terpdac-espcfg", "wifi_pass")
    
    if valid_config:
        print("We found a previous configuration:")
        print("  Serial port:   %s" % serial_port)
        print("  WiFi AP Name:  %s" % wifi_name)
        print("  WiFI Password: [hidden]")
        
        print("To use this configuration, just press ENTER.")
        print("To delete and recreate a configuration file, type \"delete\"")
        print("(without quotes) and press ENTER.")
        
        resp = raw_input(">> ")
        
        if resp == "delete":
            print("Deleting old configuration and creating a new one...")
            os.remove(cfgpath)
            valid_config = False
        else:
            print("Using existing configuration.")
    else:
        print("Missing or invalid configuration detected, starting configuration.")
        try:
            os.remove(cfgpath)
        except OSError:
            pass
        
    if not valid_config:
        print("ESP8266 Configuration")
        print("======================")
        
        if not parser.has_section("terpdac-espcfg"):
            parser.add_section("terpdac-espcfg")
        
        selected_ser_port = serial_port_wizard()
        parser.set("terpdac-espcfg", "serial_port", selected_ser_port)
        
        ap_name, ap_pass = wifi_user_pass_wizard()
        parser.set("terpdac-espcfg", "wifi_name", ap_name)
        parser.set("terpdac-espcfg", "wifi_pass", ap_pass)
        
        # Confirm the information
        print("Confirm the new configuration:")
        print("  Serial port:   %s" % selected_ser_port)
        print("  WiFi AP Name:  %s" % ap_name)
        print("  WiFI Password: [hidden]")
        print("To accept this configuration, press ENTER.")
        print("Otherwise, press CTRL-C.")
        raw_input(">> ")
        
        # Save configuration
        try:
            fh = open(cfgpath, "w")
            parser.write(fh)
            fh.close()
        except:
            print("Error writing configuration file. Continuing without saving.")
        
        valid_config = True
    
    # Fetch configuration values!
    serial_port = parser.get("terpdac-espcfg", "serial_port")
    wifi_name = parser.get("terpdac-espcfg", "wifi_name")
    wifi_pass = parser.get("terpdac-espcfg", "wifi_pass")
    
    # Return!
    return serial_port, wifi_name, wifi_pass
