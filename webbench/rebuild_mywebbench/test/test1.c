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


/*这个第一个例子　看懂了usage*/
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

    
    return 0;
}