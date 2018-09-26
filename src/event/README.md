[root@localhost event]# tree .
.
├── modules
│   ├── ngx_devpoll_module.c /** dev/pool 事件驱动模型*/
│   ├── ngx_epoll_module.c /** epoll 事件驱动模型*/
│   ├── ngx_eventport_module.c /** 事件驱动模型端口*/
│   ├── ngx_iocp_module.c /***/
│   ├── ngx_iocp_module.h /***/
│   ├── ngx_kqueue_module.c /** kqueue 事件驱动模型*/
│   ├── ngx_poll_module.c /** poll 事件驱动模型*/
│   ├── ngx_select_module.c /** select 事件驱动模型*/
│   └── ngx_win32_select_module.c /** Win32 平台下的select 事件驱动模型*/
├── ngx_event_accept.c
├── ngx_event_acceptex.c
├── ngx_event.c
├── ngx_event_connect.c
├── ngx_event_connectex.c
├── ngx_event_connect.h
├── ngx_event.h
├── ngx_event_openssl.c
├── ngx_event_openssl.h
├── ngx_event_openssl_stapling.c
├── ngx_event_pipe.c
├── ngx_event_pipe.h
├── ngx_event_posted.c
├── ngx_event_posted.h
├── ngx_event_timer.c
├── ngx_event_timer.h
├── ngx_event_udp.c
└── README.md

1 directory, 27 files
[root@localhost event]#

modules 子目录里面的源代码实现了 Nginx 支持的事件驱动模型

    AIO
    epoll
    kqueue
    select
    /dev/poll
    poll

等事件模型驱动

除去 modules 子目录外，其他的文件提供了
    事件驱动相关的数据结构定义
    初始化
    事件接收
    传递
    管理功能以及事件驱动模型调用功能