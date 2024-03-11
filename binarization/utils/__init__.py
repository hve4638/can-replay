import sys, os
import time
from threading import Thread

def script_path():
    srcpath = os.path.abspath(sys.argv[0]).replace('\\','/')
    return os.path.dirname(srcpath).replace('\\','/')

def mkdir_p(directory):
    if not os.path.exists(directory):
        os.mkdir(directory)

waitcurse_index = 0
waitcurse_str = "|/-\\"
def waitcurse(th:Thread):
    global waitcurse_index
    while th.is_alive():
        sys.stdout.write(waitcurse_str[waitcurse_index % len(waitcurse_str)])
        sys.stdout.write("\r")
        waitcurse_index += 1
        time.sleep(0.3)

def extract_signiture(source:str):
    _, file = os.path.split(source)
    name, _ = os.path.splitext(file)
    return name