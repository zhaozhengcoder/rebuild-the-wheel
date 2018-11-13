
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_
#define _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct ngx_http_upstream_rr_peer_s   ngx_http_upstream_rr_peer_t;

struct ngx_http_upstream_rr_peer_s {
    struct sockaddr                *sockaddr;                   /*后端服务器地址*/
    socklen_t                       socklen;                    /*地址长度*/
    ngx_str_t                       name;                       /*后端服务器地址的字符串*/
    ngx_str_t                       server;                     /*server的名称*/

    ngx_int_t                       current_weight;             /*当前权重,动态调整,初始为0*/
    ngx_int_t                       effective_weight;           /*有效权重*/
    ngx_int_t                       weight;                     /*配置项指定的权重,固定*/

    ngx_uint_t                      conns;                      /*当前连接数*/
    ngx_uint_t                      max_conns;                  /*最大连接数*/

    ngx_uint_t                      fails;                      /*失败次数(一段时间内)*/
    time_t                          accessed;                   /*最近一次失败的时间点*/
    time_t                          checked;                    /*用于检测是否超过了"一段时间"*/

    ngx_uint_t                      max_fails;                  /*最大失败次数*/
    time_t                          fail_timeout;               /*"一段时间"的值,固定*/
    ngx_msec_t                      slow_start;
    ngx_msec_t                      start_time;

    ngx_uint_t                      down;                       /*服务器永久不可用标记*/

#if (NGX_HTTP_SSL || NGX_COMPAT)
    void                           *ssl_session;
    int                             ssl_session_len;
#endif

#if (NGX_HTTP_UPSTREAM_ZONE)
    ngx_atomic_t                    lock;
#endif

    ngx_http_upstream_rr_peer_t    *next;                       /*指向下一个后端,用于构成链表*/

    NGX_COMPAT_BEGIN(32)
    NGX_COMPAT_END
};


typedef struct ngx_http_upstream_rr_peers_s  ngx_http_upstream_rr_peers_t;
/*
 * 后端集群
 */
struct ngx_http_upstream_rr_peers_s {
    ngx_uint_t                      number;                     /*后端服务器数量*/

#if (NGX_HTTP_UPSTREAM_ZONE)
    ngx_slab_pool_t                *shpool;
    ngx_atomic_t                    rwlock;
    ngx_http_upstream_rr_peers_t   *zone_next;
#endif

    ngx_uint_t                      total_weight;               /*权重累加值*/

    unsigned                        single:1;                   /*是否只有一台*/
    unsigned                        weighted:1;                 /*是否使用权重*/

    ngx_str_t                      *name;                       /*upstream配置块名称*/

    ngx_http_upstream_rr_peers_t   *next;                       /*备用集群*/

    ngx_http_upstream_rr_peer_t    *peer;                       /*后端服务器组成的链表*/
};


#if (NGX_HTTP_UPSTREAM_ZONE)

#define ngx_http_upstream_rr_peers_rlock(peers)                               \
                                                                              \
    if (peers->shpool) {                                                      \
        ngx_rwlock_rlock(&peers->rwlock);                                     \
    }

#define ngx_http_upstream_rr_peers_wlock(peers)                               \
                                                                              \
    if (peers->shpool) {                                                      \
        ngx_rwlock_wlock(&peers->rwlock);                                     \
    }

#define ngx_http_upstream_rr_peers_unlock(peers)                              \
                                                                              \
    if (peers->shpool) {                                                      \
        ngx_rwlock_unlock(&peers->rwlock);                                    \
    }


#define ngx_http_upstream_rr_peer_lock(peers, peer)                           \
                                                                              \
    if (peers->shpool) {                                                      \
        ngx_rwlock_wlock(&peer->lock);                                        \
    }

#define ngx_http_upstream_rr_peer_unlock(peers, peer)                         \
                                                                              \
    if (peers->shpool) {                                                      \
        ngx_rwlock_unlock(&peer->lock);                                       \
    }

#else

#define ngx_http_upstream_rr_peers_rlock(peers)
#define ngx_http_upstream_rr_peers_wlock(peers)
#define ngx_http_upstream_rr_peers_unlock(peers)
#define ngx_http_upstream_rr_peer_lock(peers, peer)
#define ngx_http_upstream_rr_peer_unlock(peers, peer)

#endif


typedef struct {
    ngx_uint_t                      config;
    ngx_http_upstream_rr_peers_t   *peers;                      /* 后端集群 */
    ngx_http_upstream_rr_peer_t    *current;                    /* 当前使用的后端服务器 */
    uintptr_t                      *tried;                      /* 指向后端服务器的位图 */
    uintptr_t                       data;                       /* 当后端服务器的数量较少时，用于存放其位图 */
} ngx_http_upstream_rr_peer_data_t;


ngx_int_t ngx_http_upstream_init_round_robin(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_init_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_create_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_resolved_t *ur);
ngx_int_t ngx_http_upstream_get_round_robin_peer(ngx_peer_connection_t *pc,
    void *data);
void ngx_http_upstream_free_round_robin_peer(ngx_peer_connection_t *pc,
    void *data, ngx_uint_t state);

#if (NGX_HTTP_SSL)
ngx_int_t
    ngx_http_upstream_set_round_robin_peer_session(ngx_peer_connection_t *pc,
    void *data);
void ngx_http_upstream_save_round_robin_peer_session(ngx_peer_connection_t *pc,
    void *data);
#endif


#endif /* _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_ */
