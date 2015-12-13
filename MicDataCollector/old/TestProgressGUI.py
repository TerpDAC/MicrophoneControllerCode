import time

from proggui import ProgressBar

def stopCallback():
    print("Callback triggered - GUI closed!")

def main():
    pb = ProgressBar(title="Testing...", status="Testing 1, 2, 3...", pulsing = True, cancel_callback = stopCallback)
    pb.show()

    time.sleep(1)
    pb.disablePulse()

    for i in xrange(1, 101):
        if pb.isClosed():
            break
        else:
            pb.setProgress(i)
            
            if i > 80:
                pb.setStatus("Almost done...")
            elif i > 60:
                pb.setStatus("Halfway there (and some)...")
                pb.enableCancel()
            elif i > 40:
                pb.setStatus("Working on it...")
                pb.disableCancel()
            elif i > 20:
                pb.setStatus("Running the gears...")
            
        time.sleep(0.1)

    if pb.isOpen():
        pb.close()

if __name__ == "__main__":
    main()
