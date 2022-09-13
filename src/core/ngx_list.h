
/*
 *
 * ngx_list_t 是Nginx 封装的链表容器，链表容器内存分配是基于内存池进行的，
 * 操作方便，效率高，
 * Nginx 链表容器和普通链表类似，均有链表表头和链表节点，通过节点指针组成链表
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 *
 * 链表的操作只有两个
 * 1、创建链表
 * 2、添加元素
 * 由于链表的内存妇女病是基于内存池的，所有内存的消耗有内存池进行，及链表中没有销毁操作
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
    void             *elts;/** 指向该节点数据区域的首地址*/
    ngx_uint_t        nelts;/** 该节点数据区域实际存放的元素个数 */
    ngx_list_part_t  *next; /** 指向链表的下一个节点*/
};

/**
 * 链表表头的结构
 */
typedef struct {
    ngx_list_part_t  *last; /** 指向链表中最后一个节点*/
    ngx_list_part_t   part; /** 链表中表头包含的第一个节点*/
    size_t            size; /** 元素的字节大小*/
    ngx_uint_t        nalloc;/** 链表中每个节点所能容纳元素个数*/
    ngx_pool_t       *pool; /** 该链表节点空间的内存池对象*/
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

/**
 * 初始化链表
 * @param list
 * @param pool
 * @param n
 * @param size
 * @return
 */
static ngx_inline ngx_int_t ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /**
     * 分配节点数据区内存，并返回改节点数据区域首地址
     */
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }
    /** 初始化节点成员*/
    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;
    return NGX_OK;
}


/*
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


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
