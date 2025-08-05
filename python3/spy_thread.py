import threading
import time
from datetime import datetime
spy_count = 0
sem1 = threading.Semaphore(0)
sem2 = threading.Semaphore(0)
def thread_routine(idd, delay):
    iscontinue = 1
    global spy_count
    while iscontinue:
        if idd == 1:
            sem1.acquire()
        else:
            sem2.acquire()
        spy_count += 1
        if spy_count > 10:
            iscontinue = False
        now = datetime.now()
        print(f"Thread ID = {threading.get_ident()}, \tspy_count = {spy_count},\tFormatted: {now.strftime('%Y-%m-%d %H:%M:%S')}")
        time.sleep(delay)
        if idd == 1:
            sem2.release()
        else:
            sem1.release()        


def spy_thread():
    t1 = threading.Thread(target=thread_routine, args=(1, 1))
    t2 = threading.Thread(target=thread_routine, args=(2, 2))
    t1.start()
    t2.start()
    now = datetime.now()
    print(f"Formatted: {now.strftime('%Y-%m-%d %H:%M:%S')}")
    time.sleep(10)
    sem1.release()
    t1.join()
    t2.join()    
    

spy_thread()