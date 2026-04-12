import socket

msg = b"Tcp server test"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect(("192.168.0.91", 8080))
s.send(msg)
data = s.recv(len(msg))
s.shutdown(socket.SHUT_RDWR)
s.close()

print(data)
