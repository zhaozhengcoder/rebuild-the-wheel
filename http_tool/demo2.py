import socket
import requests
import re 

def parse_response(data):
    print (">> 服务器的响应 >>")
    #print (data.decode('utf-8'))
    header, html = data.split(b'\r\n\r\n', 1)
    print (header.decode('utf-8'))
    print (html.decode('utf-8'))

def show_request(head,request_lines):
    print (">> 发送的http请求 >>")
    print (head[:-2])    #切掉后面的/r/n
    for item in request_lines:
        print (item[:-2])


def build_http_get_request(host,path,isshow=True):
    head = 'GET {path} HTTP/1.1\r\n'.format(path=path)
    request_lines = []
    request_lines.append('Host: {host}\r\n'.format(host = host))
    request_lines.append('User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36\r\n')
    request_lines.append('Connection:close\r\n')
    request_lines.append('Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n')
    request_lines.append('Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n')   # 注意 请求结束的时候，这里有两个\r\n
    if isshow == True:
        show_request(head,request_lines)

    ret =head.encode('ascii')
    for item in request_lines:
        ret += item.encode('ascii')
    return ret


# 构造一个post请求
def build_http_post_request(host,path,port=80,post_data={'username':'value1','password':'value2'},isshow=True):
    body = ''
    for key in post_data:
        body +=key+'='+post_data[key]+'&'
    if body[-1]=='&':
        body=body[:-1]
    #body ='username=312&password=123123123'
    
    head = 'POST {path} HTTP/1.1\r\n'.format(path=path)
    request_lines = []
    request_lines.append('Host: 127.0.0.1:{port}\r\n'.format(port=port))
    
    request_lines.append('User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36\r\n')
    request_lines.append('Connection:close\r\n')
    request_lines.append('Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n')
    request_lines.append('Content-Length: '+str(len(body))+'\r\n')
    request_lines.append('Content-Type: application/x-www-form-urlencoded\r\n')  #这个很重要
    request_lines.append('Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n')
    
    if isshow == True:
        show_request(head,request_lines)

    ret =head.encode('ascii')
    for item in request_lines:
        ret += item.encode('ascii')
    ret+=body.encode('ascii')
    return ret


def parse_url(url):
    method = url.split(':')[0]
    host = re.findall(r'//(.+?)/',url)[0]
    path = url.strip().split(host)[-1]
    print (">> 解析url >>")
    print ("method : ",method)
    print ("host : ",host)
    print ("path : ",path)
    return method,host,path


def https_method_get(url,host):
    headers = {
        'host':host,
        'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36',
    }
    print (">> 发送的http请求 >>")
    for key in headers:
        print (key," : ",headers[key])
    r = requests.get(url3,headers=headers)
    print (">> 服务器的响应 >>")
    print (r.status_code)
    for key in r.headers:
        print (key," : ",r.headers[key])

def http_method_get(host,path):
    http_request = build_http_get_request(host,path)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, 80))
    s.send(http_request)

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

def http_method_post(host,path,port,post_data):
    http_request = build_http_post_request(host,path,port=port,post_data=post_data)
    #print (http_request)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.send(http_request)

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



if __name__=="__main__":
    url = 'http://hello.tongji.edu.cn/'
    url1= 'http://127.0.0.1/login'
    url3 ='https://www.liaoxuefeng.com/'
    method ='post'
    

    protocol,host,path = parse_url(url1)
    
    if (method == 'get'):
        if (protocol=="https"):
            https_method_get(url,host)
        else:
            http_method_get(host,path)
    
    if (method =='post'):
        port = 5555
        post_data = {'username':'value1','password':'value2'}
        http_method_post(host,path,port=port,post_data=post_data)
        
 
"""
1. demo2的版本在demo版本上面 ，使用requests库，支持了对https的支持。当然，requests也很好的支持http，但是我也是用了我的代码（在socket层是实现的http，这样可能对底层有一个很好的支持）

2. demo支持了从基于socket是实现的post的方法（在requests库里面 对post已经有很好的支持了，but 同样的道理，还是打算自己是实现一下）
"""
