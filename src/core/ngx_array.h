
/*
 * 在 Nginx 数组中，内存分配是基于内存池的，并不是固定不变的，
 * 也不是需要多少内存就申请多少，若当前内存不足以存储所需元素时，
 * 按照当前数组的两倍内存大小进行申请，这样做减少内存分配的次数，提高效率。
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 动态数组的数据结构如下
 */
typedef struct {
    void        *elts;   // 执行数组数据区域的首地址
    ngx_uint_t   nelts;  // 数组实际数据的个数
    size_t       size;   // 单个元素所占据的字节大小
    ngx_uint_t   nalloc; // 数组容量
    ngx_pool_t  *pool;   // 数组对象所在的内存池
} ngx_array_t;

//创建新的动态数组
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);
//销毁数组对象，内存被内存池回收
void ngx_array_destroy(ngx_array_t *a);
// 在现有的数组中增加一个新的元素
void *ngx_array_push(ngx_array_t *a);
//在现有的数组中增加N个新的元素
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);

/**
 * Ngx的数组初始化，
 * @param array
 * @param pool
 * @param n
 * @param size
 * @return
 * 当一个数组对象被分配到堆上，且调用ngx_array_destory之后，若想在重新使用，则调用改函数
 */
static ngx_inline ngx_int_t ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */
    // netls要先于 etls 初始化
    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;
    // 分配数组数据域所需要的内存，有内存首地址
    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }
    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
