
/*
 *
 * ngx_conf_module
 * ngx_event_timer_rbtree
 * Ngx_http_file_cache
 * Ngx_http_geo_module
 * Ngx_http_limit_conn_module
 * Ngx_http_limit_req_module
 * Ngx_http_lua_shdict:ngx.shard.DICT   LRU 链表性质
 * resolver    ngx_resolver_t
 * Ngx_stream_geo_module
 * Ngx_stream_limit_conn_module
 *
 * https://zhuanlan.zhihu.com/p/336580412
 * https://pansong291.gitee.io/web/html/other/RedBlackTree.html
 * 红黑树的在线演示
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc .
 *
 * 自平衡二叉查找树
 * 高度差不会超过 2倍 log(n)
 * 红黑树的高度趋近 logn ; 查找，删除，插入的时间复杂度是 O(log n)
 * 有N个节点的红黑树，他的高度 最多是  2（log(n+1))
 * 遍历复杂度O(n)
 * 有N个节点的红黑树，他的高度 最多是  2（log(n+1))
 *
 *
 * 特性如下：
 *
 * 所有节点非红即黑，红色节点的子节点一定是黑色节点
 * 根节点是黑色
 * 页节点是不存储数据的黑色空节点
 * 任意相邻的两个节点不能同时为红色即不能有连续的红色
 * 任意节点到叶子节点路径中有相同数量的黑色节点

 *
 *
 * 疑问？？
 * sentinel 这个参数是干嘛的？？？
 */


#ifndef _NGX_RBTREE_H_INCLUDED_
#define _NGX_RBTREE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef ngx_uint_t  ngx_rbtree_key_t;
typedef ngx_int_t   ngx_rbtree_key_int_t;

/**
 * 红黑树节点结构
 */
typedef struct ngx_rbtree_node_s  ngx_rbtree_node_t;

struct ngx_rbtree_node_s {
    ngx_rbtree_key_t       key;/** 节点的键值*/
    ngx_rbtree_node_t     *left; /** 节点的左孩子*/
    ngx_rbtree_node_t     *right;/** 节点的有孩子*/
    ngx_rbtree_node_t     *parent;/** 节点的父亲*/
    u_char                 color; /** 节点的颜色*/
    u_char                 data; /** 节点的数据*/
};

// _s 是结构体， _t 是类型
typedef struct ngx_rbtree_s  ngx_rbtree_t;

// 插入函数指针， 可以调用 ngx_rbtree_insert_value 作用是找到合适的插入点
typedef void (*ngx_rbtree_insert_pt) (ngx_rbtree_node_t *root,ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);

/**
 * 红黑树结构
 */
struct ngx_rbtree_s {
    ngx_rbtree_node_t     *root; /** 指向树的根结点*/
    ngx_rbtree_node_t     *sentinel; /** 哨兵节点指针，指向树的叶子节点 NIL*/
    ngx_rbtree_insert_pt   insert;
    /** 添加元素节点的函数指针，解决具有相同键值，但不同颜色节点的冲突问题*/
    /**
     * 该函数指针决定新节点的行为是新增的还是替换原始的某个节点
     */
};

/**
 * 将函数指针变量作为结构体成员变量以达成可以呗结构体当做类来使用，（既有成员也有成员方法）的效果
 * 这种手法在Nginx的源代码中相当普遍，关于函数，NGINX还有一种更神奇的手段-宏；
 *
 * 初始化红黑树，即为空的红黑树
 * tree 是指向红黑树的指针
 * s 是红黑树的一个NIL 节点 哨兵节点 表示无值，任何变量在没有被赋值之前的值都为nil。
 * i 表示函数指针，决定节点是新增还是替换
 */
#define ngx_rbtree_init(tree, s, i)                                           \
    ngx_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i // 这里的insert函数指针的赋值实现了多态

// 插入
void ngx_rbtree_insert(ngx_rbtree_t *tree, ngx_rbtree_node_t *node);

//删除
void ngx_rbtree_delete(ngx_rbtree_t *tree, ngx_rbtree_node_t *node);

//插入函数
void ngx_rbtree_insert_value(ngx_rbtree_node_t *root, ngx_rbtree_node_t *node,ngx_rbtree_node_t *sentinel);

//ngx_rbtree_insert_timer_value函数跟ngx_rbtree_insert_value函数唯一区别就是判断大小时，采用了两个值相减，避免溢出
void ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *root,ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
ngx_rbtree_node_t *ngx_rbtree_next(ngx_rbtree_t *tree,ngx_rbtree_node_t *node);

/** 给节点着色，1 表示红色， 0 表示黑色*/
#define ngx_rbt_red(node)               ((node)->color = 1)
#define ngx_rbt_black(node)             ((node)->color = 0)
#define ngx_rbt_is_red(node)            ((node)->color)
#define ngx_rbt_is_black(node)          (!ngx_rbt_is_red(node)) // because red-black tree node is red or black
#define ngx_rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */
#define ngx_rbtree_sentinel_init(node)  ngx_rbt_black(node)

/**
 * 获取红黑树键值的最小的节点
 * @param node
 * @param sentinel
 * @return
 */
static ngx_inline ngx_rbtree_node_t *ngx_rbtree_min(ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel)
{
    // 寻找 nginx下的最小值，在.c中，你看到的调用其实就是 找到右子树的最小值用来作为
    // 后继节点
    while (node->left != sentinel) {
        // 在不溢出的情况下执行厦门的语句
        node = node->left;
    }

    return node;
}

//ngx_inline 是一个宏，实际值就是关键字 inline,这个内联函数非常好懂

#endif /* _NGX_RBTREE_H_INCLUDED_ */
