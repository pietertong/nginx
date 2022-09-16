
/*
 *
 * 哈希表结合了数组和链表的特点，使其 寻址 插入及删除操作更加方便，哈希表的过程是将 关键字通过
 * 某种哈希函数映射到相应的哈希表位置，即对应的哈希值所在哈希表的位置，但是会出现多个关键字映射
 * 到相同位置导致冲突的问题，为了解决这个情况，哈希表使用两个选择的方法： 拉链法、开放寻址法
 *
 *Ngx的哈希表中使用开放寻址来解决冲突问题，为了处理字符串，Nginx 还实现了支持通配符操作的相关函数
 *
 *
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HASH_H_INCLUDED_
#define _NGX_HASH_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 哈希表中关键字元素的结构 ngx_hash_elt_t ，
 * hash 散列表中元素结构，采用键值及其所对应的值 <key,val>
 * */
typedef struct {
    void             *value; // 指向用户自定义的数据
    u_short           len; /**键值key的长度*/
    u_char            name[1]; /** 键值 key的第一个字符，数组名 name 标识指向键值 key 首地址*/
} ngx_hash_elt_t;

/**哈希表基本结构 ngx_hash_t */
typedef struct {
    ngx_hash_elt_t  **buckets; // 指向 hash 散列表第一个存储元素的桶
    ngx_uint_t        size; // hash 散列表的桶个数
} ngx_hash_t;


typedef struct {
    ngx_hash_t        hash;
    void             *value;
} ngx_hash_wildcard_t;

/** 计算待添加元素的hash元素结构*/
typedef struct {
    ngx_str_t         key; /** 元素关键字*/
    ngx_uint_t        key_hash; /** 元素关键字key计算出的hash值 */
    void             *value; /** 指向关键字 key 对应的值，组成hash表元素，键-值<key,val>*/
} ngx_hash_key_t;


typedef ngx_uint_t (*ngx_hash_key_pt) (u_char *data, size_t len);


typedef struct {
    ngx_hash_t            hash;
    ngx_hash_wildcard_t  *wc_head;
    ngx_hash_wildcard_t  *wc_tail;
} ngx_hash_combined_t;

/**
 * 初始化hash结构体
 * */
typedef struct {
    ngx_hash_t       *hash; /** 指向待初始化的基本 hash结构*/
    ngx_hash_key_pt   key; /** hash 函数指针*/

    ngx_uint_t        max_size; /** hash 表中桶 bucket 的最大个数*/
    ngx_uint_t        bucket_size; /** 每个桶bucket的存储空间*/

    char             *name;/** hash 结构的名称，仅在错误日志中使用*/
    ngx_pool_t       *pool; /** 分配hash结构的内存池*/
    ngx_pool_t       *temp_pool; /** 分配临时数据空间的内存池，仅在初始hash表前，用于分配一些临时数组*/
} ngx_hash_init_t; /**初始化hash结构体*/


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
void *ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key,u_char *name, size_t len);

ngx_int_t ngx_hash_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,ngx_uint_t nelts);
ngx_int_t ngx_hash_wildcard_init(ngx_hash_init_t *hinit, ngx_hash_key_t *names,ngx_uint_t nelts);

/**定义 ngx_hash 函数*/
#define ngx_hash(key, c)   ((ngx_uint_t) key * 31 + c)
ngx_uint_t ngx_hash_key(u_char *data, size_t len);
ngx_uint_t ngx_hash_key_lc(u_char *data, size_t len);
ngx_uint_t ngx_hash_strlow(u_char *dst, u_char *src, size_t n);


ngx_int_t ngx_hash_keys_array_init(ngx_hash_keys_arrays_t *ha, ngx_uint_t type);
ngx_int_t ngx_hash_add_key(ngx_hash_keys_arrays_t *ha, ngx_str_t *key,void *value, ngx_uint_t flags);


#endif /* _NGX_HASH_H_INCLUDED_ */
