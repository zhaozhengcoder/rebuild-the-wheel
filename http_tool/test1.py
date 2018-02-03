import socket 

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
url ='www.liaoxuefeng.com'
s.connect((url, 80))
s.send(b'GET /wiki/0014316089557264a6b348958f449949df42a6d3a2e542c000 HTTP/1.1\r\nHost: www.liaoxuefeng.com\r\nConnection: close\r\n\r\n')


buffer = []
while True:
    # 每次最多接收1k字节:
    d = s.recv(1024)
    if d:
        buffer.append(d)
    else:
        break
data = b''.join(buffer)

s.close()

header, html = data.split(b'\r\n\r\n', 1)
print(header.decode('utf-8'))