
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_list_part_s  ngx_list_part_t;

//链表每个节点的结构
struct ngx_list_part_s {
    void             *elts; //指向该节点的数据区(该数据区中可存放nalloc个大小为size的元素)
    ngx_uint_t        nelts; //已存放的元素个数
    ngx_list_part_t  *next; //指向下一个链表节点
};

// list结构
typedef struct {
    ngx_list_part_t  *last;  //最后一个元素
    ngx_list_part_t   part;  //链表头中包含的第一个节点(part)
    size_t            size;  //单个元素大小，这个元素指的是part里面的elts的元素大小
    ngx_uint_t        nalloc;//容量。表示在不引发扩容的前提下，可以最多存储的元素的个数
    ngx_pool_t       *pool;  //使用的资源池
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);
/*
初始化list结构，n个size大小的内存元素组成的list
*/
static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
