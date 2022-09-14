
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */
/**
 * 返回队列链表中心元素
 * @param queue
 * @return
 */
ngx_queue_t * ngx_queue_middle(ngx_queue_t *queue)
{
    ngx_queue_t  *middle, *next;
    /**获取头部节点*/
    middle = ngx_queue_head(queue);
    if (middle == ngx_queue_last(queue)) {
        // 判断尾部是否和头部相等，若相等则说明是一个元素，是自己即可；
        return middle;
    }
    //临时指针 next, 首先执行队列链表的头结点
    next = ngx_queue_head(queue);
    for ( ;; ) {
        /**若队列链表不止一个元素，则等价 middle = ngx_queue_next(middle)*/
        middle = ngx_queue_next(middle);

        next = ngx_queue_next(next);
        if (next == ngx_queue_last(queue)) {
            // 链表有偶数个元素
            return middle;
        }

        next = ngx_queue_next(next);
        if (next == ngx_queue_last(queue)) {
            // 链表有奇数个元素
            return middle;
        }
    }
}


/* the stable insertion sort */
/**
 * 队列链表排序采用的是稳定的简单插入排序方法，
 * 即从第一个节点开始遍历，
 * 依次将当前节点(q)插入前面已经排好序的队列(链表)中，
 * 下面程序中，前面已经排好序的队列的尾节点为prev。操作如下
 * @param queue
 * @param cmp
 */
void ngx_queue_sort(ngx_queue_t *queue,ngx_int_t (*cmp)(const ngx_queue_t *, const ngx_queue_t *))
{
    ngx_queue_t  *q, *prev, *next;
    q = ngx_queue_head(queue);
    if (q == ngx_queue_last(queue)) {
        // 若只有一个元素则返回
        return;
    }
    /**遍历整个链表*/
    for (q = ngx_queue_next(q); q != ngx_queue_sentinel(queue); q = next) {
        prev = ngx_queue_prev(q);
        next = ngx_queue_next(q);
        // 把元素q独立出来
        ngx_queue_remove(q);
        // 找到合适q的位置
        do {
            if (cmp(prev, q) <= 0) {
                break;
            }
            prev = ngx_queue_prev(prev);
        } while (prev != ngx_queue_sentinel(queue));
        /**在prev 节点前插入 q*/
        ngx_queue_insert_after(prev, q);
    }
}
