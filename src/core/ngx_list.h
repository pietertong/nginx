
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/**
 * 链表结构
 */
typedef struct ngx_list_part_s  ngx_list_part_t;

/**
 * 链表中的节点结构
 */
struct ngx_list_part_s {
    void             *elts;/** 指向该节点数据区的首地址*/
    ngx_uint_t        nelts;/** 该节点数据区实际存放的元素个数*/
    ngx_list_part_t  *next;/** 指向链表的下一个节点*/
};

/**
 * 链表表头的结构
 */
typedef struct {
    ngx_list_part_t  *last;
    ngx_list_part_t   part;
    size_t            size;
    ngx_uint_t        nalloc;
    ngx_pool_t       *pool;
} ngx_list_t;


/**
 * Nginx 链表的操作只有两个：
 * 创建链表和添加元素，
 * 由于链表的内存分配是基于内存池的，所有的内存销毁是有内存池进行，即链表没有销毁操作
 */

/**
 * 创建新的链表时，首先分配链表表头，再对该链表进行初始化，在初始化过程中分配头节点数据区内存
 */

/**
 * 创建链表
 * @param pool
 * @param n
 * @param size
 * @return
 */
ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

/**
 * 初始化链表
 */
static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /**
     * 分配节点数据内存，并返回该节点数据区的首地址
     */
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    /**
     * 初始化节点成员
     */
    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/**
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


/**
 * 添加元素
 *
 * 添加元素到链表时，都是从最后一个节点开始，首先判断最后一个节点的数据区是否由内存存放新增的元素，
 *
 * 若足以存放该新元素，则返回存储新元素的内存位置，
 * 若没有足够的内存存储新增加的元素，则分配一个新的节点，再把该新的节点连接到现有的链表中，并返回存储新元素内存的位置
 *
 * 注意添加的元素可以时整数，也可以时一个结构
 */
void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
