
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 创建新的动态数组
 * @param p
 * @param n
 * @param size
 * @return
 * 创建数组的操作实现如下，首先分配数组头，然后分配数组数据区，
 * 两次分配均在传入的内存池(pool指向的内存池)中进行。
 * 然后简单初始化数组头并返回数组头的起始位置。
 */
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;
    // 分配动态数组头部
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }
    /**分配容量为N的动态数组数据区，并将其初始化*/
    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }
    return a;
}

/**
 * 销毁动态数组
 *
 * @param a
 */
void ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;
    p = a->pool;
    // 先消耗数组数据区
    // 移动内存池的last 指针，是否数组所有元素占据的内存
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }
    // 销毁数组头
    // 释放数组首指针所占据的内存
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}


/**
 * 添加元素的操作
 * 数组添加元素的操作有两个，
 * ngx_array_push(),ngx_array_push_n()
 * 分别是添加一个和多个元素，实际的添加操作并不算在这两个函数中完成的，
 * 只是在这两个函数中申请元素所需的内存空间，并返回指向该内存空间的首地址，在利用指针赋值的形式添加元素
 */

/**
 * 数组添加一个元素
 * @param a
 * @return
 */
void * ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
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
            // 若当前内存池的内存空间至少可容纳一个元素大小
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */
            /**
             * 新的数组内存为当前数组大小的2倍
             */
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }
            /**
            * 首先把现有的数组的所有元素复制到新的数组中
            */
            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}

/**
 * 数组增加N个元素
 * @param a
 * @param n
 * @return
 */
void * ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {
        // 数组将要满了
        /* the array is full */
        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last && p->d.last + size <= p->d.end)
        {
            //还有空余的地址分配，则直接分配新的内存就行了
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += size;
            a->nalloc += n;

        } else {
            /* allocate a new array */
            //申请的数量大于等于原来的数组容量，则是 2 倍的 申请的数量，否则是 数组容量的2倍
            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);
            new = ngx_palloc(p, nalloc * a->size); //申请数据地址内存
            if (new == NULL) {
                return NULL;
            }
            //把旧的数据拷贝到新的地址内存中
            ngx_memcpy(new, a->elts, a->nelts * a->size); // 是怎样的拷贝？？零拷贝还是深拷贝，还是浅拷贝？
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}
