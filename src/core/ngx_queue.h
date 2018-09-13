
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
/**
 * 在 Nginx的队列链表中，其维护的是指向链表节点的指针，并没有实际的数据区，
 * 所有对实际数据的操作需要我们自行操作，队列链表实质是双向循环链表，其操作是双向链表的基本操作
 */

#include <ngx_config.h>
#include <ngx_core.h>


#ifndef _NGX_QUEUE_H_INCLUDED_
#define _NGX_QUEUE_H_INCLUDED_

/**
 * 队列双向链表结构
 */

 /**
  * 队列结构体，其实质是具有有头节点的双向循环链表
  */
typedef struct ngx_queue_s  ngx_queue_t;

/**
 * 队列中每个节点结构，只有两个指针，并没有数据区域
 */
struct ngx_queue_s {
    ngx_queue_t  *prev;
    ngx_queue_t  *next;
};

/**
 *队列链表操作
 */

/**
 * q 为链表结构体 ngx_queue_t的指针；初始化 双链表
 *
 * 初始化队列，即节点指针指向自己，表示为空队列
 */
#define ngx_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

/**
 * h 为 链表容器结构体 ngx_queue_t的指针；
 * 判断链表是否为空
 */
#define ngx_queue_empty(h)                                                    \
    (h == (h)->prev)

/**
 * h 为 链表容器结构体 ngx_queue_t的指针，
 * x 为 插入元素结构体中 ngx_queue_t成员的指针，
 * 将 x 插入到 链表头部
 *
 * 在队列头节点的下一节点插入新节点，其中 h 为头节点，x为新节点
 */
#define ngx_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x

/**
 *
 */
#define ngx_queue_insert_after   ngx_queue_insert_head

/**
 * h 为 链表容器结构体 ngx_queue_t的指针，
 * x 为插入元素结构体中 ngx_queue_t成员的指针，
 * 将 x 插入到链表的尾部
 *
 * 在队列尾节点之后插入新节点，其中 h 为尾节点，x 为新节点
 */
#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x


/**
 * 获取队列头节点
 *
 * h 为链表容器结构体 ngx_queue_t的指针
 * 返回链表容器 h 中的第一个元素的 ngx_queue_t结构体指针
 */
#define ngx_queue_head(h)                                                     \
    (h)->next

/**
 * 获取队列尾节点
 *
 * h 为 链表容器结构体 ngx_queue_t的指针
 * 返回链表容器 h 中的最后一个元素 的ngx_queue_t结构体指针
 */
#define ngx_queue_last(h)                                                     \
    (h)->prev

/**
 * h 为 链表容器结构体 ngx_queue_t的指针，
 * 返回链表结构体的指针
 */
#define ngx_queue_sentinel(h)                                                 \
    (h)

/**
 * 获取队列指定节点的下一个节点
 */
#define ngx_queue_next(q)                                                     \
    (q)->next

/**
 * 获取队列指定节点的前一个节点
 */
#define ngx_queue_prev(q)                                                     \
    (q)->prev


/**
 * x 为链表容器结构体 ngx_queue_t的指针，从容器移除 x 元素
 */
#if (NGX_DEBUG)
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next;                                              \
    (x)->prev = NULL;                                                         \
    (x)->next = NULL

#else
/**
 * 删除队列指定的节点
 */
#define ngx_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#endif

/**
 * 拆分队列链表，使其称为两个独立的队列链表；
 * 其中 h 是原始队列的头节点，q 是原始队列中的一个元素节点，n是新的节点
 * 拆分后，原始队列 以q 为分界，头节点h到q之前的节点作为一个队列，（不包括q节点），
 * 另一个队列是以 n 为头结点，以节点 q 及其之后的节点作为新的队列链表
 */
/**
 * h 为链表容器结构体 ngx_queue_t的指针，该函数用于拆分链表
 * h 是链表容器，而 q是链表 h中的一个元素
 * 将链表 h 以元素 q 为界 拆分成两个链表 h 和 n
 */
#define ngx_queue_split(h, q, n)                                              \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

/**
 * h 为链表容器结构体 ngx_queue_t的指针
 * n 为 另一个链表容器结构体 ngx_queue_t的指针
 * 合并链表
 * 将 n 链表添加到 h 链表的末尾
 */
#define ngx_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;


/**
 * 有队列基本结构和以上操作可知，
 *
 * Nginx的队列操作只队链表指针进行简单的修改指向操作，并不负责节点数据空间的分配
 * 因此，用户在使用Nginx队列时，要自己定义数据结构分配空间，
 * 且在其中包含一个ngx_queue的指针或对象，当需要获取队列节点数据时，
 * 使用 ngx_queue_data宏，其定义如下
 */
/**
 * q 为 链表中某一个元素结构体的 ngx_queue_t成员的指针，
 * type 是链表 元素的结构体类型名称
 * link 是上面这个结构体中 ngx_queue_t类型的成员名字
 * 返回 q 元素所属结构体的地址
 */
#define ngx_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))

/**
 * *queue 为链表容器结构体 ngx_queue_t的指针
 * 返回链表中心元素 即 第N/2+1 个
 * @param queue
 * @return
 */
ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue);

/**
 *
 *
 * @param queue
 * @param cmp
 */
void ngx_queue_sort(ngx_queue_t *queue, ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *));


#endif /* _NGX_QUEUE_H_INCLUDED_ */
