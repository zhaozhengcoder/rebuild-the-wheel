
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


ngx_uint_t  ngx_pagesize;
ngx_uint_t  ngx_pagesize_shift;
ngx_uint_t  ngx_cacheline_size;


void *
ngx_alloc(size_t size, ngx_log_t *log)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno,
                      "malloc(%uz) failed", size);
    }

    ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, log, 0, "malloc: %p:%uz", p, size);

    return p;
}


void *
ngx_calloc(size_t size, ngx_log_t *log)
{
    void  *p;

    p = ngx_alloc(size, log);

    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}


#if (NGX_HAVE_POSIX_MEMALIGN)
/*
 *  Linux系统下调用posix_memalign进行内存分配和内存对齐
 *
 */
void *
ngx_memalign(size_t alignment, size_t size, ngx_log_t *log)
{
    void  *p;
    int    err;
    /*
     * http://hahaya.github.io/memory-alloc-in-nginx/
     * 背景：
     *      1）POSIX 1003.1d
     *      2）POSIX 标明了通过malloc( ), calloc( ), 和 realloc( ) 返回的地址对于任何的C类型来说都是对齐的
     * 功能：由posix_memalign分配的内存空间，需要由free释放。
     * 参数：
     *      p           分配好的内存空间的首地址
     *      alignment   对齐边界，Linux中，32位系统是8字节，64位系统是16字节
     *      size        指定分配size字节大小的内存
     * 要求：
     *      1）要求alignment是2的幂，并且是p指针大小的倍数
     *      2）要求size是alignment的倍数
     * 返回：
     *      0       成功
     *      EINVAL  参数不满足要求
     *      ENOMEM  内存分配失败
     * 注意：
     *      1）该函数不影响errno，只能通过返回值判
     *      2）在32位系统下malloc、calloc分配的内存是8字节为边界对齐的 即返回的内存地址起始值是8的倍数
     *      3）在64位系统下malloc、calloc分配的内存时16字节为边界对齐的 即返回的内存地址起始值是16的倍数
     *      4）有时候，对齐更大的边界，例如页面等，程序员需要动态的对齐，于是出现了posix_memalign函数
     *      5）posix_memalign函数分配内存返回的内存地址起始值是alignment的倍数
     */
    err = posix_memalign(&p, alignment, size);

    if (err) {
        ngx_log_error(NGX_LOG_EMERG, log, err,
                      "posix_memalign(%uz, %uz) failed", alignment, size);
        p = NULL;
    }

    ngx_log_debug3(NGX_LOG_DEBUG_ALLOC, log, 0,
                   "posix_memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

#elif (NGX_HAVE_MEMALIGN)
/*
 * Solaris系统下调用memalign进行内存分配和内存对齐
 *
 */
void *
ngx_memalign(size_t alignment, size_t size, ngx_log_t *log)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno,
                      "memalign(%uz, %uz) failed", alignment, size);
    }

    ngx_log_debug3(NGX_LOG_DEBUG_ALLOC, log, 0,
                   "memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

#endif
