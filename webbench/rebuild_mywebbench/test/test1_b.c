#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void usage(void)
{
   printf(
    "\n  unknown option \n"
	"  Eg : webbench -t 5 -c 5 http://localhost/  \n"
	"  -t|--time <sec>          Run benchmark for <sec> seconds. Default 30.\n"
	"  -c|--clients <n>         Run <n> HTTP clients at once. Default one.\n"
	);
};

int main(int argc, char **argv) {
    int ch;
    int var_a=0;
    int var_b=0;
    while((ch = getopt(argc, argv, "t:c:")) != -1) {
        switch(ch) {
            case 't':
                //printf("time : %s\n", optarg);
                var_a=atoi(optarg);
                break;
            case 'c':
                //printf("client :  %s\n", optarg);
                var_b=atoi(optarg);
                break;
            default:
                usage();
                return -1;
        }
    }
    if(var_a==0 | var_b==0){
        usage();
        return -1;
    }
    printf("time : %d  \n",var_a);
    printf("client : %d \n",var_b);
}