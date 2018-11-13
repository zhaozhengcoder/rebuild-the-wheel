
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/*从内存池中分配一块size大小的缓冲区，内部成员初始化好，temporary设为1*/
ngx_buf_t *
ngx_create_temp_buf(ngx_pool_t *pool, size_t size)
{
    ngx_buf_t *b;

    /* 最终调用的是内存池pool，开辟一段内存用作缓冲区，主要放置ngx_buf_t结构体 */
    b = ngx_calloc_buf(pool);
    if (b == NULL) {
        return NULL;
    }

    /* 分配缓冲区内存;  pool为内存池，size为buf的大小*/
    b->start = ngx_palloc(pool, size);
    if (b->start == NULL) {
        return NULL;
    }

    /*
     * set by ngx_calloc_buf():
     *
     *     b->file_pos = 0;
     *     b->file_last = 0;
     *     b->file = NULL;
     *     b->shadow = NULL;
     *     b->tag = 0;
     *     and flags
     */

    b->pos = b->start;
    b->last = b->start;
    b->end = b->last + size;
    b->temporary = 1;

    return b;
}

//从内存池中获取ngx_chian_t对象
ngx_chain_t *
ngx_alloc_chain_link(ngx_pool_t *pool)
{
    ngx_chain_t  *cl;
    /*
     * 首先从内存池中去取ngx_chain_t，
     * 被清空的ngx_chain_t结构都会放在pool->chain 缓冲链上
     */
    cl = pool->chain;

    if (cl) {
        pool->chain = cl->next;
        return cl;
    }
    /* 如果取不到，则从内存池pool上分配一个数据结构  */
    cl = ngx_palloc(pool, sizeof(ngx_chain_t));
    if (cl == NULL) {
        return NULL;
    }

    return cl;
}

//一次创建多个缓冲区，返回一个连接好的链表
ngx_chain_t *
ngx_create_chain_of_bufs(ngx_pool_t *pool, ngx_bufs_t *bufs)
{
    u_char       *p;
    ngx_int_t     i;
    ngx_buf_t    *b;
    ngx_chain_t  *chain, *cl, **ll; //ll代表指针的指针

    p = ngx_palloc(pool, bufs->num * bufs->size);
    if (p == NULL) {
        return NULL;
    }

    //用ll来交换指针变量地址
    ll = &chain; //chain的地址赋值给ll，使用*ll作赋值操作时会改变chain的指针

    for (i = 0; i < bufs->num; i++) {

        b = ngx_calloc_buf(pool);
        if (b == NULL) {
            return NULL;
        }

        /*
         * set by ngx_calloc_buf():
         *
         *     b->file_pos = 0;
         *     b->file_last = 0;
         *     b->file = NULL;
         *     b->shadow = NULL;
         *     b->tag = 0;
         *     and flags
         *
         */

        b->pos = p;
        b->last = p;
        b->temporary = 1;

        b->start = p;
        p += bufs->size;
        b->end = p;

        //分配一个nginx_chain_t
        cl = ngx_alloc_chain_link(pool);
        if (cl == NULL) {
            return NULL;
        }

        /* 将buf，都挂载到ngx_chain_t链表上，最终返回ngx_chain_t链表 */
        cl->buf = b;
        *ll = cl;   //ll相同地址的对象，chain等于cl
        ll = &cl->next; //cl->next的地址赋值给ll，使用*ll作赋值操作时会改变cl->next的指针
    }

    *ll = NULL;

    return chain;
}

/**
 * 将其它缓冲区链表放到已有缓冲区链表结构的尾部
 */
ngx_int_t
ngx_chain_add_copy(ngx_pool_t *pool, ngx_chain_t **chain, ngx_chain_t *in)
{
    ngx_chain_t  *cl, **ll;

    ll = chain;

    /* 找到缓冲区链表结尾部分，cl->next== NULL；cl = *chain既为指针链表地址*/
    for (cl = *chain; cl; cl = cl->next) {
        ll = &cl->next; //设置ll的指向对象
    }

    //依次将in链表中的节点插入到chain链表末尾
    while (in) {
        cl = ngx_alloc_chain_link(pool);
        if (cl == NULL) {
            return NGX_ERROR;
        }

        cl->buf = in->buf;
        *ll = cl;   //*ll被赋值操作，会改变chain链表尾部指针
        ll = &cl->next;
        in = in->next;
    }

    *ll = NULL;

    return NGX_OK;
}

/*从空闲的buf链表上，获取一个未使用的buf链表*/
ngx_chain_t *
ngx_chain_get_free_buf(ngx_pool_t *p, ngx_chain_t **free)
{
    ngx_chain_t  *cl;

    /*空闲列表有数据，则直接返回一个链表*/
    if (*free) {
        cl = *free;
        *free = cl->next;
        cl->next = NULL;
        return cl;
    }

    /*分配一个新的buf*/
    cl = ngx_alloc_chain_link(p);
    if (cl == NULL) {
        return NULL;
    }

    cl->buf = ngx_calloc_buf(p);
    if (cl->buf == NULL) {
        return NULL;
    }

    cl->next = NULL;

    return cl;
}

/**
 * 将busy链表中的空闲节点回收到free链表中
 *
 * @param p
 * @param free 空闲链表
 * @param busy busy链表
 * @param out 已输出链表
 * @param tag 需要释放buf的标记
 */
void
ngx_chain_update_chains(ngx_pool_t *p, ngx_chain_t **free, ngx_chain_t **busy,
    ngx_chain_t **out, ngx_buf_tag_t tag)
{
    ngx_chain_t  *cl;

    /*将已输出out链表放到busy链表上*/
    if (*out) {
        if (*busy == NULL) {
            *busy = *out;   //busy直接指向out链表

        } else {
            //找到busy链表的最后一个节点
            for (cl = *busy; cl->next; cl = cl->next) { /* void */ }

            cl->next = *out;
        }

        *out = NULL;
    }

    while (*busy) {
        cl = *busy;
        //合并后的该busy链表节点有内容时，则表示剩余节点都有内容，则退出
        if (ngx_buf_size(cl->buf) != 0) {
            break;
        }

        //如果该busy链表节点不属于tag指向的模块，则跳过。
        if (cl->buf->tag != tag) {
            *busy = cl->next;
            ngx_free_chain(p, cl);
            continue;
        }

        //重置buf缓冲区所有空间都可用
        cl->buf->pos = cl->buf->start;
        cl->buf->last = cl->buf->start;

        //将该空闲空闲区加入到free链表表头
        *busy = cl->next;
        cl->next = *free;
        *free = cl;
    }
}


off_t
ngx_chain_coalesce_file(ngx_chain_t **in, off_t limit)
{
    off_t         total, size, aligned, fprev;
    ngx_fd_t      fd;
    ngx_chain_t  *cl;

    total = 0;

    cl = *in;
    fd = cl->buf->file->fd;

    do {
        size = cl->buf->file_last - cl->buf->file_pos;

        if (size > limit - total) {
            size = limit - total;

            aligned = (cl->buf->file_pos + size + ngx_pagesize - 1)
                       & ~((off_t) ngx_pagesize - 1);

            if (aligned <= cl->buf->file_last) {
                size = aligned - cl->buf->file_pos;
            }

            total += size;
            break;
        }

        total += size;
        fprev = cl->buf->file_pos + size;
        cl = cl->next;

    } while (cl
             && cl->buf->in_file
             && total < limit
             && fd == cl->buf->file->fd
             && fprev == cl->buf->file_pos);

    *in = cl;

    return total;
}


ngx_chain_t *
ngx_chain_update_sent(ngx_chain_t *in, off_t sent)
{
    off_t  size;

    /* 开始重新遍历chain，这里是为了防止没有发送完全的情况，此时我们就需要切割buf了 */
    for ( /* void */ ; in; in = in->next) {

        if (ngx_buf_special(in->buf)) {
            continue;
        }

        if (sent == 0) {
            break;
        }

        /* 得到buf size */
        size = ngx_buf_size(in->buf);
        /* 如果大于当前的size，则说明这个buf的数据已经被完全发送完毕了，因此更新它的域 */
        if (sent >= size) {
            sent -= size;
            /* 如果在内存则更新pos */
            if (ngx_buf_in_memory(in->buf)) {
                in->buf->pos = in->buf->last;
            }
            /* 如果在file中则更显file_pos */
            if (in->buf->in_file) {
                in->buf->file_pos = in->buf->file_last;
            }

            continue;
        }
        /* 到这里说明当前的buf只有一部分被发送出去了，因此只需要修改指针。以便于下次发送 */
        if (ngx_buf_in_memory(in->buf)) {
            in->buf->pos += (size_t) sent;
        }

        if (in->buf->in_file) {
            in->buf->file_pos += sent;
        }

        break;
    }

    return in;
}
