#!/usr/bin/env python
import os
import sys
import glob
import serial
import time
import Queue

from PyQt4 import QtGui, QtCore
import qdarkstyle

import sys
import colorama

import MicDataCollectorGUI
from proggui import ProgressQueueThread, ProgressWindow

class MDCWindow(QtGui.QDialog, MicDataCollectorGUI.Ui_mdcDialog):
    def __init__(self):
        # Explaining super is out of the scope of this article
        # So please google it if you're not familar with it
        # Simple reason why we use it here is that it allows us to
        # access variables, methods etc in the design.py file
        super(self.__class__, self).__init__(None, QtCore.Qt.WindowSystemMenuHint | QtCore.Qt.WindowTitleHint | QtCore.Qt.MSWindowsFixedSizeDialogHint)
        self.setupUi(self)  # This is defined in design.py file automatically
                            # It sets up layout and widgets that are defined
        
        #self.ports = get_possible_serial_ports(QtGui.QApplication.instance())
        
        #get_possible_serial_ports(QtGui.QApplication.instance())
        
        #self.serialPortCombo.clear()
        #self.serialPortCombo.addItems(self.ports)
        
        self.refreshSerialPorts()
        self.serialPortCombo.addItems(["/dev/ttyUSB0"])
        self.refreshBtn.clicked.connect(self.refreshSerialPorts)
        
        self.startBtn.clicked.connect(self.startCollection)
    
    def finishSerialData(self, result):
        self.ports = result.val
        
        self.serialPortCombo.clear()
        self.serialPortCombo.addItems(self.ports)
        
    def refreshSerialPorts(self):
        #self.rsp_thread = ExecThread(self.finishSerialData, get_possible_serial_ports, QtGui.QApplication.instance())
        #self.ports = get_possible_serial_ports(QtGui.QApplication.instance())
        #self.rsp_thread.start()
        
        self.ports = get_possible_serial_ports(QtGui.QApplication.instance())
        self.serialPortCombo.clear()
        self.serialPortCombo.addItems(self.ports)
        
    def startCollection(self):
        port = self.serialPortCombo.currentText()
        if port:
            get_data(self.serialPortCombo.currentText())

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

def prune_ports_uno(ports):
    result = []
    for port in ports:
        try:
            s = serial.Serial(port, 9600, timeout = 3)
            x = s.read(16)
            s.close()
            
            if "TERPDAC" in x:
                result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def get_possible_serial_ports(qtapp = None):
    q = Queue.Queue()
    
    # Progress bar
    pb = ProgressWindow(title="Serial Port Search", status="Searching for serial ports (this may take some time)...", pulsing = True, comm_queue = q)
    pb.show()
    
    print("Searching for serial ports (this may take some time)...")
    
    s_ports = serial_ports()
    
    if len(s_ports) <= 0:
        print("No open serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.")
        QtGui.QMessageBox.critical(pb, "Error",
                "No open serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.",
                QtGui.QMessageBox.Ok)
        pb.close()
        return []
    
    pb.updatePulse(False)
    pb.updateStatus("Found serial ports, scanning for mic sensor...")
    pb.updateProg(50)
    
    uno_ports = prune_ports_uno(s_ports)
    
    if len(uno_ports) <= 0:
        print("No open Arduino serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.")
        QtGui.QMessageBox.critical(pb, "Error",
                "No open Arduino serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.",
                QtGui.QMessageBox.Ok)
        
        pb.close()
        
        return []
    
    pb.updateProg(100)
    pb.close()
    
    return uno_ports

def get_data(port):
    q = Queue.Queue()
    
    # Progress bar
    pb = ProgressWindow(title="Data Collection", status="Now collecting data!", pulsing = True, comm_queue = q)
    pb.show()
    
    fh = open("data.csv", "w")
    
    startTime = time.time()
    
    dataBuf = ""
    print str(port)
    try:
        s = serial.Serial(str(port), 9600, timeout = 0.1)
    except (OSError, serial.SerialException):
        print("ERROR")
    
    print("Start data collect")
    
    #pb.updatePulse(False)
    
    samps = 60
    
    while 1:
        now = time.time()
        
        buf = s.read(10000)
        dataBuf += buf
        
        ct_arr = dataBuf.split("\n")
        ct = len(ct_arr)
        
        if not ct_arr[-1].isdigit():
            s.write("GOGOGO\r\n")
        
        if ct >= samps:
            break
        #print buf
        #if (now - startTime >= 10):
        #    break
        
        #pb.updateProg(100 * (now - startTime) / 10)
        #prog = 100 * ct / samps
        #pb.updateProg(prog)
        
        #print "%i/%i %i%%" % (ct, samps, prog)
    
    csv_vals = ",".join(dataBuf.split("\r\n"))
    
    fh.write(csv_vals)
    fh.close()
    
    s.close()
    
    pb.close()

def main():
    # Initialize terminal colors for Windows (and do nothing for *nix)
    colorama.init()
    
    # Create the application and the main window
    app = QtGui.QApplication(sys.argv)
    
    # Setup stylesheet
    app.setStyleSheet(qdarkstyle.load_stylesheet(pyside = False))
    
    window = MDCWindow()
    
    # Run
    window.show()
    exit(app.exec_())

if __name__ == "__main__":
    main()
