#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>

void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}

int startup(u_short *port)
{
    int httpd = 0;
    int on = 1;
    struct sockaddr_in name;

    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)  
    {  
        error_die("setsockopt failed");
    }
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
        error_die("bind");
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, 5) < 0)
        error_die("listen");
    return(httpd);
}


int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    //只读一行，行尾的\n 表示结束
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        //如果读到一个字符
        if (n > 0)
        {
            //如果读到\r\n的时候，将以\r\n结尾的请求字符串，变成以\n结尾的字符串放到buf里面
            //如果看不懂MSG_PEEK的话，http://blog.csdn.net/g1036583997/article/details/49202405
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    //在buf的结尾写一个‘\0’　表示字符串的结尾
    buf[i] = '\0';
    //返回值i 表示buff的长度
    return(i);
}

//arg的参数表示的是，一个连接的client
void accept_request(void *arg)
{
    int client = (intptr_t)arg;
    char buf[1024];
    size_t numchars;
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    struct stat st;
    int cgi = 0;      /* becomes true if server decides this is a CGI program */
    char *query_string = NULL;

    //将读到的buff输出
	numchars = get_line(client, buf, sizeof(buf));
	printf("\n buff : %s \n",buf);
	while(numchars > 0){
    	numchars = get_line(client, buf, sizeof(buf));
		printf("\n buff : %s \n",buf);
	}

}


//main
//test1.c 搞定到了get_line 函数
int main(){
	int server_sock = -1;
    u_short port = 4000;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    pthread_t newthread;
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);
	//printf("socket : %d",server_sock);

	while (1)
    {
		//等待一个socket连接
        client_sock = accept(server_sock,(struct sockaddr *)&client_name,&client_name_len);
        if(client_sock == -1)
            error_die("accept");
		//对于一个socket连接，调用pthread_create 创建一个线程去处理, 创建线程执行的函数是accept_request,参数是client_sock
        if(pthread_create(&newthread , NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0)
            perror("pthread_create");
    }

    close(server_sock);
	return 0;
}
