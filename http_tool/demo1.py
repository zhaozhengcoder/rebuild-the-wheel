import socket
import re 
    
def parse_response(data):
    print (">> 服务器的响应 >>")
    #print (data.decode('utf-8'))
    header, html = data.split(b'\r\n\r\n', 1)
    print (header.decode('utf-8'))
    #print (html.decode('utf-8'))

def show_request(head,request_lines):
    print (">> 发送的http请求 >>")
    print (head[:-2])    #切掉后面的/r/n
    for item in request_lines:
        print (item[:-2])


def build_http_request(host,path,isshow=True):
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

def parse_url(url):
    method = url.split(':')[0]
    host = re.findall(r'//(.+?)/',url)[0]
    path = url.strip().split(host)[-1]
    print (">> 解析url >>")
    print ("method : ",method)
    print ("host : ",host)
    print ("path : ",path)
    return method,host,path
    


if __name__ =="__main__":
    #url ='https://github.com/zhaozhengcoder/Machine-Learning'
    url2 ='https://www.sina.com.cn/'
    url3 ='https://www.liaoxuefeng.com/'
    url4 ='http://hello.tongji.edu.cn/'  #现在还不支持https的协议，所以如果一个https的协议的话，就会location 重定向

    method,host,path = parse_url(url4)
    http_request = build_http_request(host,path)

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


"""

这个版本的问题是：
1. 不支持https的请求 ，因为使用了socket.connect 的函数，太底层了，是不是用request 库很好一点？
2. parse_response 函数解析html的时候，如果这个页面里面有中文，就可能出bug，原因应该是中文编码造成的，但是没想好怎么解决
3. 看看request 库 ，或 curl的是实现，能不能找到一些灵感

解决的想法：
1. 关于https的问题
    看来一个https，是在http基础上面+ssl 实现了一个加密的传输，如果自己搞出来，估计时间成本太高了。所以，打算使用requests库，来支持https协议。
"""

