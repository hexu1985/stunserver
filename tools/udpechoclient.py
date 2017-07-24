#!/usr/bin/env python
# UDP Example - Chapter 2

import socket, sys, time

if len(sys.argv) < 3 or len(sys.argv) > 4:
    print "usage: udpechoclient01.py <hostname> <service> [localport]"
    sys.exit(1)

host = sys.argv[1]
textport = sys.argv[2]

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

if len(sys.argv) == 4:
    local_port = int(sys.argv[3])
    s.bind(("", local_port))

try:
    port = int(textport)
except ValueError:
    # That didn't work.  Look it up instread.
    port = socket.getservbyname(textport, 'udp')

print "Looking for replies; press Ctrl-C or Ctrl-Break to stop."
while 1:
    data = sys.stdin.readline().strip()
    if not len(data):
        break
    s.sendto(data, (host, port))
    buf = s.recv(2048)
    if not len(buf):
        break
    print "Received: %s" % buf

s.close()
