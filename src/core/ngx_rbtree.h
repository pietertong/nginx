
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
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


typedef struct ngx_rbtree_s  ngx_rbtree_t;

typedef void (*ngx_rbtree_insert_pt) (ngx_rbtree_node_t *root,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);

/**
 * 红黑树结构
 */
struct ngx_rbtree_s {
    ngx_rbtree_node_t     *root; /** 指向树的根结点*/
    ngx_rbtree_node_t     *sentinel; /** 指向树的叶子节点 NIL*/
    ngx_rbtree_insert_pt   insert; /** 添加元素节点的函数指针，解决具有相同键值，但不同颜色节点的冲突问题*/
    /**
     * 该函数指针决定新节点的行为是新增的还是替换原始的某个节点
     */
};

/**
 * 初始化红黑树，即为空的红黑树
 *
 * tree 是指向红黑树的指针
 *
 * s 是红黑树的一个NIL 节点
 *
 * i 表示函数指针，决定节点是新增还是替换
 *
 */
#define ngx_rbtree_init(tree, s, i)                                           \
    ngx_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i


void ngx_rbtree_insert(ngx_rbtree_t *tree, ngx_rbtree_node_t *node);
void ngx_rbtree_delete(ngx_rbtree_t *tree, ngx_rbtree_node_t *node);
void ngx_rbtree_insert_value(ngx_rbtree_node_t *root, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel);
void ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *root,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
ngx_rbtree_node_t *ngx_rbtree_next(ngx_rbtree_t *tree,
    ngx_rbtree_node_t *node);

/** 给节点着色，1 表示红色， 0 表示黑色*/
#define ngx_rbt_red(node)               ((node)->color = 1)
#define ngx_rbt_black(node)             ((node)->color = 0)
#define ngx_rbt_is_red(node)            ((node)->color)
#define ngx_rbt_is_black(node)          (!ngx_rbt_is_red(node))
#define ngx_rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */

#define ngx_rbtree_sentinel_init(node)  ngx_rbt_black(node)


static ngx_inline ngx_rbtree_node_t *
ngx_rbtree_min(ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel)
{
    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}


#endif /* _NGX_RBTREE_H_INCLUDED_ */
