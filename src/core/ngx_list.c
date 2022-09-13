
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 创建链表
 * 创建链表时，首先分配链表表头，在对链表进行初始化，在初始化过程中分配头节点数据区内存
 * @param pool
 * @param n
 * @param size
 * @return
 */
ngx_list_t * ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    ngx_list_t  *list;
    /**
     * 分配链表表头的内存
     */
    list = ngx_palloc(pool, sizeof(ngx_list_t));
    if (list == NULL) {
        return NULL;
    }
    /**
     * 初始化链表
     */
    if (ngx_list_init(list, pool, n, size) != NGX_OK) {
        return NULL;
    }
    return list;
}

/**
 * 添加元素
 * 添加元素到链表，都是从最后一个节点开始，
 * 首先判断最后一个节点的数据区是否由内存存放新增加的元素，
 * 若足以存储该新元素，则返回存储新元素内存的位置
 * 若没有足够的内存存储新增加的元素，则分配一个新的节点，再把该新的节点链接到现有链表中，并返回存储新元素内存的位置
 * 注意：
 *  添加的元素可以是整数，也可以是一个结构
 * @param l
 * @return
 * 添加一个元素
 */
void * ngx_list_push(ngx_list_t *l)
{
    void             *elt;
    ngx_list_part_t  *last;
    // last 节点指针指向链表的最后一个节点
    last = l->last;

    if (last->nelts == l->nalloc) {
        /** 若最后一个节点的数据区已满*/
        /* the last part is full, allocate a new list part */
        // 在原来的资源池中操作申请一个新的节点
        last = ngx_palloc(l->pool, sizeof(ngx_list_part_t));
        if (last == NULL) {
            // 没有申请到
            return NULL;
        }
       /** 分配新节点数据区内存，并使节点结构指向数据区的首地址*/
        last->elts = ngx_palloc(l->pool, l->nalloc * l->size);
        if (last->elts == NULL) {
            // 没有申请到
            return NULL;
        }

        last->nelts = 0;
        last->next = NULL;

        l->last->next = last;
        l->last = last;
    }

    //计算存储新元素的位置
    elt = (char *) last->elts + l->size * last->nelts;
    last->nelts++; /**实际存放元素 + 1*/
    /**返回新元素所在的位置*/
    return elt;
}
