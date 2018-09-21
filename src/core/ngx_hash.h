
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
/**
 * 关于哈希表结合了数组和链表的特点，使其寻址、插入以及删除操作更加方便。
 *
 * 哈希表的过程是将关键字通过某种哈希函数映射到相应的哈希表位置，即对应的哈希值所在哈希表的位置
 *
 * 但是会出现多个关键字映射相同位置的情况导致冲突问题，为了解决这种情况，
 *
 * 哈希表使用两个可选的方法： 拉链法和开放寻址法
 *
 * Nginx 的哈希表中使用开放寻址来解决冲突问题，
 *
 * 为了处理字符串，Nginx还实现了支持通配符操作的相关函数，下面对Nginx中的哈希表源码进行分析；
 */

#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/**
 * hash 散列表中元素的结构，采用键值及其所对应的值 <key,value>
 */
typedef struct {
    void             *value;/** 指向用户自定义的数据 */
    u_short           len;/** 键值key的长度 */
    u_char            name[1];/** 键值key的第一个字符，数组名 name 表示指向键值 key 首地址 */
} ngx_hash_elt_t;

/**
 * 哈希表基本结构 ngx_hash_t
 */
typedef struct {
    ngx_hash_elt_t  **buckets; /** 指向 hash散列表第一个存储元素的桶*/
    ngx_uint_t        size; /** hash 散列表的桶个数*/
} ngx_hash_t;


typedef struct {
    ngx_hash_t        hash;
    void             *value;
} ngx_hash_wildcard_t;


/**
 * 计算待添加元素的 hash 元素结构
 */
typedef struct {
    ngx_str_t         key; /** 元素关键字*/
    ngx_uint_t        key_hash;/**元素关键字key计算出的hash值*/
    void             *value;/**指向关键字key对应的值，组成hash表元素 ： 键-值 <key,value>*/
} ngx_hash_key_t;

/**
 * 哈希初始化结构 ngx_hash_init_t ,Nginx的hash 初始化结构体是 ngx_hash_init_t ，用来将其相关数据封装起来作为参数传递给 ngx_hash_init();
 */
typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;
    ngx_hash_wildcard_t  *wc_head;
    ngx_hash_wildcard_t  *wc_tail;
} ngx_hash_combined_t;


/**
 * 初始化 hash 结构
 */
typedef struct {
    ngx_hash_t       *hash; /** 指向待初始化的基本 hash结构 */
    ngx_hash_key_pt   key; /** hash 函数指针*/

    ngx_uint_t        max_size; /** hash 表中桶bucket的最大个数*/
    ngx_uint_t        bucket_size; /** 每个桶 bucket的存储空间*/

    char             *name;/** hash 结构的名词（仅在错误日志中使用）*/
    ngx_pool_t       *pool; /** 分配hash结构的内存池*/
    /**
     * 分配临时数据空间的内存池，仅 在初始化hash表前，用于分配一些临时数组
     */
    ngx_pool_t       *temp_pool;
} ngx_hash_init_t;


#define NGX_HASH_SMALL            1
#define NGX_HASH_LARGE            2

#define NGX_HASH_LARGE_ASIZE      16384
#define NGX_HASH_LARGE_HSIZE      10007

#define NGX_HASH_WILDCARD_KEY     1
#define NGX_HASH_READONLY_KEY     2


typedef struct {
    ngx_uint_t        hsize;

    ngx_pool_t       *pool;
    ngx_pool_t       *temp_pool;

    ngx_array_t       keys;
    ngx_array_t      *keys_hash;

    ngx_array_t       dns_wc_head;
    ngx_array_t      *dns_wc_head_hash;

    ngx_array_t       dns_wc_tail;
    ngx_array_t      *dns_wc_tail_hash;
} ngx_hash_keys_arrays_t;


typedef struct {
    ngx_uint_t        hash;
    ngx_str_t         key;
    ngx_str_t         value;
    u_char           *lowcase_key;
} ngx_table_elt_t;


void *ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len);
void *ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len);
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,
    u_char *name, size_t len);

/**
 * 哈希初始化结构 ngx_hash_init_t,
 * Nginx的 hash 初始化结构是 ngx_hash_init_t，用来将其相关数据封装起来作为参数传递给 ngx_hash_init()
 * @param hinit
 * @param names
 * @param nelts
 * @return
 */
ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names, ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names, ngx_uint_t nelts);


/**
 * hash 函数
 */
#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,
    void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */


/**
 * 哈希操作包括初始化函数、查找函数；
 * 其中初始化函数是 Nginx中哈希表比较重要的函数，
 * 由于Nginx的hash表是静态只读的，
 * 即不能在运行时动态添加新元素的，一切的数据结构和数据都是在配置初始化的时候就已经规划完毕
 */

/**
 *
 */