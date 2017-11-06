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


#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_OPTIONS 2
#define METHOD_TRACE 3
#define PROGRAM_VERSION "1.5"



int method=METHOD_GET;
int force=0;
int force_reload=0;
int clients=1;    //并发数
int proxyport=80;
char *proxyhost=NULL;
int http10=1;
int benchtime=30; //压力测试的时间



char host[MAXHOSTNAMELEN]; // 目标主机地址 
#define REQUEST_SIZE 2048
char request[REQUEST_SIZE];   // build_request函数修改的对象，发送的构造的HTTP请求

// 管道，用于父子进程通信
int mypipe[2];

//往管道里面写到内容
int speed=0; // 记录进程成功得到服务器相应的数量，即成功数
int failed=0; // 记录失败的数目
int bytes=0;  // 记录进程成功读取的字节数，当force = 0时有效

static const struct option long_options[]=
{
 {"force",no_argument,&force,1},
 {"reload",no_argument,&force_reload,1},
 {"time",required_argument,NULL,'t'},
 {"help",no_argument,NULL,'?'},
 {"http09",no_argument,NULL,'9'},
 {"http10",no_argument,NULL,'1'},
 {"http11",no_argument,NULL,'2'},
 {"get",no_argument,&method,METHOD_GET},
 {"head",no_argument,&method,METHOD_HEAD},
 {"options",no_argument,&method,METHOD_OPTIONS},
 {"trace",no_argument,&method,METHOD_TRACE},
 {"version",no_argument,NULL,'V'},
 {"proxy",required_argument,NULL,'p'},
 {"clients",required_argument,NULL,'c'},
 {NULL,0,NULL,0}
};

static void usage(void)
{
   printf(
	"webbench [option]... URL\n"
	"  -f|--force               Don't wait for reply from server.\n"
	"  -r|--reload              Send reload request - Pragma: no-cache.\n"
	"  -t|--time <sec>          Run benchmark for <sec> seconds. Default 30.\n"
	"  -p|--proxy <server:port> Use proxy server for request.\n"
	"  -c|--clients <n>         Run <n> HTTP clients at once. Default one.\n"
	"  -9|--http09              Use HTTP/0.9 style requests.\n"
	"  -1|--http10              Use HTTP/1.0 protocol.\n"
	"  -2|--http11              Use HTTP/1.1 protocol.\n"
	"  --get                    Use GET request method.\n"
	"  --head                   Use HEAD request method.\n"
	"  --options                Use OPTIONS request method.\n"
	"  --trace                  Use TRACE request method.\n"
	"  -?|-h|--help             This information.\n"
	"  -V|--version             Display program version.\n"
	);
};


/* test1.c 看懂了usage 
   test2.c 看懂了buildrequest函数，构造HTTP请求头
   test3.c 看懂了bench 函数
*/

// 构造HTTP请求头
void build_request(const char *url)
{
    char tmp[10];
    int i;

    bzero(host,MAXHOSTNAMELEN);
    bzero(request,REQUEST_SIZE);

    if(force_reload && proxyhost!=NULL && http10<1) http10=1;
    if(method==METHOD_HEAD && http10<1) http10=1;
    if(method==METHOD_OPTIONS && http10<2) http10=2;
    if(method==METHOD_TRACE && http10<2) http10=2;

    switch(method)
    {
        default:
        case METHOD_GET: strcpy(request,"GET");break;
        case METHOD_HEAD: strcpy(request,"HEAD");break;
        // 该请求方法的相应不能缓存
        case METHOD_OPTIONS: strcpy(request,"OPTIONS");break;
        case METHOD_TRACE: strcpy(request,"TRACE");break;
    }
		  
    strcat(request," ");

    if(NULL==strstr(url,"://"))
    {
        fprintf(stderr, "\n%s: is not a valid URL.\n",url);
        exit(2);
    }
  
    if(strlen(url)>1500)
    {
        fprintf(stderr,"URL is too long.\n");
        exit(2);
    }
    
    if(proxyhost==NULL)
        if (0!=strncasecmp("http://",url,7)) // 未使用代理服务器的情况下，只允许HTTP协议 
        { 
            fprintf(stderr,"\nOnly HTTP protocol is directly supported, set --proxy for others.\n");
            exit(2);
        }
    /* protocol/host delimiter */
    // 指向"://"后的第一个字母
    i=strstr(url,"://")-url+3;
    /* printf("%d\n",i); */
    // URL后必须得'/'
    if(strchr(url+i,'/')==NULL) 
    {
        fprintf(stderr,"\nInvalid URL syntax - hostname don't ends with '/'.\n");
        exit(2);            
    }
    // 如果未使用代理服务器，就表示肯定是HTTP协议
    if(proxyhost==NULL)
    {
        /* get port from hostname */
        // 如果是server : port 形式，解析主机和端口
        if(index(url+i,':')!=NULL &&
                index(url+i,':')<index(url+i,'/'))
        {
            // 获取主机地址
            strncpy(host,url+i,strchr(url+i,':')-url-i);
            bzero(tmp,10);
            strncpy(tmp,index(url+i,':')+1,strchr(url+i,'/')-index(url+i,':')-1);
            /* printf("tmp=%s\n",tmp); */
            // 目标端口
            proxyport=atoi(tmp);
            if(proxyport==0) proxyport=80;
        }     
        else
        {
            strncpy(host,url+i,strcspn(url+i,"/"));
        }
        // printf("Host=%s\n",host);
        // url + i + strcspn(url+i,"/") 得到域名后面的目标地址
        strcat(request+strlen(request),url+i+strcspn(url+i,"/"));

    } 
    else
    {
        // printf("ProxyHost=%s\nProxyPort=%d\n",proxyhost,proxyport);
        // 如若使用代理服务器
        strcat(request,url);
    }
  
    if(http10==1)
        strcat(request," HTTP/1.0");
    else if (http10==2)
        strcat(request," HTTP/1.1");
    // 完成如 GET / HTTP1.1 后，添加"\r\n"
    strcat(request,"\r\n");
    if(http10>0)
        strcat(request,"User-Agent: WebBench "PROGRAM_VERSION"\r\n");
    if(proxyhost==NULL && http10>0)
    {
        strcat(request,"Host: ");
        strcat(request,host);
        strcat(request,"\r\n");
    }
    // force_reload = 1 和存在代理服务器，则不缓存 
    if(force_reload && proxyhost!=NULL)
    {
        strcat(request,"Pragma: no-cache\r\n");
    }
    // 如果为HTTP1.1，则存在长连接，应将Connection置为close
    if(http10>1)
        strcat(request,"Connection: close\r\n");
    /* add empty line at end */
    // 别忘记在请求后添加"\r\n"
    if(http10>0) strcat(request,"\r\n"); 
    // printf("Req=%s\n",request);



    printf("----test ----- the http request is ----   : \n");
    printf("%s  \n",request);
    printf("----end  ------\n");
    printf("----test ----- host ----   : \n");
    printf("%s   \n",host);
    printf("----end  ------\n");
}


//benchcore 里面，这里做了一个简单的模拟就是，就是讲子进程的结果speed,failed,bytes 写到管道里面
void benchcore(const char *host,const int port,const char *req){
    printf("benchcore \n");
    printf("pid is  %d  \n",getpid());

    speed=(int)getpid();
    failed=(int)getpid();
    bytes=(int)getpid();
}

//bench
static int bench(void)
{
    int i,j,k;	
    pid_t pid=0;
    FILE *f;

    /* check avaibility of target server */
    i=Socket(proxyhost==NULL?host:proxyhost,proxyport);
    if(i<0) 
    { 
        fprintf(stderr,"\nConnect to server failed. Aborting benchmark.\n");
        return 1;
    }
    close(i);

    /* create pipe */
    if(pipe(mypipe))
    {
        perror("pipe failed.");
	    return 3;
    }


    /* fork childs */
    // 根据并发数创建子进程
    for(i=0;i<clients;i++)
    {
        pid=fork();
        if(pid <= (pid_t) 0)
        {
            //pid <0 create child process error
            //pid =0 表示是子进程的返回值
            sleep(1); 
            break;
	    }
    }
    //pid <0 create child process error
    if( pid< (pid_t) 0)
    {
        fprintf(stderr,"problems forking worker no. %d\n",i);
        perror("fork failed.");
        return 3;
    }


    // 核心代码
    // 子进程则调用benchcore函数
    if(pid== (pid_t) 0)
    {
        printf("i am child process ,pid is %d \n",getpid());
	    
        /* I am a child */
        if(proxyhost==NULL)
            benchcore(host,proxyport,request);
        else
            benchcore(proxyhost,proxyport,request);

        /* write results to pipe */
	    f=fdopen(mypipe[1],"w");
	    if(f==NULL)
	    {
            perror("open pipe for writing failed.");
            return 3;
	    }
	    /* fprintf(stderr,"Child - %d %d\n",speed,failed); */
	    // 子进程将speed failed bytes写进管道
        
        printf("speed ,failed and bytes :  %d %d %d\n",speed,failed,bytes);
        //fprintf(f,"%d %d %d\n",speed,failed,bytes);
	    //fclose(f);
	    return 0;
    } 
    else  //父进程
    {
        printf("i am father process pid is %d \n",getpid());
        pid_t ret=wait(NULL);
        printf("catch child is %d \n",ret);
        ret=wait(NULL);
        printf("catch child is %d \n",ret);
    }
    return i;
}


int main(int argc,char * argv[]){
    int opt=0;
    int options_index=0;
    char *tmp=NULL;
    while((opt=getopt_long(argc,argv,"912Vfrt:p:c:?h",long_options,&options_index))!=EOF )
    {
        switch(opt)
        {
            case  0 : break;
            case 'f': force=1;break;
            case 'r': force_reload=1;break; 
            case '9': http10=0;break;
            case '1': http10=1;break;
            case '2': http10=2;break;
            case 'V': printf(PROGRAM_VERSION"\n");exit(0);
            case 't': benchtime=atoi(optarg); printf("benchtime is %d \n",benchtime); break;	     
            case 'p': 
                    /* proxy server parsing server:port */
                    tmp=strrchr(optarg,':');
                    proxyhost=optarg;
                    if(tmp==NULL)
                    {
                        break;
                    }
                    if(tmp==optarg)
                    {
                        fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
                        return 2;
                    }
                    if(tmp==optarg+strlen(optarg)-1)
                    {
                        fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
                        return 2;
                    }
                    *tmp='\0';
                    proxyport=atoi(tmp+1);break;
            case ':':
            case 'h':
            case '?': usage();return 2;break;
            case 'c': clients=atoi(optarg);  printf("clients is : %d \n",clients); break;
        }
    }
    usage();

    printf("clients is : %d \n",clients);
    printf("benchtime is %d \n",benchtime);
    printf("argc is %d \n",argc);
    printf("optind is %d \n",optind);
    printf("argv is %s \n",argv[optind]);
    printf("MAXHOSTNAMELEN is %d \n",MAXHOSTNAMELEN);

    build_request(argv[optind]);

    bench();

    return 0;
}