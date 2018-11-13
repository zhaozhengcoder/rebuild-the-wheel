# nginx-1.14.0版本的源码分析

# docs文件夹

docs文件夹存放我们的源码分析文章, 里面的ppt和xmind或者md都是its-tech小组成员的研究总结

# src

以注释的方式进行分析说明


例如：
```
#!/bin/sh

# Copyright (C) Igor Sysoev
# Copyright (C) Nginx, Inc.

# 处理configure命令的参数。
. auto/options
# 初始化后续将产生的文件路径。例如，Makefile,ngx_modules.c等文件默认情况下会在<nginx-source>/objs/
. auto/init
# 分析Nginx的源码结构，这样才能构造后续的Makefile文件
. auto/sources

# 编译过程中所有目录文件生成的路径由--builddir=DIR参数指定，默认情况下为<nginx-source>/objs,此时这个目录将会被创建
test -d $NGX_OBJS || mkdir $NGX_OBJS

```


```
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_event_connect.h>

/*
  pc当中保存着与上游连接的套接字参数
  1. 打开一个套接字
  2. 设置该套接字为非阻塞
*/
ngx_int_t
ngx_event_connect_peer(ngx_peer_connection_t *pc)
{
    int                rc;
    ngx_int_t          event;
    ngx_err_t          err;
    ngx_uint_t         level;
    ngx_socket_t       s;
    ngx_event_t       *rev, *wev;
    ngx_connection_t  *c;

    rc = pc->get(pc, pc->data);
```
