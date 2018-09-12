
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 创建数组的操作实现如下，首先分配数组头，然后分配数组数据区
 * 两次分配军在传入的内存池（pool指向的内存池）中进行
 * 然后简单初始化数组头，并返回数组头的起始位置
 * 创建动态数组对象
 * @param p
 * @param n
 * @param size
 * @return
 */

ngx_array_t * ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

    /**
     * 分配动态数组头部
     */
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

    /**
     * 分配容量为 n的动态数组数据区，并将其初始化
     */
    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }

    return a;
}

/**
 * 销毁数组的操作实现如下，包括销毁数组数据区和数组头。销毁动作实际上就是修改内存池的last
 * 指针，即数组的内存被内存池回收，并没有调用 free()等释放内存的操作
 * 销毁数组对象，即数组所占用的内存被内存池回收
 * @param a
 */
void ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

    /**
     * 移动内存池的last指针，释放数组所有元素占据的内存
     */
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    /**
     * 释放数组首指针所占据的内存
     */
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}


/**
 * 添加元素的操作
 * 数组添加元素的操作有两个，
 * ngx_array_push(),ngx_array_push_n()
 * 分别是添加一个和多个元素，实际的添加操作并不算在这两个函数中完成的，
 * 只是在这两个函数中申请元素所需的内存空间，
 * 并返回指向该内存空间的首地址，在利用指针赋值的形式添加元素
 */

/**
 * 数组添加一个元素
 * @param a
 * @return
 */
void * ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new_a;
    size_t       size;
    ngx_pool_t  *p;

    /** 判断数组是否已经满 **/
    if (a->nelts == a->nalloc) {
        /** 若现有的数组所容纳的元素个数已满 */
        /** the array is full */

        /** 计算数组所有元素占据的内存大小*/
        size = a->size * a->nalloc;

        p = a->pool;

        if ((u_char *) a->elts + size == p->d.last && p->d.last + a->size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            /**
             * 若当前内存池的内存空间至少可以容纳一个元素大小
             */
            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */
            /**
             * 若当前内存池不足以容纳一个元素，则分配新的数组内存
             */

            /**
             * 新的数组内存为当前数组大小的2倍
             */
            new_a = ngx_palloc(p, 2 * size);
            if (new_a == NULL) {
                return NULL;
            }
            /**
             * 首先把现有的数组的所有元素复制到新的数组中
             */
            ngx_memcpy(new_a, a->elts, size);
            a->elts = new_a;
            a->nalloc *= 2;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    /** 返回指向新增元素的指针 */
    return elt;
}

/**
 * 数组增加 n 个元素
 * @param a
 * @param n
 * @return
 */
void * ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new_a;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {

        /* the array is full */

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new_a = ngx_palloc(p, nalloc * a->size);
            if (new_a == NULL) {
                return NULL;
            }

            ngx_memcpy(new_a, a->elts, a->nelts * a->size);
            a->elts = new_a;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}
