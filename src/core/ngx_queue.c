
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * Nginx 队列双向链表结构
 */
/**
 *
 * @param queue
 * @return
 */

/**
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */

/**
 *
 * 返回队列链表中心元素
 * @param queue
 * @return
 */
ngx_queue_t *ngx_queue_middle(ngx_queue_t *queue)
{
    ngx_queue_t  *middle, *next;

    /**
     * 获取队列链表头节点
     */
    middle = ngx_queue_head(queue);

    /**
     * 若队列链表的头节点就是尾节点，则表示该队列链表只有一个元素
     */
    if (middle == ngx_queue_last(queue)) {
        return middle;
    }

    /**
     * next 作为临时指针，首先指向队列链表的头节点
     */
    next = ngx_queue_head(queue);

    for ( ;; ) {
        /**
         * 若队列链表不止一个元素，则等价于 middle = middle->next
         */
        middle = ngx_queue_next(middle);

        next = ngx_queue_next(next);

        /**
         * 队列链表有偶数个元素
         */
        if (next == ngx_queue_last(queue)) {
            return middle;
        }

        next = ngx_queue_next(next);

        /**
         * 队列链表有奇数个元素
         */
        if (next == ngx_queue_last(queue)) {
            return middle;
        }
    }
}

/**
 * 链表排序，
 *
 * 队列链表排序采用的是稳定的简单插入排序方法
 * 即从第一个节点开始遍历，依次将当前节点 （q） 插入前面已经排好序的队列（链表）中，
 *
 * 下面程序中，
 *
 * 前面已经排好序的队列的尾节点为prev.
 *
 * @param queue
 * @param cmp
 */
/* the stable insertion sort */

void ngx_queue_sort(
    ngx_queue_t *queue,
    ngx_int_t (*cmp)
    (
        const ngx_queue_t *,
        const ngx_queue_t *
    )
) {
    ngx_queue_t  *q, *prev, *next;

    q = ngx_queue_head(queue);

    /**
     * 若队列链表只有一个元素，则直接返回
     */
    if (q == ngx_queue_last(queue)) {
        return;
    }

    for (q = ngx_queue_next(q); q != ngx_queue_sentinel(queue); q = next) {

        prev = ngx_queue_prev(q);
        next = ngx_queue_next(q);

        /**
         * 首先是把元素节点 q 独立出来
         */
        ngx_queue_remove(q);

        do {
            if (cmp(prev, q) <= 0) {
                break;
            }

            prev = ngx_queue_prev(prev);

        } while (prev != ngx_queue_sentinel(queue));

        /**
         * 插入元素节点 q
         */
        ngx_queue_insert_after(prev, q);
    }
}
