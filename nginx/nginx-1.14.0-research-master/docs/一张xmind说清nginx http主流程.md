# 一张脑图说清 Nginx 的主流程

这个脑图在 [nginx-1.14.0-research](https://github.com/its-tech/nginx-1.14.0-research/tree/master/docs) 上。这是我在研究nginx的http模块的时候画的。基本上把 Nginx 主流程（特别是 HTTP 的部分）的关键函数和关键设置画了下来，了解了这个脑图，就对整个 Nginx 的主流程有了定性的了解了。

Nginx 的启动过程分为两个部分，一个部分是读取配置文件，做配置文件中配置的一些事情（比如监听端口等）。第二个部分是形成 Master-Worker 的多进程模型。这两个过程就是 Nginx 代码中最重要的两个函数：`ngx_init_cycle` 和 `ngx_master_process_cycle`

![](http://tuchuang.funaio.cn/18-6-29/29766630.jpg)

# ngx_init_cycle

ngx_init_cycle 是 Nginx 中最重要的函数，没有之一。我们可以想想，如果我们写一个和 Nginx 一样的 Web 服务，我们会怎么做？我们大致的思路一定是解析配置文件，把配置文件存入到一个数据结构中，然后根据数据结构，进行端口监听。是的，差不多，Nginx 就是这么一个流程。不过 Nginx 里面有个模块的概念，所有的功能都是用模块的方式进行加载的。

## Nginx 的模块

Nginx 的模块分为几类，这几类分别为 Core，Event，Conf，Http，Mail。看名字就知道 Core 模块是最重要的。模块是什么意思呢？它包含一堆命令（cmd）和命令对应的处理函数（cmd->handler），我们根据配置文件中的配置（token）就知道这个配置是属于哪个模块的哪个命令，然后调用命令对应的处理函数来处理或者设置我们的服务。

这几类模块中，Core 模块是 Nginx 启动的时候一定会加载的，其他的模块，只有在解析配置的时候，遇到了这个模块的命令，才会加载对应的模块。
这个也是体现了 Nginx 按需加载的理念。（昨天还和小组成员讨论，如果我们写的话，可能就会先把所有模块都加载，然后根据配置文件进行匹配，这样可能 Nginx 的启动过程和进程资源就变大了）。

模块的另一个问题是我这个 Nginx 最多有哪些模块的能力呢？这个是编译的时候就决定了，Nginx 的编译过程可以参考这篇[文章](https://www.cnblogs.com/yjf512/p/9177562.html) 。我们可以不用管./configure 的时候的具体内容，但是我们最关注的就是 `objs/ngx_modules.c` 这个编译出来的文件，里面有个`ngx_modules`全局变量，这个变量里面就存放了我们这次编译的 Nginx 最多可以支持的模块。

模块的结构是我们需要关注的另外一个问题。 Nginx 中模块的结构叫做`ngx_module_s`（你或许会看到`ngx_module_t`，其实就是`struct ngx_moudle_s`的缩写）

![](http://tuchuang.funaio.cn/18-6-29/26236212.jpg)

里面有个结构`*ctx`，对于不同的模块类型，这个`ctx`指向的结构是不一样的，我们这里最主要是研究 HTTP 类型的模块，所以我们就记得 HTTP 模块指向的结构是`ngx_http_module_t`

![](http://tuchuang.funaio.cn/18-6-29/79112482.jpg)

## 主流程

了解了 Nginx 的模块概念，我们再回到`ngx_init_cycle`函数

![](http://tuchuang.funaio.cn/18-6-29/63307342.jpg)

这个函数里面做了几个事情:

* `ngx_cycle_modules`，它本质就是把`objs/ngx_modules.c`里面的全局变量拷贝到`cycle`这个全局变量里面
* 调用了每个 Core 类型模块的`create_conf`方法
* `ngx_conf_parse` 解析配置文件，调用每个Core 类型模块的`init_conf`方法
* 调用了每个 Core 类型模块的`init_conf`方法
* `ngx_open_listening_sockets` 打开配置文件中设置的监听端口和IP
* `ngx_init_modules` 调用每个加载模块的`init_module`方法

`create_conf`是创建一些模块需要初始化的结构，但是这个结构里面并没有具体的值。`init_conf`是往这些初始化结构里面填写配置文件中解析出来的信息。

其中的`ngx_conf_parse`是真正解析配置文件的。

在代码`ngx_open_listening_sockets`里面我们看到熟悉的bind，listen的命令。所以 Nginx 是如何多个进程同时监听一个80端口的？本质是启动了一个master进程，在`ngx_init_cycle`里面监听了端口，然后在`ngx_master_process_cycle`里面 fork 出来多个 worker 子进程。

## ngx_conf_parse

这个函数是非常非常重要的。

![](http://tuchuang.funaio.cn/18-6-29/56409966.jpg)

它的逻辑，就是这两步，首先使用函数`ngx_conf_read_token`先循环逐行逐字符查找，看匹配的字符，获取出`cmd`, 然后去所有的模块查找对应的`cmd`,调用那个查找后的`cmd->set`方法。用Http模块举例子，我们的配置文件中一定有且只有一个关键字叫http
```
http{

}
```
先解析这个配置的时候发现了`http`这个关键字，然后去各个模块匹配，发现`ngx_http_module`这个模块包含了`http`命令。它对应的set方法是`ngx_http_block`。这个方法就是http模块非常重要的方法了。当然，这里顺带提一下，event模块也有类似的方法，`ngx_events_block`。它具体做的事情就是解析
```
event epoll
```
这样的命令，并创建出事件驱动的模型。

## ngx_http_block

这个函数是`http`模块加载的时候最重要的函数，首先，它会遍历`modules.c`中的所有 http 模块，还记得上文说的，HTTP 模块结构`ngx_module_s`中的`**ctx` 指向的是 `ngx_http_module`。

![](http://tuchuang.funaio.cn/18-7-2/24841255.jpg)


### 第一步，模块对三个层级的回调

它内部有这个 HTTP 模块定义的，在各个层级（http，server，location）所需要加载回调的方法。

我们这里再附带说一下 HTTP 的三个层级，这三个层级对应我们配置文件里面的三个不同的 Block 语句。
```
http {
  server {
    listen       80;
    location {
      root   html;
      index  index.html;
    }
  }
}
```
这三个层次里面的命令有可能会有重复，有冲突。比如，root这个命令，在 location 中可以有，在 server 中也可以有，如果赋值不一致的化，是上层覆盖下层，还是下层覆盖上层（当然大部分都是下层覆盖上层）。这个就在具体的模块定义的`ngx_http_module`结构中定义了 `create_main_conf`, `create_srv_conf`,...,`merge_srv_conf`等方法。这些方法的调用就是在`ngx_http_block`方法的第一步进行调用的。

### 第二步，设置连接回调和请求监听回调

第二步是调用方法`ngx_http_optimize_servers`。它对配置文件中的所有listening的端口和IP进行监听设置。记住，这里只是进行回调的设置，具体的`listening`和`binding`操作不是在`ngx_conf_parse`中，而是在`ngx_open_listening_sockets` 中。

![](http://tuchuang.funaio.cn/18-7-2/69227664.jpg)

这个XMind中有标注（xxx时候回调）的分支就是只有在事件回调的时候会进行调用，不是在`ngx_conf_parse`的时候调用的。

我们再仔细看看这个脑图中的流程，在conf_parse的时候，我实际上只对HTTP连接的时候设置了一个回调函数（ngx_http_init_connection）。在有HTTP连接上来的时候，才会设置读请求的回调（ngx_http_wait_request_handler）。在这个回调，才是真正的解析 HTTP 请求的请求头，请求体等。nginx 中著名的11阶段就是在这个地方进行一个个步骤进行调用的。

这里说一下回调。nginx 是由各种各样的回调组合起来的。回调就需要要求有一个事件驱动机制。在nginx中，这个事件驱动机制也是一个模块，event 模块。在编译的时候，编译程序会判断你的系统支持哪些事件驱动，比如我的是centos，支持的是epoll，在配置文件配置
```
event epoll;
```
之后，就用这个epoll事件驱动监听IO事件。其他模块和事件驱动的交互就是通过`ngx_add_event`进行事件监听和回调的。

http请求由于可能请求体或者返回体比较大，所以不一定会在一个事件中完成，为了整体的 nginx 高效，http 模块在处理 http请求的时候，处理完成了一个event回调函数之后，如果没有处理完成整个HTTP，就会在event中继续注册一个回调，然后把处理权和资源都交给事件驱动中心。等待事件驱动下一次触发回调。

### 第三步，初始化定义 HTTP 的11个处理阶段

HTTP请求在nginx中会经过11个处理阶段和他们的checker方法：

* NGX_HTTP_POST_READ_PHASE阶段（ngx_http_core_generic_phase）
* NGX_HTTP_SERVER_REWRITE_PHASE阶段（ngx_http_rewrite_handler）
* NGX_HTTP_FIND_CONFIG_PHASE阶段（ngx_http_core_find_config_phase）
* NGX_HTTP_REWRITE_PHASE阶段（ngx_http_rewrite_handler）
* NGX_HTTP_POST_REWRITE_PHASE阶段（ngx_http_core_post_rewrite_phase）
* NGX_HTTP_PREACCESS_PHASE阶段（ngx_http_core_generic_phase）
* NGX_HTTP_ACCESS_PHASE阶段（ngx_http_core_access_phase）
* NGX_HTTP_POST_ACCESS_PHASE阶段（ngx_http_core_post_access_phase）
* NGX_HTTP_TRY_FILES_PHASE阶段（ngx_http_core_try_files_phase）
* NGX_HTTP_CONTENT_PHASE阶段（ngx_http_core_content_phase）
* NGX_HTTP_LOG_PHASE阶段（ngx_http_log_module中的ngx_http_log_handler）

就像把大象放冰箱需要几步，处理 HTTP 需要11步，这11个步骤，有的是处理配置文件，有的是处理rewrite，有的是处理权限，有的是处理日志。所以，如果我们要自己开发一个http模块，我们就需要定义我们这个http模块处理http请求的这11个阶段（当然并不是这11个阶段都可以被自定义的，有的阶段是不能被自定义模块设置的）。然后当一个请求进来的时候，就按照顺序把请求经过所有模块的这11个阶段。

这里所谓的经过这些11个阶段本质上就是调用他们的 checker 方法。这些checker方法除了最后一个 NGX_HTTP_LOG_NGX_HTTP_LOG_PAHSE 是在 ngx_http_log_module 里面之外，其他的都是在 http 的 core 模块中定义好了。

### 其他

这里其他的函数调用就没有特别需要注意的了。

## ngx_http_wait_request_handler

我们继续跟着`ngx_http_optimize_servers`,`ngx_http_init_listening`,`ngx_http_add_listening`,`ngx_http_init_connection` 进入到处理http请求内容的函数里面。

![](http://tuchuang.funaio.cn/18-7-2/8260342.jpg)

这个函数先调用recv将http请求的数据获取到，然后调用`ngx_http_create_request`创建了 HTTP 的请求结构体。

首先nginx处理的是HTTP的第一行，就是`HTTP 1.0 GET`, 从这一行，HTTP 会获取到协议，方法等。接着再调用`ngx_http_process_request_line`一行一行处理请求头。`ngx_http_read_request_header`，`ngx_http_process_request_headers`。处理完成 http header 头之后，`ngx_http_process_request` 再接着进行后续的处理

`ngx_http_process_request` 设置了读和写的handler，并且把监听事件从epoll事件驱动队列中拿出来（ngx_http_block_reading），就代表这个时候是阻塞做事件处理的事情。然后再调用`ngx_http_core_run_phases`来让请求经过那11个阶段。

其实并不是所有请求都需要读取整个HTTP请求体。比如你只是获取一个css文件，nginx在11个阶段中的配置读取阶段就不会再继续读取HTTP的body了。但是如果是一个fastcgi请求，在http_fastcgi的模块中，就会进入到NGX_HTTP_CONTENT_PHASE阶段，在这个阶段，它做的一个事情就是循环读取读缓存区的数据，直到读取完毕，然后进行处理，再返回结构。
