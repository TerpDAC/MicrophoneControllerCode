from PyQt4 import QtGui, QtCore
from proggui.progwin import ProgressWindow
from proggui.pguiproc import ProgressProcessMP, ProgressProcessSPPipe

import sys
import multiprocessing

# Python 3 uses queue, Python 2 uses Queue
try:
    import queue
except ImportError:
    import Queue as queue

import time

class ProgressBar:
    def __init__(self, cancel_callback = None, have_qapp = False, **kwargs):
        self.kwargs = kwargs
        
        # Check if a QApplication instance already exists.
        # If a QApplication is already initialized, this means that we
        # will have to do some interesting workarounds regarding queues
        # and multiprocessing.
        # 
        # This is due to the fact that with multiprocessing, on *nix
        # platforms, this is achieved with os.fork(). Forking means
        # creating a new process with a copy of the program's memory...
        # which includes everything, not just Python internal memory!
        # 
        # This includes PyQt's (and thus Qt's) C++ side, as well as any
        # other resources opened by PyQt/Qt. Copying Qt's memory data
        # doesn't make too much sense - if a window was open, fork()
        # would copy the window data... and that would result in two
        # windows.
        # 
        # Except that you wouldn't have two windows, you'd just
        # have very odd behavior since the forked process is trying to
        # access the same window on the screen, leading to weird
        # behavior and errors!
        # 
        # Hence, we need to copy only the data we need (almost nothing,
        # except for the queue), and work from there.
        # 
        # On the topic of queues - fork() works nicely though with
        # multiprocessing queues, since essentially multiprocessing
        # just pipes the queue data to the process.
        # 
        # If we are on Python >= 3.4, there's a new feature that lets
        # us configure how we want to launch our process - via spawning,
        # or simply just opening a new process (no fork)! Queues still
        # work (since it's just piping), so this lets us achieve
        # multiprocessing with PyQt successfully (and easily)!
        # 
        # If we are NOT on Python >= 3.4... then we need to play around
        # a bit. Thankfully, multiprocessing already provides a lot of
        # good infrastructure - we just have to write the way to
        # create a new process!
        # 
        
        # Assume that we're in the easy life!
        # 1 = multiprocessing, 2 = our implementation
        self.process_type = 1
        
        # First, check if we even need to care about this.
        if have_qapp or (QtCore.QCoreApplication.instance() != None):
            # OK, so we need to make sure we don't fork().
            print("ANTI-FORK required to run. Attempting to do that!")
            
            # If we are using Python >= 3.4, we can use this new little
            # trick (spawn method!) to make multiprocessing work with
            # PyQt.
            if (sys.version_info >= (3, 4)):
                multiprocessing.set_start_method('spawn')
            else:
                # We need to do our own tricks!
                self.process_type = 2
        
        if self.process_type == 1:
            # multiprocessing, as usual
            self.process_queue = multiprocessing.Queue()
            self.process = ProgressProcessMP(self.process_queue, **kwargs)
        else:
            # Do alternative method
            raise Exception("Anti-fork for python <3.4 not implemented yet!")
            pass
        
        self.cancel_callback = cancel_callback
        
        self.closed_cleanly = None
        
        self.proc_started = False
        
    def show(self):
        if not self.proc_started:
            multiprocessing.freeze_support()
            self.process.start()
            self.proc_started = True
        else:
            self.showWindow()
    
    def hide(self):
        self.hideWindow()
    
    def close(self):
        self.closeWindow()
    
    # Internal/external function
    # Check if the process is still alive. If it is, return True.
    # Otherwise, return False.
    def checkProcessState(self):
        if not self.process.is_alive():
            if (self.closed_cleanly == None):
                # Search the queue for our exit message!
                while True:
                    try:
                        req = self.process_queue.get(timeout = 1)
                    except queue.Empty:
                        break
                    
                    if len(req) > 0:
                        if req[0] == "closed" and (len(req) == 2):
                            self.closed_cleanly = req[1]
                            
                if not self.closed_cleanly:
                    if self.cancel_callback:
                        self.cancel_callback()
            
            return False
        else:
            return True
    
    def isOpen(self):
        return self.checkProcessState()
    
    def isClosed(self):
        return not self.checkProcessState()
    
    # Internal function - raises if already exited
    def checkExited(self):
        if not self.checkProcessState():
            raise Exception("GUI already exited!")
    
    def setStatus(self, status):
        self.checkExited()
        self.process_queue.put(["setStatus", status])
    
    def setProgress(self, progress):
        self.checkExited()
        self.process_queue.put(["setProgress", progress])
    
    def setCancelLbl(self, cancel_label):
        self.checkExited()
        self.process_queue.put(["setCancelLbl", cancel_label])
    
    def setCancelEnabled(self, en):
        self.checkExited()
        self.process_queue.put(["setCancelEnabled", en])
    
    def enableCancel(self):
        self.checkExited()
        self.process_queue.put(["enableCancel"])
    
    def disableCancel(self):
        self.checkExited()
        self.process_queue.put(["disableCancel"])
    
    def setPulseEnabled(self, en):
        self.checkExited()
        self.process_queue.put(["setPulseEnabled", en])
    
    def enablePulse(self):
        self.checkExited()
        self.process_queue.put(["enablePulse"])
    
    def disablePulse(self):
        self.checkExited()
        self.process_queue.put(["disablePulse"])
    
    def closeWindow(self):
        self.checkExited()
        self.process_queue.put(["closeWindow"])
    
    def hideWindow(self):
        self.checkExited()
        self.process_queue.put(["hideWindow"])
    
    def showWindow(self):
        self.checkExited()
        self.process_queue.put(["showWindow"])
    
    def finish(self):
        self.closeWindow()
        self.process.join()
    
    def __del__(self):
        self.process.join()
