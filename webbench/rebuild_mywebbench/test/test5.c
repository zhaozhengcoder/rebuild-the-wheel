#include "socket.c"
#include <unistd.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <strings.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#define REQUEST_SIZE 2048
char request[REQUEST_SIZE];   // 发送的构造的HTTP请求

//这个是一个单进程的，产生http请求
int main(){
    //统计
    int failed=0;
    int bytes=0;
    int speed=0;
    //负责缓存服务器的相应
    char buf[1500];
    //构造请求
    strcpy(request,"GET / HTTP/1.0");
    strcat(request,"\r\n");
    strcat(request,"User-Agent: WebBench 1.5");
    strcat(request,"\r\n");
    strcat(request,"Host: localhost");
    strcat(request,"\r\n");
    strcat(request,"\r\n");
    int rlen=strlen(request);
    printf("----test ----- the http request is ----   : \n");
    printf("%s",request);
    printf("----end  ------\n");

    //host and port 
    char *host="localhost";
    int port=80;

    int times=10;
    while(times--){
        int s=Socket(host,port);
        if(s<0){
            failed++;        
            printf("error \n");
            return -1;
        }
        else{
            printf("ok \n");
        }

        //write  发送请求
        if(rlen!=write(s,request,rlen)){
            failed++;
            printf("fail \n");
            close(s);
            return -1;
        }
        printf("write len is %d  \n",rlen);

        //read　不停的循环去读取，直到是空，表示读完
        int i=0;
        while(1){
            i=read(s,buf,1500);
            printf("read len i is : %d \n",i);
            if(i<0){
                printf("fail \n");
                close(s);
                return -1;
            }
            if(i==0){
                //printf("%s",buf);
                printf("read comlete \n");
                break;
            }
            if(i>0){
                //printf("%s",buf);
                bytes+=bytes+i;
            }
        }//end read
        close(s);
        speed++;   //表示完成了一个完整的read和write
    }

    printf("speed is %d ,fail is %d , bytes is %d  \n",speed,failed,bytes);
    return 0;
}