# 如何创建测试程序调试nginx数据结构

由于在学习nginx的过程中遇到很多数据结构，往往我都想写一个程序来跑一下，看下到底返回什么。最开始想的方法是使用nginx make 完成之后的.o文件，做LINK的方式来做。这个路子尝试下去之后发现还是很麻烦，也没有尝试成功，需要对objs/Makefile做很多调整。

后来看到了这么一篇才想到可以直接写一个程序，只需要把一些头文件和c文件给引入正确应该就可以了。https://www.kancloud.cn/digest/understandingnginx/202590。

果然成功了，这里记录一下。

# 创建test文件

我现在当前目录在nginx的目录

创建
```
src/test/demo.c
```
这里的代码如下：
```
#include "ngx_config.h"
#include <stdio.h>
#include "ngx_conf_file.h"
#include "nginx.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"
#include "ngx_array.h"

volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}

void dump_array(ngx_array_t* a)
{
    if (a)
    {
        printf("array = 0x%x\n", a);
        printf("  .elts = 0x%x\n", a->elts);
        printf("  .nelts = %d\n", a->nelts);
        printf("  .size = %d\n", a->size);
        printf("  .nalloc = %d\n", a->nalloc);
        printf("  .pool = 0x%x\n", a->pool);

        printf("elements: ");
        int *ptr = (int*)(a->elts);
        for (; ptr < (int*)(a->elts + a->nalloc * a->size); )
        {
            printf("%d  ", *ptr++);
        }
        printf("\n");
    }
}

int main()
{
    ngx_pool_t *pool;
    int i;

    printf("--------------------------------\n");
    printf("create a new pool:\n");
    printf("--------------------------------\n");
    pool = ngx_create_pool(1024, NULL);

    printf("--------------------------------\n");
    printf("alloc an array from the pool:\n");
    printf("--------------------------------\n");
    ngx_array_t *a = ngx_array_create(pool, 5, sizeof(int));

    for (i = 0; i < 5; i++)
    {
        int *ptr = ngx_array_push(a);
        *ptr = 2*i;
    }

    dump_array(a);

    ngx_array_destroy(a);
    ngx_destroy_pool(pool);
    return 0;
}
```

这里代码有几个地方需要特别注意，否则一定编译不过：

1
```
#include "ngx_config.h"
#include <stdio.h>
#include "ngx_conf_file.h"
#include "nginx.h"
#include "ngx_core.h"
```
这几个头文件一定要引入，包含了大部分nginx需要的信息

```
#include "ngx_string.h"
#include "ngx_palloc.h"
#include "ngx_array.h"
```

这些根据需要引入。

```
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}

```
这两个代码是需要的，特别是ngx_log_error_core，否则真正的这个函数会需要你引入很多文件。
这里使用的是一个空实现。

一定不要引入 ngx_log.c 这个文件。否则会有冲突。


# 编译

编译命令是这样：
```
gcc src/test/demo.c src/core/ngx_array.c src/os/unix/ngx_alloc.c  src/core/ngx_list.c  src/core/ngx_palloc.c  -I src/os/unix/ -I src/core/ -I objs/
```

这是使用静态编译的方式，所有需要编译的文件都会打包到这个输出中。后面的-I是引入了几个文件夹，这三个文件夹也是必要的。

gcc后面的c文件使用到了哪些就需要引用哪些


然后就可以编译成功了。
