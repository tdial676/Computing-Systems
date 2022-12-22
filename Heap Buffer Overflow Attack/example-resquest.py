import socket

from stage1 import DATA

METHOD = b"GET/"
URL = b"/"
HEADERS = b"Host: www.example.com"

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect(("example.com", 80))
request = METHOD + b" " + URL + b"\r\n" + HEADERS + b"\r\n\r\n" + DATA
print(request)
client.send(request)
response = client.recv(4096)
print(response.decode())