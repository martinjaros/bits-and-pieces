#!/usr/bin/python
# This script emulates network jitter in UDP sockets

from socket import *
from time import time
from random import random, gauss
from Queue import PriorityQueue

LOCAL = ('', 5000)
HOST = ('127.0.0.1', 5002)
DELAY = 0.5
JITTER = 0.1
LOSS = 0.1

sock = socket(AF_INET, SOCK_DGRAM)
sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sock.bind(LOCAL)
sock.connect(HOST)
queue = PriorityQueue()

print "Local  %s:%d" % (LOCAL[0], LOCAL[1])
print "Host   %s:%d" % (HOST[0], HOST[1])
print "Delay  %dms" % (DELAY * 1000)
print "Jitter %dms" % (JITTER * 1000)
print "Loss   %d%%" % (LOSS * 100)

while True:
    if queue.empty():
        sock.settimeout(None)
        item = None
    else:
        while True:
            item = queue.get()
            delta = item[0] - time()
            if delta > 0:
                sock.settimeout(delta)
                break;
            else:
                sock.send(item[1])
    try:
        data = sock.recv(1024)
        if not random() < LOSS:
            queue.put((time() + gauss(DELAY, JITTER), data))
        if item:
            queue.put(item)
    except timeout:
        sock.send(item[1])
