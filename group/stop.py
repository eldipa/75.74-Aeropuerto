
import time
import signal

def stop(process):
    if process:
       t = 10
       while process.poll() is None and t >= 0:
          try:
            process.send_signal(signal.SIGINT)
          except OSError: #TODO ver por que hay problemass aqui
            pass

          t += 1
          time.sleep(0.1)
       
       if process.poll() is None:
          try:
            process.send_signal(signal.SIGKILL)
          except OSError: #TODO ver por que hay problemass aqui
            pass

       process.wait()

