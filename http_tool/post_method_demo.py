import socket
import requests
import re 

def parse_response(data):
    print (">> 服务器的响应 >>")
    #print (data.decode('utf-8'))
    header, html = data.split(b'\r\n\r\n', 1)
    print (header.decode('utf-8'))
    print (html.decode('utf-8'))


head = 'POST /login HTTP/1.1\r\n'
request_lines=[]
request_lines.append('Host: 127.0.0.1:5555\r\n')
request_lines.append('Connection: keep-alive\r\n')
request_lines.append('Content-Length: 31\r\n')
request_lines.append('Cache-Control: max-age=0\r\n')
request_lines.append('Origin: http://127.0.0.1:5555\r\n')
request_lines.append('Upgrade-Insecure-Requests: 1\r\n')
request_lines.append('Content-Type: application/x-www-form-urlencoded\r\n')
request_lines.append('User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36\r\n')
request_lines.append('Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n')
request_lines.append('Referer: http://127.0.0.1:5555/login\r\n')
request_lines.append('Accept-Encoding: gzip, deflate, br\r\n')
request_lines.append('Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n')

body ='username=312&password=123123123'

ret =head.encode('ascii')
for item in request_lines:
    ret+=item.encode('ascii')
ret+=body.encode('ascii')

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('127.0.0.1', 5555))
s.send(ret)

buffer = []
while True:
    d = s.recv(2048)
    if d:
        buffer.append(d)
    else:
        break
data = b''.join(buffer)
s.close()
parse_response(data)