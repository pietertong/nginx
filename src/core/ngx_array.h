
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct {
    void        *elts; /*指向数组数据区域的首地址*/
    ngx_uint_t   nelts;/*数组实际数据个数*/
    size_t       size;/*单个元素所占据的字节大小*/
    ngx_uint_t   nalloc;/*数组容量*/
    ngx_pool_t  *pool;/*数组对象所在的内存池*/
} ngx_array_t;

/**
 * 创建新的动态数组
 * @param p
 * @param n
 * @param size
 * @return
 */
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);

/**
 * 销毁数组对象，内存被内存池回收
 * @param a
 */
void ngx_array_destroy(ngx_array_t *a);
/**
 * 在现有的数组中增加一个新的元素
 * @param a
 * @return
 */
void *ngx_array_push(ngx_array_t *a);
/**
 * 在现有的数组中增加n个新的元素
 * @param a
 * @param n
 * @return
 */
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);

/**
 * 当一个数组对象被分配在堆上，且调用ngx_array_destroy之后，若想重新使用，则需调用该函数
 * 若数组对象被分配在栈上，则需调用此函数
 */
static ngx_inline ngx_int_t ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */

    /**
     * 初始化数组成员，注意： nelts必须比 elts先初始化
     */
    array->nelts = 0; /*数组实际数据个数 = 0*/
    array->size = size;/*单个元素所占据的字节大小 = 0*/
    array->nalloc = n;/*占用内存大小 = n*/
    array->pool = pool;/*数组对象所在的内存池 对象*/

    /**
     * 分配数组数据域所需要的内存
     */
    array->elts = ngx_palloc(pool, n * size); /*指向数组数据区域的首地址*/
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
