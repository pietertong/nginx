
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/**
 * 创建链表
 * @param pool
 * @param n
 * @param size
 * @return
 */
ngx_list_t *
ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size)
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
 * 添加一个元素
 *
 * @param l
 * @return
 */
void *ngx_list_push(ngx_list_t *l)
{
    void             *elt;
    ngx_list_part_t  *last;

    /**
     * last节点指针指向链表最后一个节点
     */
    last = l->last;

    /**
     * 若最后一个节点的数据区已经满了
     */
    if (last->nelts == l->nalloc) {

        /* the last part is full, allocate a new list part */

        /**
         * 则分配一个新的节点
         */
        last = ngx_palloc(l->pool, sizeof(ngx_list_part_t));
        if (last == NULL) {
            return NULL;
        }

        /**
         * 分配新节点数据区内存，并使节点结构指向该数据区的首地址
         */
        last->elts = ngx_palloc(l->pool, l->nalloc * l->size);
        if (last->elts == NULL) {
            return NULL;
        }

        /**
         * 初始化新节点结构
         */
        last->nelts = 0;
        last->next = NULL;

        /**
         * 把新节点连接到现有的链表中
         */
        l->last->next = last;
        l->last = last;
    }

    /**
     * 计算存储新元素的位置
     */
    elt = (char *) last->elts + l->size * last->nelts;
    last->nelts++;/** 实际存放元素 + 1*/

    /**
     * 返回新元素所在位置
     */
    return elt;
}
