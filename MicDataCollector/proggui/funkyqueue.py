import sys
import threading

# Pickle - attempt to get C version
try:
   import cPickle as pickle
except:
   import pickle

# Python 3 uses queue, Python 2 uses Queue
try:
    import queue
except ImportError:
    import Queue as queue

# Global variables to hold state
global inqueue, outqueue, inthread, outthread

inqueue = None
outqueue = None
inthread = None
outthread = None

# Thread to handle stdin without blocking for FunkyQueueStdIO.
# Note that this actually uses a real queue to handle queuing and
# dequeuing the input from stdin, allowing input handling to be
# non-blocking. This only handles input, not output - output uses a
# secondary thread.
class FunkyQueueStdIOThreadInput(threading.Thread):
    def __init__(self, inqueue):
        self.inqueue = inqueue
    
    def run(self):
        for line in sys.stdin:
            self.inqueue.put(line) 
        
        # This will run until sys.stdin hits EOF, or we terminate with
        # the process.

# Thread to handle stdout without blocking for FunkyQueueStdIO.
# Note that this actually uses a real queue to handle queuing and
# dequeuing the output being sent to stdout, allowing output handling to
# be non-blocking. This only handles output, not input - input uses a
# secondary thread.
class FunkyQueueStdIOThreadOutput(threading.Thread):
    def __init__(self, outqueue):
        self.outqueue = outqueue
        self.canStop = False
    
    def run(self):
        while self.canStop:
            try:
                out = self.outqueue.get(timeout = 0.1)
                sys.stdout.write(out)
                sys.stdout.flush()
            except queue.Empty:
                pass
        
        # This will run until we terminate with the process.
    
    def stopNow(self):
        self.canStop = True

# Function to create queues and threads, as necessary.
# If they already exist, just return them.
def makeSomeFunky():
    global inqueue, outqueue, inthread, outthread
    
    if not inqueue:
        inqueue = queue.Queue()
    
    if not outqueue:
        outqueue = queue.Queue()
    
    if not inthread:
        inthread = FunkyQueueStdIOThreadInput(inqueue)
        inthread.start()
    
    if not outthread:
        outthread = FunkyQueueStdIOThreadOutput(inqueue)
        outthread.start()
    
    return (inqueue, outqueue)

# FunkyQueueStdIO - a queue implemented using process standard IO.
# put() will result in stdout, and a get() will result in scanning from
# stdin.
# Note that this is NOT a real queue - it behaves more like a pipe
# (e.g. multiprocessing.Pipe).
class FunkyQueueStdIO:
    def __init__(self):
        self.inqueue, self.outqueue = makeSomeFunky()

    def empty(self):
        return self.items == []

    def put(self, ele):
        
    
    # Block = False -> ignore timeout, raise exception if empty
    # Block = True, timeout defined -> wait timeout, raise exception
    # afterwards if empty
    def get(self, block = True, timeout = None):
        # Note that we don't attempt to catch exceptions here...
        # The caller is expected to handle queue.Empty exceptions!
        if block:
            if not timeout:
                inline = self.inqueue.get(timeout = timeout)
            else:
                inline = self.inqueue.get()
        else:
            inline = self.inqueue.get(block = block)
        
        
