# read nginx open source code
# version 1.15.3

[root@localhost src]# tree -L 1
 .
 ├── core
 ├── event
 ├── http
 ├── mail
 ├── misc
 ├── os
 └── stream

 7 directories, 0 files
 [root@localhost src]#

 core : Nginx 的核心代码，内存池，链表，hashmap String 等常用的数据结构 以及Nginx 内核实现的核心代码
 event : Nginx 的事件驱动模型，Libevent 以及定时器的实现相关代码
 http : Nginx 实现 http服务相关的代码
 mail : Nginx 实现 邮件代理服务器相关的代码
 misc : 辅助 代码，测试 C++ 头 的兼容性，以及 对 Google_PerfTools的支持
 os : 不同体系结构所提供的系统函数的封装，提供对外统一的系统调用接口；

 [root@localhost core]# tree -L 1
 .
 /** 实现对个模块的整体控制，是 Nginx 程序 main 函数*/
 ├── nginx.c
 ├── nginx.h
 /** 以下是基本数据结构及其操作 start */
 ├── ngx_array.c
 ├── ngx_array.h

 ├── ngx_string.c
 ├── ngx_string.h

 ├── ngx_hash.c
 ├── ngx_hash.h

 ├── ngx_list.c
 ├── ngx_list.h

 ├── ngx_queue.c
 ├── ngx_queue.h

 ├── ngx_radix_tree.c
 ├── ngx_radix_tree.h

 ├── ngx_rbtree.c
 ├── ngx_rbtree.h

 ├── ngx_output_chain.c

 ├── ngx_buf.c
 ├── ngx_buf.h

 ├── ngx_module.c
 ├── ngx_module.h

 ├── ngx_open_file_cache.c
 ├── ngx_open_file_cache.h
 ├── ngx_palloc.c
 ├── ngx_palloc.h
 ├── ngx_parse_time.c
 ├── ngx_parse_time.h

 ├── ngx_rwlock.c
 ├── ngx_rwlock.h

 ├── ngx_shmtx.c
 ├── ngx_shmtx.h
 ├── ngx_slab.c
 ├── ngx_slab.h

 ├── ngx_thread_pool.c
 ├── ngx_thread_pool.h

 /** 实现支持正则表达式*/
 ├── ngx_regex.c
 ├── ngx_regex.h

 /** 反向代理的协议信息*/
 ├── ngx_proxy_protocol.c
 ├── ngx_proxy_protocol.h

 /** PCRE 上层封装*/
 ├── ngx_parse.c
 ├── ngx_parse.h

 /** hash字符串操作*/
 ├── ngx_md5.c
 ├── ngx_md5.h
 ├── ngx_murmurhash.c
 ├── ngx_murmurhash.h

 /** 实现文件读写相关的功能*/
 ├── ngx_file.c
 ├── ngx_file.h

 /** 实现对系统运行过程参数、资源的通用管理*/
 ├── ngx_cycle.c
 ├── ngx_cycle.h

 /** 实现日志输出、管理的相关功能*/
 ├── ngx_log.c
 ├── ngx_log.h
 ├── ngx_syslog.c
 ├── ngx_syslog.h

 /** socket 网络套接字功能*/
 ├── ngx_inet.c
 ├── ngx_inet.h

 /** 定义一些头文件于结构别名*/
 ├── ngx_core.h
 ├── ngx_cpuinfo.c

 /** 时间获取和管理功能 start*/
 ├── ngx_times.c
 └── ngx_times.h
 /** 时间获取和管理功能 end*/

 /** 整个Nginx模块构架基本配置管理 start*/
 ├── ngx_conf_file.c
 ├── ngx_conf_file.h
 ├── ngx_config.h
 /** 整个Nginx模块构架基本配置管理 end*/

 /** 网络连接管理 start*/
 ├── ngx_connection.c
 ├── ngx_connection.h
 /** 网络连接管理 end*/

 /** CRC 校验表信息 start*/
 ├── ngx_crc32.c
 ├── ngx_crc32.h
 ├── ngx_crc.h
 /** CRC 校验表信息 end*/

 /**其他文件*/
 ├── ngx_resolver.c
 ├── ngx_resolver.h
 ├── ngx_sha1.c
 ├── ngx_sha1.h
 ├── ngx_spinlock.c
 ├── ngx_crypt.c
 ├── ngx_crypt.h


 1 directory, 74 files
 [root@localhost core]#
 [参考]
https://github.com/y123456yz/reading-code-of-nginx-1.9.2/blob/master/
https://blog.csdn.net/initphp/category_9265172.html

 根据各个模块功能，可以把Nginx 源码划分如下几个部分


                       【   Nginx   源   码  】
        ______________________|__________|_______________________________
       |          |           |          |       |          |            |
【核心功能模块】 【配置解析】【内存管理】【事件驱动】  【日志管理】 【Http 服务】 【Mail 服务】

【核心功能模块】：
【配置解析】：
【内存管理】：
【事件驱动】：
【日志管理】：
【Http服务】：
【Mail服务】：


nginx -> CMakeList.txt 脚本生产
