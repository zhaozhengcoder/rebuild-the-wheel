
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;
    void                 *data;
    ngx_pool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

//大内存结构
struct ngx_pool_large_s {
    ngx_pool_large_t     *next;     //下一个大块内存
    void                 *alloc;    //nginx分配的大块内存空间
};


typedef struct {
    u_char               *last;     //当前内存分配结束位置，即下一段可分配内存的起始位置
    u_char               *end;      //内存池结束位置
    ngx_pool_t           *next;     //链接到下一个内存池
    ngx_uint_t            failed;   //统计该内存池不能满足分配请求的次数
} ngx_pool_data_t;


/*
 * 该结构维护整个内存池的头部信息
 * Nginx使用内存池时总是只申请,不释放,使用完毕后直接destroy整个内存池
*/
struct ngx_pool_s {
    ngx_pool_data_t       d;        //数据块
    size_t                max;      //数据块大小，即小块内存的最大值
    ngx_pool_t           *current;  //保存当前内存值
    ngx_chain_t          *chain;    //可以挂一个chain结构
    ngx_pool_large_t     *large;    //分配大块内存用，即超过max的内存请求
    ngx_pool_cleanup_t   *cleanup;  //挂载一些内存池释放的时候，同时释放的资源
    ngx_log_t            *log;
};

/*ngx_pool_cleanup_t中的*data成员通常指向ngx_pool_cleanup_file_t结构体*/
typedef struct {
    ngx_fd_t              fd;
    u_char               *name;
    ngx_log_t            *log;
} ngx_pool_cleanup_file_t;

//通过ngx_create_pool可以创建一个内存池
ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);    //销毁内存池
void ngx_reset_pool(ngx_pool_t *pool);      //释放所有large内存，重置复用普通内存

void *ngx_palloc(ngx_pool_t *pool, size_t size);    //通过ngx_palloc可以从内存池中分配指定大小的内存. palloc取得的内存是对齐的
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);   //pnalloc取得的内存是不对齐的
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);   //pcalloc直接调用palloc分配好内存，然后进行一次0初始化操作
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);   //在分配size大小的内存，并按照alignment对齐，然后挂到large字段下
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);     //释放large内存


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);   //注册cleanup
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd); //清除 p->cleanup链表上的内存块（主要是文件描述符）
void ngx_pool_cleanup_file(void *data); //关闭文件回调函数
void ngx_pool_delete_file(void *data);  //删除文件回调函数


#endif /* _NGX_PALLOC_H_INCLUDED_ */
