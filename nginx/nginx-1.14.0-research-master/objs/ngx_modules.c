
#include <ngx_config.h>
#include <ngx_core.h>


extern ngx_module_t  ngx_core_module;
extern ngx_module_t  ngx_errlog_module;
extern ngx_module_t  ngx_conf_module;
extern ngx_module_t  ngx_events_module;
extern ngx_module_t  ngx_event_core_module;
extern ngx_module_t  ngx_epoll_module;
extern ngx_module_t  ngx_http_module;
extern ngx_module_t  ngx_http_core_module;
extern ngx_module_t  ngx_http_log_module;
extern ngx_module_t  ngx_http_upstream_module;
extern ngx_module_t  ngx_http_static_module;
extern ngx_module_t  ngx_http_autoindex_module;
extern ngx_module_t  ngx_http_index_module;
extern ngx_module_t  ngx_http_mirror_module;
extern ngx_module_t  ngx_http_try_files_module;
extern ngx_module_t  ngx_http_auth_basic_module;
extern ngx_module_t  ngx_http_access_module;
extern ngx_module_t  ngx_http_limit_conn_module;
extern ngx_module_t  ngx_http_limit_req_module;
extern ngx_module_t  ngx_http_geo_module;
extern ngx_module_t  ngx_http_map_module;
extern ngx_module_t  ngx_http_split_clients_module;
extern ngx_module_t  ngx_http_referer_module;
extern ngx_module_t  ngx_http_proxy_module;
extern ngx_module_t  ngx_http_fastcgi_module;
extern ngx_module_t  ngx_http_uwsgi_module;
extern ngx_module_t  ngx_http_scgi_module;
extern ngx_module_t  ngx_http_memcached_module;
extern ngx_module_t  ngx_http_empty_gif_module;
extern ngx_module_t  ngx_http_browser_module;
extern ngx_module_t  ngx_http_upstream_hash_module;
extern ngx_module_t  ngx_http_upstream_ip_hash_module;
extern ngx_module_t  ngx_http_upstream_least_conn_module;
extern ngx_module_t  ngx_http_upstream_keepalive_module;
extern ngx_module_t  ngx_http_upstream_zone_module;
extern ngx_module_t  ngx_http_write_filter_module;          /* 最后一个body filter，负责往外发送数据 */
extern ngx_module_t  ngx_http_header_filter_module;         /* 最后一个header filter，负责在内存中拼接出完整的http响应头，并调用ngx_http_write_filter发送 */
extern ngx_module_t  ngx_http_chunked_filter_module;        /* 对响应头中没有content_length头的请求，强制短连接（低于http 1.1）或采用chunked编码（http 1.1) */
extern ngx_module_t  ngx_http_range_header_filter_module;   /* header filter，负责处理range头 */
extern ngx_module_t  ngx_http_gzip_filter_module;           /* 支持流式的数据压缩 */
extern ngx_module_t  ngx_http_postpone_filter_module;       /* body filter，负责处理子请求和主请求数据的输出顺序 */
extern ngx_module_t  ngx_http_ssi_filter_module;            /* 支持过滤SSI请求，采用发起子请求的方式，去获取include进来的文件 */
extern ngx_module_t  ngx_http_charset_filter_module;        /* 支持添加charset，也支持将内容从一种字符集转换到另外一种字符集 */
extern ngx_module_t  ngx_http_userid_filter_module;         /* 支持添加统计用的识别用户的cookie */
extern ngx_module_t  ngx_http_headers_filter_module;        /* 支持设置expire和Cache-control头，支持添加任意名称的头 */

extern ngx_module_t  ngx_http_copy_filter_module;           /* 根据需求重新复制输出链表中的某些节点（比如将in_file的节点从文件读出并复制到新的节点），并交给后续filter进行处理 */
extern ngx_module_t  ngx_http_range_body_filter_module;     /* body filter，支持range功能，如果请求包含range请求，那就只发送range请求的一段内容 */
extern ngx_module_t  ngx_http_not_modified_filter_module;   /* 如果请求的if-modified-since等于回复的last-modified值，说明回复没有变化，清空所有回复的内容，返回304 */

//静态初始化
//1 用宏NGX_MODULE_V1初始化前7个字段，默认ctx_index 都是 NGX_MODULE_UNSET_INDEX，比如：ngx_core_module = {...
//2 用用全局对象ngx_mname_module_ctx的地址初始化ctx指针，比如：ngx_core_module_ctx
//3 用全局数组ngx_mname_commands[]初始化commands指针, 比如：ngx_core_commands[]
//4 nginx共有5种类型的模块，分别为"NGX_CORE_MODULE","NGX_CONF_MODULE","NGX_EVENT_MODULE","NGX_HTTP_MODULE","NGX_MAIL_MODULE"
//5 初始化init_master等callback
//6 用宏NGX_MODULE_V1_PADDING初始化最后8个字段, 即：spare_hook0到spare_hook7
ngx_module_t *ngx_modules[] = {
    &ngx_core_module,
    &ngx_errlog_module,
    &ngx_conf_module,
    &ngx_events_module,
    &ngx_event_core_module,
    &ngx_epoll_module,
    &ngx_http_module,
    &ngx_http_core_module,
    &ngx_http_log_module,
    &ngx_http_upstream_module,
    &ngx_http_static_module,
    &ngx_http_autoindex_module,
    &ngx_http_index_module,
    &ngx_http_mirror_module,
    &ngx_http_try_files_module,
    &ngx_http_auth_basic_module,
    &ngx_http_access_module,
    &ngx_http_limit_conn_module,
    &ngx_http_limit_req_module,
    &ngx_http_geo_module,
    &ngx_http_map_module,
    &ngx_http_split_clients_module,
    &ngx_http_referer_module,
    &ngx_http_proxy_module,
    &ngx_http_fastcgi_module,
    &ngx_http_uwsgi_module,
    &ngx_http_scgi_module,
    &ngx_http_memcached_module,
    &ngx_http_empty_gif_module,
    &ngx_http_browser_module,
    &ngx_http_upstream_hash_module,
    &ngx_http_upstream_ip_hash_module,
    &ngx_http_upstream_least_conn_module,
    &ngx_http_upstream_keepalive_module,
    &ngx_http_upstream_zone_module,
    &ngx_http_write_filter_module,
    &ngx_http_header_filter_module,
    &ngx_http_chunked_filter_module,
    &ngx_http_range_header_filter_module,
    &ngx_http_gzip_filter_module,
    &ngx_http_postpone_filter_module,
    &ngx_http_ssi_filter_module,
    &ngx_http_charset_filter_module,
    &ngx_http_userid_filter_module,
    &ngx_http_headers_filter_module,
    &ngx_http_copy_filter_module,
    &ngx_http_range_body_filter_module,
    &ngx_http_not_modified_filter_module,
    NULL
};

/*全局模块指针数组*/
char *ngx_module_names[] = {
    "ngx_core_module",
    "ngx_errlog_module",
    "ngx_conf_module",
    "ngx_events_module",
    "ngx_event_core_module",
    "ngx_epoll_module",
    "ngx_http_module",
    "ngx_http_core_module",
    "ngx_http_log_module",
    "ngx_http_upstream_module",
    "ngx_http_static_module",
    "ngx_http_autoindex_module",
    "ngx_http_index_module",
    "ngx_http_mirror_module",
    "ngx_http_try_files_module",
    "ngx_http_auth_basic_module",
    "ngx_http_access_module",
    "ngx_http_limit_conn_module",
    "ngx_http_limit_req_module",
    "ngx_http_geo_module",
    "ngx_http_map_module",
    "ngx_http_split_clients_module",
    "ngx_http_referer_module",
    "ngx_http_proxy_module",
    "ngx_http_fastcgi_module",
    "ngx_http_uwsgi_module",
    "ngx_http_scgi_module",
    "ngx_http_memcached_module",
    "ngx_http_empty_gif_module",
    "ngx_http_browser_module",
    "ngx_http_upstream_hash_module",
    "ngx_http_upstream_ip_hash_module",
    "ngx_http_upstream_least_conn_module",
    "ngx_http_upstream_keepalive_module",
    "ngx_http_upstream_zone_module",
    "ngx_http_write_filter_module",
    "ngx_http_header_filter_module",
    "ngx_http_chunked_filter_module",
    "ngx_http_range_header_filter_module",
    "ngx_http_gzip_filter_module",
    "ngx_http_postpone_filter_module",
    "ngx_http_ssi_filter_module",
    "ngx_http_charset_filter_module",
    "ngx_http_userid_filter_module",
    "ngx_http_headers_filter_module",
    "ngx_http_copy_filter_module",
    "ngx_http_range_body_filter_module",
    "ngx_http_not_modified_filter_module",
    NULL
};
