
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_PIPE_H_INCLUDED_
#define _NGX_EVENT_PIPE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


typedef struct ngx_event_pipe_s  ngx_event_pipe_t;

typedef ngx_int_t (*ngx_event_pipe_input_filter_pt)(ngx_event_pipe_t *p,
                                                    ngx_buf_t *buf);
typedef ngx_int_t (*ngx_event_pipe_output_filter_pt)(void *data,
                                                     ngx_chain_t *chain);

//nginx处理buffering机制的结构
struct ngx_event_pipe_s {
    ngx_connection_t  *upstream;    //表示nginx和client，以及和后端的两条连接
    ngx_connection_t  *downstream;  //这个表示客户端的连接

    ngx_chain_t       *free_raw_bufs;   //保存了从upstream读取的数据(没有经过任何处理的)，以及缓存的buf.
    ngx_chain_t       *in;              //每次读取数据后，调用input_filter对协议格式进行解析，解析完后的数据部分放到in里面形成一个链表。

    //关于p->in和shadow，多说一下，in指向一堆chain链表每个链表指向一块实实在在的fcgi DATA数据，多个这样的html代码块共享一块大的裸FCGI数据块；
    //属于某个大的裸FCGI数据块的最后一个数据节点的last_shadow成员为1，表示我是这个大FCGI数据块的最后一个，并且我的shadow指针指向这个裸FCGI数据块的buf指针
    //释放这些大数据块的时候，可以参考ngx_event_pipe_drain_chains进行释放。
    ngx_chain_t      **last_in;         //上面的in结构的最后一个节点的next指针的地址，p->last_in = &cl->next;，这样就可以将新分析到的FCGI数据链接到后面了。

    ngx_chain_t       *writing;

    //buf到tempfile的数据会放到out里面。在ngx_event_pipe_write_chain_to_temp_file函数里面设置的。
    ngx_chain_t       *out;
    ngx_chain_t       *free;            //这里就是那些空闲的内存节点，从busy移动过来的。注意是节点，不是buf
    ngx_chain_t       *busy;            //这里就是那些空闲的内存节点，从busy移动过来的。注意是节点，不是buf

    /*
     * the input filter i.e. that moves HTTP/1.1 chunks
     * from the raw bufs to an incoming chain
     */
    //FCGI为ngx_http_fastcgi_input_filter，其他为ngx_event_pipe_copy_input_filter 。用来解析特定格式数据
    ngx_event_pipe_input_filter_pt    input_filter;     //这个用来解析对应协议的数据。比如解析FCGI协议的数据。
    void                             *input_ctx;

    ngx_event_pipe_output_filter_pt   output_filter;    //ngx_http_output_filter输出filter
    void                             *output_ctx;

#if (NGX_THREADS || NGX_COMPAT)
    ngx_int_t                       (*thread_handler)(ngx_thread_task_t *task,
                                                      ngx_file_t *file);
    void                             *thread_ctx;
    ngx_thread_task_t                *thread_task;
#endif

    unsigned           read:1;          //标记是否读了数据。
    unsigned           cacheable:1;
    unsigned           single_buf:1;    //如果使用了NGX_USE_AIO_EVENT异步IO标志，则设置为1
    unsigned           free_bufs:1;
    unsigned           upstream_done:1; //表示Nginx与上游交互已经结束unsigned
    unsigned           upstream_error:1;
    unsigned           upstream_eof:1;      //表示与上游服务器的连接已关闭unsigned
    unsigned           upstream_blocked:1;  //ngx_event_pipe用来标记是否读取了upstream的数据来决定是不是要write
    unsigned           downstream_done:1;   //与下游的交互已结束unsigned
    unsigned           downstream_error:1;
    unsigned           cyclic_temp_file:1;
    unsigned           aio:1;

    ngx_int_t          allocated;           //表示已经分配了的bufs的个数，每次会++
    ngx_bufs_t         bufs;                //fastcgi_buffers等指令设置的nginx用来缓存body的内存块数目以及大小。ngx_conf_set_bufs_slot函数会解析这样的配置。
    ngx_buf_tag_t      tag;

    ssize_t            busy_size;           //fastcgi_busy_buffers_size 指令或者其他upstream设置的大小，作用为最大的busy状态的内存总容量。

    off_t              read_length;         //从upstream读取的数据长度
    off_t              length;

    off_t              max_temp_file_size;
    ssize_t            temp_file_write_size;

    ngx_msec_t         read_timeout;
    ngx_msec_t         send_timeout;
    ssize_t            send_lowat;

    ngx_pool_t        *pool;
    ngx_log_t         *log;

    ngx_chain_t       *preread_bufs;        //指读取upstream的时候多读的，或者说预读的body部分数据。p->preread_bufs->buf = &u->buffer;
    size_t             preread_size;
    ngx_buf_t         *buf_to_file;

    size_t             limit_rate;
    time_t             start_sec;

    ngx_temp_file_t   *temp_file;

    /* STUB */ int     num;
};


ngx_int_t ngx_event_pipe(ngx_event_pipe_t *p, ngx_int_t do_write);
ngx_int_t ngx_event_pipe_copy_input_filter(ngx_event_pipe_t *p, ngx_buf_t *buf);
ngx_int_t ngx_event_pipe_add_free_buf(ngx_event_pipe_t *p, ngx_buf_t *b);


#endif /* _NGX_EVENT_PIPE_H_INCLUDED_ */
