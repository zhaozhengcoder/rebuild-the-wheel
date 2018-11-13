
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

/* hash散列表中元素的结构，采用键值及其所以应的值<key，value>*/
typedef struct {
    void             *value; /* 指向用户自定义的数据 */
    u_short           len;  /* 键值key的长度 */
    u_char            name[1]; /* 键值key的第一个字符，数组名name表示指向键值key首地址 */
} ngx_hash_elt_t;

/* 基本hash散列表结构 */
typedef struct {
    ngx_hash_elt_t  **buckets; /* 指向hash散列表第一个存储元素的桶 */
    ngx_uint_t        size; /* hash散列表的桶个数 */
} ngx_hash_t;


typedef struct {
    ngx_hash_t        hash;
    void             *value;
} ngx_hash_wildcard_t;

/* 计算待添加元素的hash元素结构 */
typedef struct {
    ngx_str_t         key; /* 元素关键字 */
    ngx_uint_t        key_hash; /* 元素关键字key计算出的hash值 */
    void             *value; /* 指向关键字key对应的值，组成hash表元素：键-值<key，value> */
} ngx_hash_key_t;


typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;
    ngx_hash_wildcard_t  *wc_head;
    ngx_hash_wildcard_t  *wc_tail;
} ngx_hash_combined_t;

/* 初始化hash结构 */
typedef struct {
    ngx_hash_t       *hash; /* 指向待初始化的基本hash结构 */
    ngx_hash_key_pt   key; /* hash 函数指针 */

    ngx_uint_t        max_size; /* hash表中桶bucket的最大个数 */
    ngx_uint_t        bucket_size; /* 每个桶bucket的存储空间 */

    char             *name; /* hash结构的名称(仅在错误日志中使用) */
    ngx_pool_t       *pool; /* 分配hash结构的内存池 */
    ngx_pool_t       *temp_pool; /* 分配临时数据空间的内存池，仅在初始化hash表前，用于分配一些临时数组 */
} ngx_hash_init_t;


#define NGX_HASH_SMALL            1
#define NGX_HASH_LARGE            2

#define NGX_HASH_LARGE_ASIZE      16384
#define NGX_HASH_LARGE_HSIZE      10007

#define NGX_HASH_WILDCARD_KEY     1
#define NGX_HASH_READONLY_KEY     2


typedef struct {
    ngx_uint_t        hsize;

    ngx_pool_t       *pool;
    ngx_pool_t       *temp_pool;

    ngx_array_t       keys;
    ngx_array_t      *keys_hash;

    ngx_array_t       dns_wc_head;
    ngx_array_t      *dns_wc_head_hash;

    ngx_array_t       dns_wc_tail;
    ngx_array_t      *dns_wc_tail_hash;
} ngx_hash_keys_arrays_t;

/*
ngx_table_elt_t是为HTTP头部量身定制
*/
typedef struct {
    ngx_uint_t        hash; //对应的散列表结构中的位置
    ngx_str_t         key; // 头部名称
    ngx_str_t         value; // 头部值
    u_char           *lowcase_key; // 小写的头部名称key
} ngx_table_elt_t;


void *ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len);
void *ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,
    u_char *name, size_t len);

ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,
    ngx_uint_t nelts);

#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */
