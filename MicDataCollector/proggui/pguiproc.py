from PyQt4 import QtGui, QtCore
import sys
import multiprocessing
from proggui import qdarkstyle

from proggui.progwin import ProgressWindow

class ProgressProcess:
    def __init__(self, comm_queue, **kwargs):
        self.comm_queue = comm_queue
        self.kwargs = kwargs
        
        self.closed_cleanly = None
    
    def run(self):
        self.app = QtGui.QApplication(sys.argv)
        self.app.setStyleSheet(qdarkstyle.load_stylesheet(pyside = False))
        self.window = ProgressWindow(comm_queue = self.comm_queue, **self.kwargs)
        self.window.open()
        
        return(self.app.exec_())

class ProgressProcessMP(ProgressProcess, multiprocessing.Process):
    def __init__(self, comm_queue, **kwargs):
        multiprocessing.Process.__init__(self)
        ProgressProcess.__init__(self, comm_queue, **kwargs)

# Jump - jump by creating another process, and running code there!
# This class actually pretends to run the code - it actually
# runs this file, and passes the data via pipe.
class ProgressProcessJump(ProgressProcess):
    def run(self):
        proc = subprocess.Popen([sys.executable] + sys.argv, 
                        stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE,
                        )
        proc.communicate()

# SPPipe = SubProcess + Pipe
# The class that actually handles running the code. Uses the piped
# stdin for commands.
class ProgressProcessSPPipe(ProgressProcess):
    def run(self):
        print("RUNNING")
