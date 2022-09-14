
/*
 * 队列双向循环链表实现文件
 *
 * 在Nginx的队列实现中，实质就是具有头结点的双向循环链表，这里的双向链表中的节点是没有数据区的，只有两个指向节点的指针，
 * 需要注意的是队列链表的内存分配不是直接从内存池中分配的，即没有进行内存管理，而是需要我们自己管理内存，
 * 所有我们可以指定它在内存池管理或直接在堆里面进行管理，最好使用内存池进行管理，节点结构定义如下
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef _NGX_QUEUE_H_INCLUDED_
#define _NGX_QUEUE_H_INCLUDED_

/**队列结构，其实质是具有由头节点的双向循环链表*/
typedef struct ngx_queue_s  ngx_queue_t;
/**队列中每个节点结构，只有两个指针，并没有数据区域*/
struct ngx_queue_s {
    ngx_queue_t  *prev;
    ngx_queue_t  *next;
};

/**
 * q 为链表结构体， ngx_queue_t 的指针，初始化链表
 * 即 节点指针都指向自己 表示空队列
 * */
#define ngx_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

/**h为链表容器结构体，ngx_queue_t 的指针，判断是否是空的链表*/
#define ngx_queue_empty(q)                                                    \
    (q == (q)->prev)

/**
 * h为链表容器结构体，ngx_queue_t 的指针
 * x为插入元素结构体中 ngx_queue_t 成员的指针
 * 将x 插入链表头部
 * */
#define ngx_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define ngx_queue_insert_after   ngx_queue_insert_head

/**
 * h 为链表容器结构体 ngx_queue_t 的指针，
 * x 为插入元素结构体 ngx_queue_t 成员的指针
 * 将x插入到链表尾部
 * */
#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x

/**
 * h 为链表容器结构体 ngx_queue_t 的指针
 * 返回 链表容器h中的第一个元素的 ngx_queue_t 结构体指针
 * */
#define ngx_queue_head(h)                                                     \
    (h)->next

/**
 * h 为链表容器结构体 ngx_queue_t 的指针
 * 返回链表容器h中的最后一个元素 ngx_queue_t结构体指针
 * */
#define ngx_queue_last(h)                                                     \
    (h)->prev

/**
 * h 为链表容器结构体 ngx_queue_t 的指针，返回链表结构体的指针
 */
#define ngx_queue_sentinel(h)                                                 \
    (h)

/**
 * q为链表中某一个元素结构体的 ngx_queue_t成员的指针，返回q元素的下一个元素
 */
#define ngx_queue_next(q)                                                     \
    (q)->next
/**
 * q为链表中某一个元素结构体的 ngx_queue_t成员的指针，返回q元素的上一个元素
 */
#define ngx_queue_prev(q)                                                     \
    (q)->prev


#if (NGX_DEBUG)
/**
 * x为链表容器结构体 ngx_queue_t 的指针，从容器中移除 x元素
 */
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next;                                              \
    (x)->prev = NULL;                                                         \
    (x)->next = NULL

#else

#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#endif

/**
 * h 为链表容器结构体 ngx_queue_t的指针，该函数用于拆分链表
 * h 是链表容器，而且 q 是链表h中的一个元素
 * 将 链表h以元素q为界限拆分成两个链表 h 和n
 */
#define ngx_queue_split(h, q, n)                                              \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

/**
 * h为链表容器结构体 ngx_queue_t的指针，n为另一个链表容器结构体 ngx_queue_t的指针
 * 合并链表 ，将 n链表添加到h链表的末尾
 */
#define ngx_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;

/***
 * q为链表中某一个元素结构体的 ngx_queue_t成员的指针，type是链表元素的结构体类型名称
 * link是上麦结构体中ngx_queue_t类型的成员名字，返回q元素所属结构体的地址
 */
#define ngx_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))


ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue);
void ngx_queue_sort(ngx_queue_t *queue,ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *));


#endif /* _NGX_QUEUE_H_INCLUDED_ */
