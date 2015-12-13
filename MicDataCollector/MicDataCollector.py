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
from proggui import ProgressBar

global pb

class WorkThread(QtCore.QThread):
    def __init__(self, window, func, *args, **kwargs):
        QtCore.QThread.__init__(self)
        self.window = window
        self.func = func
        self.args = args
        self.kwargs = kwargs
        self.result = None
    
    def run(self):
        self.result = self.func(*self.args, **self.kwargs)
        self.window.finishedWork.emit()
    
    def getResult(self):
        return self.result

class MDCWindow(QtGui.QDialog, MicDataCollectorGUI.Ui_mdcDialog):
    finishedWork = QtCore.pyqtSignal()
    
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
        
        self.workThread = None
        self.task = None
        
        self.refreshSerialPorts()
        self.refreshBtn.clicked.connect(self.refreshSerialPorts)
        
        self.startBtn.clicked.connect(self.startCollection)
        
        self.finishedWork.connect(self.doneWork)
    
    def closeEvent(self, event):
        global pb
        pb.close()
    
    def doneWork(self):
        if self.task == "refreshSerialPorts":
            self.ports = self.workThread.getResult()
            self.serialPortCombo.clear()
            self.serialPortCombo.addItems(self.ports)
        elif self.task == "startCollection":
            pass
        else:
            print("WARNING: invalid task specified!")
        
        self.task = None
        self.workThread = None
        
        self.serialPortCombo.setEnabled(True)
        self.refreshBtn.setEnabled(True)
        self.startBtn.setEnabled(True)
    
    def freeze(self):
        self.serialPortCombo.setEnabled(False)
        self.refreshBtn.setEnabled(False)
        self.startBtn.setEnabled(False)
    
    def finishSerialData(self, result):
        self.ports = result.val
        
        self.serialPortCombo.clear()
        self.serialPortCombo.addItems(self.ports)
        
    def refreshSerialPorts(self):
        #self.rsp_thread = ExecThread(self.finishSerialData, get_possible_serial_ports, QtGui.QApplication.instance())
        #self.ports = get_possible_serial_ports(QtGui.QApplication.instance())
        #self.rsp_thread.start()
        self.freeze()
        if not self.workThread:
            self.workThread = WorkThread(self, get_possible_serial_ports)
            self.task = "refreshSerialPorts"
            self.workThread.start()
            
        #self.ports = get_possible_serial_ports(QtGui.QApplication.instance())
        
    def startCollection(self):
        port = self.serialPortCombo.currentText()
        if port:
            csvfile = QtGui.QFileDialog.getSaveFileName(self, "Save data to:", os.path.abspath(os.curdir), "CSV Data Files (*.csv)")
            if csvfile:
                self.freeze()
                self.workThread = WorkThread(self, get_data, self.serialPortCombo.currentText(), csvfile)
                self.task = "startCollection"
                self.workThread.start()
        else:
            print("No serial port selected! Connect the Arduino, and then click Refresh Serial Ports to redetect your Arduino.")
            QtGui.QMessageBox.critical(self, "Error",
                "No serial port selected! Connect the Arduino, and then click Refresh Serial Ports to redetect your Arduino.",
                QtGui.QMessageBox.Ok)

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

def get_possible_serial_ports():
    global pb
    q = Queue.Queue()
    
    # Progress bar
    pb.setProgress(0)
    pb.enablePulse()
    pb.show()
    
    print("Searching for serial ports (this may take some time)...")
    pb.setStatus("Searching for serial ports (this may take some time)...")
    
    s_ports = serial_ports()
    
    if len(s_ports) <= 0:
        print("No open serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.")
        QtGui.QMessageBox.critical(None, "Error",
                "No open serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.",
                QtGui.QMessageBox.Ok)
        #pb.close()
        pb.hide()
        return []
    
    pb.disablePulse()
    pb.setStatus("Found serial ports, scanning for mic sensor...")
    pb.setProgress(50)
    
    uno_ports = prune_ports_uno(s_ports)
    
    if len(uno_ports) <= 0:
        print("No open Arduino serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.")
        QtGui.QMessageBox.critical(None, "Error",
                "No open Arduino serial ports found! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized.",
                QtGui.QMessageBox.Ok)
        
        #pb.close()
        pb.hide()
        
        return []
    
    pb.setProgress(100)
    #pb.close()
    pb.hide()
    
    return uno_ports

def get_data(port, csvfile):
    global pb
    q = Queue.Queue()
    
    # Progress bar
    #pb = ProgressWindow(title="Data Collection", status="Now collecting data!", pulsing = True, comm_queue = q)
    pb.setStatus("Now collecting data...")
    pb.enablePulse()
    pb.setProgress(0)
    pb.show()
    
    try:
        fh = open(csvfile, "w")
    except OSError:
        print("Could not open data file for writing! Make sure you have enough permissions to write to this file.")
        QtGui.QMessageBox.critical(None, "Error",
                "Could not open data file for writing! Make sure you have enough permissions to write to this file.",
                QtGui.QMessageBox.Ok)
        pb.hide()
        return
    
    startTime = time.time()
    
    dataBuf = ""
    print str(port)
    try:
        s = serial.Serial(str(port), 9600, timeout = 0.1)
    except (OSError, serial.SerialException):
        print("Could not open serial port! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized. Also, try unplugging and replugging the Arduino to reset its state.")
        QtGui.QMessageBox.critical(None, "Error",
                "Could not open serial port! Make sure you have enough permissions to open a serial port, and/or the device is plugged in and recognized. Also, try unplugging and replugging the Arduino to reset its state.",
                QtGui.QMessageBox.Ok)
        pb.hide()
        return
    
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
        
        if ct > 0:
            pb.disablePulse()
            pb.setProgress(100 * ct / samps)
        #prog = 100 * ct / samps
        #pb.updateProg(prog)
        
        #print "%i/%i %i%%" % (ct, samps, prog)
    
    csv_vals = ",".join(dataBuf.split("\r\n"))
    
    fh.write(csv_vals)
    fh.close()
    
    s.close()
    
    #pb.close()
    pb.hide()

def main():
    global pb
    pb = ProgressBar(title="Microphone Data Collector", status="Starting...", pulsing = True)
    pb.show()
    pb.hide()
    pb.disableCancel()
    
    # Initialize terminal colors for Windows (and do nothing for *nix)
    colorama.init()
    
    # Create the application and the main window
    app = QtGui.QApplication(sys.argv)
    
    # Setup stylesheet
    app.setStyleSheet(qdarkstyle.load_stylesheet(pyside = False))
    
    window = MDCWindow()
    
    # Run
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
