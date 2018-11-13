
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CONNECTION_H_INCLUDED_
#define _NGX_CONNECTION_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_listening_s  ngx_listening_t;

struct ngx_listening_s {
    ngx_socket_t        fd;  // 套接字句柄

    struct sockaddr    *sockaddr;  // 监听地址
    socklen_t           socklen;    // 监听地址长度
    size_t              addr_text_max_len; // 存储IP地址的字符串addr_text的最大长度
    ngx_str_t           addr_text; //ip地址

    int                 type; // 套接字类型。types是SOCK_STREAM时，表示是tcp

    int                 backlog; // TCP实现监听时的backlog队列，它表示允许正在通过三次握手建立tcp连接但还没有任何进程开始处理的连接最大个数
    int                 rcvbuf; // 接收缓冲区大小
    int                 sndbuf; // 发送缓冲区大小
#if (NGX_HAVE_KEEPALIVE_TUNABLE)
    int                 keepidle;
    int                 keepintvl;
    int                 keepcnt;
#endif

    /* handler of accepted connection */
    ngx_connection_handler_pt   handler; // 新的tcp连接建立成功之后回调方法

    void               *servers;  //目前主要用于HTTP或者mail等模块，用于保存当前监听端口对应着的所有主机名，就是配置文件中的server

    ngx_log_t           log; // 日志
    ngx_log_t          *logp; // 日志指针

    size_t              pool_size; // 连接池的初始化大小
    /* should be here because of the AcceptEx() preread */
    size_t              post_accept_buffer_size;
    /* should be here because of the deferred accept */
    ngx_msec_t          post_accept_timeout; //～秒后仍然没有收到用户的数据，就丢弃该连接

    ngx_listening_t    *previous; //前一个ngx_listening_t结构，用于组成单链表
    ngx_connection_t   *connection; //当前监听句柄对应的ngx_connection_t结构体

    ngx_uint_t          worker;

    unsigned            open:1; //为1表示监听句柄有效，为0表示正常关闭
    unsigned            remain:1; //为1表示不关闭原先打开的监听端口，为0表示关闭曾经打开的监听端口
    unsigned            ignore:1; //为1表示跳过设置当前ngx_listening_t结构体中的套接字，为0时正常初始化套接字

    unsigned            bound:1;       /* already bound */
    unsigned            inherited:1;   /* inherited from previous process */
    unsigned            nonblocking_accept:1;
    unsigned            listen:1; //为1表示当前结构体对应的套接字已经监听
    unsigned            nonblocking:1;
    unsigned            shared:1;    /* shared between threads or processes */
    unsigned            addr_ntop:1; //为1表示将网络地址转变为字符串形式的地址
    unsigned            wildcard:1;

#if (NGX_HAVE_INET6)
    unsigned            ipv6only:1;
#endif
    unsigned            reuseport:1;
    unsigned            add_reuseport:1;
    unsigned            keepalive:2;

    unsigned            deferred_accept:1;
    unsigned            delete_deferred:1;
    unsigned            add_deferred:1;
#if (NGX_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
    char               *accept_filter;
#endif
#if (NGX_HAVE_SETFIB)
    int                 setfib;
#endif

#if (NGX_HAVE_TCP_FASTOPEN)
    int                 fastopen;
#endif

};


typedef enum {
    NGX_ERROR_ALERT = 0,
    NGX_ERROR_ERR,
    NGX_ERROR_INFO,
    NGX_ERROR_IGNORE_ECONNRESET,
    NGX_ERROR_IGNORE_EINVAL
} ngx_connection_log_error_e;


typedef enum {
    NGX_TCP_NODELAY_UNSET = 0,
    NGX_TCP_NODELAY_SET,
    NGX_TCP_NODELAY_DISABLED
} ngx_connection_tcp_nodelay_e;


typedef enum {
    NGX_TCP_NOPUSH_UNSET = 0,
    NGX_TCP_NOPUSH_SET,
    NGX_TCP_NOPUSH_DISABLED
} ngx_connection_tcp_nopush_e;


#define NGX_LOWLEVEL_BUFFERED  0x0f
#define NGX_SSL_BUFFERED       0x01
#define NGX_HTTP_V2_BUFFERED   0x02


struct ngx_connection_s {
    void               *data; //连接未使用时，data用于充当连接池中空闲链表中的next指针。连接使用时由模块而定，HTTP中，data指向ngx_http_request_t
    ngx_event_t        *read; //连接对应的读事件
    ngx_event_t        *write; //连接对应的写事件

    ngx_socket_t        fd; //套接字对应的句柄

    ngx_recv_pt         recv; //直接接收网络字符流的方法
    ngx_send_pt         send; //直接发送网络字符流的方法
    ngx_recv_chain_pt   recv_chain; //以链表来接收网络字符流的方法
    ngx_send_chain_pt   send_chain; //以链表来发送网络字符流的方法

    ngx_listening_t    *listening; //这个连接对应的ngx_listening_t监听对象，此连接由listening监听端口的事件建立

    off_t               sent; //这个连接上已发送的字节数

    ngx_log_t          *log; //日志对象

    ngx_pool_t         *pool; /*内存池。一般在accept一个新的连接时，会创建一个内存池，而在这个连接结束时会销毁内存池。内存池大小是由上面listening成员的pool_size决定的*/

    int                 type;

    struct sockaddr    *sockaddr; //连接客户端的sockaddr
    socklen_t           socklen; //sockaddr结构体的长度
    ngx_str_t           addr_text; //连接客户段字符串形式的IP地址

    ngx_str_t           proxy_protocol_addr;
    in_port_t           proxy_protocol_port;

#if (NGX_SSL || NGX_COMPAT)
    ngx_ssl_connection_t  *ssl;
#endif

    struct sockaddr    *local_sockaddr; //本机监听端口对应的sockaddr结构体，实际上就是listening监听对象的sockaddr成员
    socklen_t           local_socklen;

    ngx_buf_t          *buffer; //用户接受、缓存客户端发来的字符流，buffer是由连接内存池分配的，大小自由决定

    ngx_queue_t         queue; 	/*用来将当前连接以双向链表元素的形式添加到ngx_cycle_t核心结构体的reuseable_connection_queue双向链表中，表示可以重用的连接*/

    ngx_atomic_uint_t   number;	/*连接使用次数。ngx_connection_t结构体每次建立一条来自客户端的连接，或者主动向后端服务器发起连接时，number都会加1*/

    ngx_uint_t          requests; //处理的请求次数

    unsigned            buffered:8; //缓存中的业务类型。

    unsigned            log_error:3;	//本连接的日志级别，占用3位，取值范围为0～7，但实际只定义了5个值，由ngx_connection_log_error_e枚举表示。

    unsigned            timedout:1; //为1表示连接已经超时
    unsigned            error:1; //为1表示连接处理过程中出现错误
    unsigned            destroyed:1; //为1表示连接已经销毁

    unsigned            idle:1; //为1表示连接处于空闲状态，如keepalive两次请求中间的状态
    unsigned            reusable:1; //为1表示连接可重用，与上面的queue字段对应使用
    unsigned            close:1; //为1表示连接关闭
    unsigned            shared:1;

    unsigned            sendfile:1; //为1表示正在将文件中的数据发往连接的另一端
    unsigned            sndlowat:1;	/*为1表示只有连接套接字对应的发送缓冲区必须满足最低设置的大小阀值时，事件驱动模块才会分发该事件。这与ngx_handle_write_event方法中的lowat参数是对应的*/
    unsigned            tcp_nodelay:2;   /* ngx_connection_tcp_nodelay_e */
    unsigned            tcp_nopush:2;    /* ngx_connection_tcp_nopush_e */

    unsigned            need_last_buf:1;

#if (NGX_HAVE_AIO_SENDFILE || NGX_COMPAT)
    unsigned            busy_count:2;
#endif

#if (NGX_THREADS || NGX_COMPAT)
    ngx_thread_task_t  *sendfile_task;
#endif
};


#define ngx_set_connection_log(c, l)                                         \
                                                                             \
    c->log->file = l->file;                                                  \
    c->log->next = l->next;                                                  \
    c->log->writer = l->writer;                                              \
    c->log->wdata = l->wdata;                                                \
    if (!(c->log->log_level & NGX_LOG_DEBUG_CONNECTION)) {                   \
        c->log->log_level = l->log_level;                                    \
    }


ngx_listening_t *ngx_create_listening(ngx_conf_t *cf, struct sockaddr *sockaddr,
    socklen_t socklen);
ngx_int_t ngx_clone_listening(ngx_conf_t *cf, ngx_listening_t *ls);
ngx_int_t ngx_set_inherited_sockets(ngx_cycle_t *cycle);
ngx_int_t ngx_open_listening_sockets(ngx_cycle_t *cycle);
void ngx_configure_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_listening_sockets(ngx_cycle_t *cycle);
void ngx_close_connection(ngx_connection_t *c);
void ngx_close_idle_connections(ngx_cycle_t *cycle);
ngx_int_t ngx_connection_local_sockaddr(ngx_connection_t *c, ngx_str_t *s,
    ngx_uint_t port);
ngx_int_t ngx_tcp_nodelay(ngx_connection_t *c);
ngx_int_t ngx_connection_error(ngx_connection_t *c, ngx_err_t err, char *text);

ngx_connection_t *ngx_get_connection(ngx_socket_t s, ngx_log_t *log);
void ngx_free_connection(ngx_connection_t *c);

void ngx_reusable_connection(ngx_connection_t *c, ngx_uint_t reusable);

#endif /* _NGX_CONNECTION_H_INCLUDED_ */
