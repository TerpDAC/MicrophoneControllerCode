from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import pyqtSignal
from gui import ProgGUI
import Queue

import time

class ProgressQueueThread(QtCore.QThread):
    def __init__(self, comm_queue, window, debug = False):
        QtCore.QThread.__init__(self)
        self.comm_queue = comm_queue
        self.window = window
        self.debug = debug
        
        self._running = True
        
        self.commandMap = {
                            "setStatus"        : self.funcMap(self.setStatus, 1),
                            "setProgress"      : self.funcMap(self.setProgress, 1),
                            "setCancelLbl"     : self.funcMap(self.setCancelLbl, 1),
                            "setCancelEnabled" : self.funcMap(self.setCancelEnabled, 1),
                            "closeWindow"      : self.funcMap(self.closeWindow, 0),
                            "enableCancel"     : self.funcMap(self.setCancelEnabled, 1, [True]),
                            "disableCancel"    : self.funcMap(self.setCancelEnabled, 1, [False]),
                            "setPulseEnabled"  : self.funcMap(self.setPulseEnabled, 1),
                            "enablePulse"      : self.funcMap(self.setPulseEnabled, 1, [True]),
                            "disablePulse"     : self.funcMap(self.setPulseEnabled, 1, [False]),
                          }
    def error(self, error):
        raise Exception(error)
    
    def funcMap(self, func, num_args, args = None):
        if args:
            if self.debug:
                print("fMap: args DEFD, func = %s, num_args = %i, args = %s" % (str(func), num_args, str(args)))
            if len(args) < num_args:
                self.error("Not enough arguments for function! (Pre-defined arguments were given.) Arguments provided: %i / %i" % (len(args), num_args))
            return (lambda x: func(*args) if len(args) == num_args else self.error("Incorrect number of arguments for function! (Pre-defined arguments were given, now in lambda.) Arguments provided: %i / %i" (len(args), num_args)))
        else:
            if self.debug:
                print("fMap: args NDEF, func = %s, num_args = %i, args = %s" % (str(func), num_args, str(args)))
            return (lambda a: func(*a) if len(a) == num_args else self.error("Incorrect number of arguments for function! Arguments provided: %i / %i" % (len(a), num_args)))
    
    def __del__(self):
        self.wait()
    
    def stop(self):
        if self.debug:
            print("[ProgressQueueThread] Got indicator to stop!")
        self._running = False
    
    def run(self):
        while self._running:
            if self.debug:
                print("[ProgressQueueThread] Checking...")
            try:
                req = self.comm_queue.get(timeout = 0.1)
            except Queue.Empty:
                continue
            
            # Request format:
            # [ "command", arg1, arg2... ]
            
            if len(req) <= 0:
                self.error("Got junk queue message!")
            
            if req[0] in self.commandMap:
                if self.debug:
                    print("CMD: "+req[0])
                self.commandMap[req[0]](req[1:])
            else:
                self.error("Invalid command for queue message!")
        if self.debug:
            print("[ProgressQueueThread] ENDING!")
        #self.terminate()
    
    def setStatus(self, status):
        self.window.statusUpdate.emit(status)
    
    def setProgress(self, progress):
        self.window.progUpdate.emit(progress)
    
    def setCancelLbl(self, cancel_label):
        self.window.cancelLblUpdate.emit(cancel_label)
    
    def setCancelEnabled(self, en):
        self.window.cancelEnableUpdate.emit(en)
    
    def enableCancel(self):
        self.setCancelEnabled(True)
    
    def disableCancel(self):
        self.setCancelEnabled(False)
    
    def setPulseEnabled(self, en):
        if self.debug:
            print("setPulseEnabled: "+str(en))
        self.window.pulseEnableUpdate.emit(en)
    
    def enablePulse(self):
        self.setPulseEnabled(True)
    
    def disablePulse(self):
        self.setPulseEnabled(False)
    
    def closeWindow(self):
        self.window.closeUpdate.emit()
        

class ProgressWindow(QtGui.QDialog, ProgGUI.Ui_progDlg):
    progUpdate = pyqtSignal(int)
    statusUpdate = pyqtSignal(str)
    cancelLblUpdate = pyqtSignal(str)
    cancelEnableUpdate = pyqtSignal(bool)
    pulseEnableUpdate = pyqtSignal(bool)
    closeUpdate = pyqtSignal()
    
    def __init__(self, title = None, status = None, cur_progress = 0,
        cancel_label = None, cancel_enabled = True, pulsing = False,
        parent = None, comm_queue = None, debug = None):
        super(self.__class__, self).__init__(parent, QtCore.Qt.WindowSystemMenuHint | QtCore.Qt.WindowTitleHint | QtCore.Qt.MSWindowsFixedSizeDialogHint)
        self.setupUi(self)  # This is defined in design.py file automatically
                            # It sets up layout and widgets that are defined
        
        self.debug = debug
        
        if title:
            self.setWindowTitle(title)
        
        if status:
            self.loadingLbl.setText(status)
        
        if cancel_label:
            self.cancelBtn.setText(cancel_label)
        
        self.progressBar.setValue(cur_progress)
        self.cancelBtn.setEnabled(cancel_enabled)
        
        self.cancelBtn.clicked.connect(self.cancelCloseHandle)
        
        self.progUpdate.connect(self.updateProg)
        self.statusUpdate.connect(self.updateStatus)
        self.cancelLblUpdate.connect(self.updateCancelLbl)
        self.cancelEnableUpdate.connect(self.updateCancelEnable)
        self.closeUpdate.connect(self.updateClose)
        self.pulseEnableUpdate.connect(self.updatePulse)
        
        self.comm_queue = comm_queue
        
        self.regular_close = False
        self.can_close = True
        
        self.updatePulse(pulsing)
        
        self.progress_qthread = ProgressQueueThread(self.comm_queue, self)
        self.progress_qthread.start()
    
    def closeEvent(self, event):
        if not self.can_close:
            event.ignore()
            return
        
        self.progress_qthread.stop()
        
        if self.debug:
            print("Waiting for progress thread to stop!")
        
        self.progress_qthread.wait()
        
        if self.debug:
            print("Progress thread stopped, exiting!")
        
        self.comm_queue.put(["closed", self.regular_close])
        event.accept()
    
    def cancelCloseHandle(self):
        self.close()
    
    def updateProg(self, progress):
        self.progressBar.setValue(progress)
    
    def updateStatus(self, status):
        self.loadingLbl.setText(status)
        
    def updateCancelLbl(self, cancel_label):
        self.cancelBtn.setText(cancel_label)
        
    def updateCancelEnable(self, cancel_enable):
        self.cancelBtn.setEnabled(cancel_enable)
        self.can_close = cancel_enable
    
    def updatePulse(self, pulse):
        if pulse:
            if self.debug:
                print("Enabling pulse!")
            self.progressBar.setRange(0, 0)
        else:
            if self.debug:
                print("Disabling pulse!")
            self.progressBar.setRange(0, 100)
    
    def updateClose(self):
        self.can_close = True
        self.regular_close = True
        self.close()
    
