
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     NGX_DEFAULT_POOL_SIZE
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

/*
共享内存结构
*/
struct ngx_shm_zone_s {
    void                     *data; // 并不是具体的数据，而是成功回调方法的上下文
    ngx_shm_t                 shm; // 具体的共享内存数据结构，这才是真正保存指向共享内存区指针的对象
    ngx_shm_zone_init_pt      init; // 初始化回调方法
    void                     *tag;  // 标签，一般指向模块，表示这个共享内存是什么模块创建的
    void                     *sync;
    ngx_uint_t                noreuse;  // 是否禁止复用，默认允许复用
};

// 全局变量
// modules,conf,listening,connections,events
struct ngx_cycle_s {
    //为什么conf_ctx要有4重指针？http://www.pagefault.info/?p=368
    void                  ****conf_ctx; // 各个模块的配置
    ngx_pool_t               *pool;

    ngx_log_t                *log;
    ngx_log_t                 new_log;

    ngx_uint_t                log_use_stderr;  /* unsigned  log_use_stderr:1; */

    ngx_connection_t        **files;    //连接文件
    ngx_connection_t         *free_connections;     //空闲连接
    ngx_uint_t                free_connection_n;    //空闲连接个数

    ngx_module_t            **modules;  // 所有的模块
    ngx_uint_t                modules_n;
    ngx_uint_t                modules_used;    /* unsigned  modules_used:1; */

    ngx_queue_t               reusable_connections_queue;   //再利用连接队列
    ngx_uint_t                reusable_connections_n;

    ngx_array_t               listening;    //监听数组
    ngx_array_t               paths;        //路径数组

    ngx_array_t               config_dump;
    ngx_rbtree_t              config_dump_rbtree;
    ngx_rbtree_node_t         config_dump_sentinel;

    ngx_list_t                open_files;
    ngx_list_t                shared_memory;    //共享内存链表

    ngx_uint_t                connection_n;     //连接个数
    ngx_uint_t                files_n;          //打开文件个数

    ngx_connection_t         *connections;      //连接
    ngx_event_t              *read_events;      //读事件
    ngx_event_t              *write_events;     //写事件

    ngx_cycle_t              *old_cycle;        //

    ngx_str_t                 conf_file;        //
    ngx_str_t                 conf_param;       //
    ngx_str_t                 conf_prefix;      //配置前缀
    ngx_str_t                 prefix;           //前缀
    ngx_str_t                 lock_file;        //锁文件
    ngx_str_t                 hostname;         //主机名
};


typedef struct {
    ngx_flag_t                daemon;
    ngx_flag_t                master;

    ngx_msec_t                timer_resolution;
    ngx_msec_t                shutdown_timeout;

    ngx_int_t                 worker_processes;
    ngx_int_t                 debug_points;

    ngx_int_t                 rlimit_nofile;
    off_t                     rlimit_core;

    int                       priority;

    ngx_uint_t                cpu_affinity_auto;
    ngx_uint_t                cpu_affinity_n;
    ngx_cpuset_t             *cpu_affinity;

    char                     *username;
    ngx_uid_t                 user;
    ngx_gid_t                 group;

    ngx_str_t                 working_directory;
    ngx_str_t                 lock_file;

    ngx_str_t                 pid;
    ngx_str_t                 oldpid;

    ngx_array_t               env;
    char                    **environment;

    ngx_uint_t                transparent;  /* unsigned  transparent:1; */
} ngx_core_conf_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
ngx_cpuset_t *ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);
void ngx_set_shutdown_timer(ngx_cycle_t *cycle);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_dump_config;
extern ngx_uint_t             ngx_quiet_mode;


#endif /* _NGX_CYCLE_H_INCLUDED_ */
