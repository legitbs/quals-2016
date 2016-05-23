import random
from os import environ, listdir, path
from sys import exit
from subprocess import Popen, PIPE
import signal
from base64 import b64decode

def alarm_handler(signum, frame):
    print "timed out, sorry"
    exit(-1)
signal.signal(signal.SIGALRM, alarm_handler)

input_timeout = int(environ.get('INPUT_TIMEOUT', 15))
crash_timeout = int(environ.get('CRASH_TIMEOUT', 5))

chall_path = "./tmp/gen/cb"
all_challs = set(listdir(chall_path))

picked = random.sample(all_challs, 10)

print "send your crash string as base64, followed by a newline"

for c in picked:
    print c
    signal.alarm(input_timeout)
    crasher = b64decode(raw_input())
    signal.alarm(0)

    signal.alarm(crash_timeout)
    proc = Popen(path.join(chall_path, c),
                 stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = proc.communicate(crasher)
    signal.alarm(0)

    if out != "canary ok":
        print "didn't pass canary, sorry"
        exit(-1)

    if proc.returncode != -signal.SIGSEGV:
        print "didn't segfault, sorry"
        exit(-1)

print "The flag is: {}".format(
    environ.get('FLAG', "you think this is the real quaid DeifCokIj"))
