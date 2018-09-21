
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */


static ngx_inline void ngx_rbtree_left_rotate(ngx_rbtree_node_t **root,
    ngx_rbtree_node_t *sentinel, ngx_rbtree_node_t *node);
static ngx_inline void ngx_rbtree_right_rotate(ngx_rbtree_node_t **root,
    ngx_rbtree_node_t *sentinel, ngx_rbtree_node_t *node);

/**
 * 红黑树插入，可以在 O(lgn) 时间内完成
 *
 *
 * 插入节点步骤
 * 1- 首先按照二叉树查找树的插入操作插入新节点
 * 2- 然后把新节点着色为红色（避免破坏红黑树性质5）
 * 3-为维持红黑树的性质，调整红黑树的节点（着色并旋转）,使其满足红黑树的性质；
 */
void ngx_rbtree_insert(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    /**
     * sentinel  哨兵节点，即空节点，此节点必须是 黑色
     */
    ngx_rbtree_node_t  **root, *temp, *sentinel;

    /**
     * 一个二叉树的插入
     * a binary tree insert
     */

    root = &tree->root;/** 获取树的根节点 */
    sentinel = tree->sentinel;

    /**
     * 若红黑树为空，则比较简单，把新节点作为根节点，
     * 并初始化该节点使其满足红黑树性质
     */
    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        ngx_rbt_black(node);
        *root = node;

        return;
    }

    /**
     * 若红黑树不为空，则按照二叉查找的插入操作进行
     * 该操作由函数指针提供
     *
     * 查看 红黑色树结构体
     *
     */
    tree->insert(*root, node, sentinel);

    /**
     * 红黑树
     * re-balance tree
     *
     * 调整红黑树，使其满足性质
     * 其实这里只是破坏了性质 4 ： 若一个节点是红色，则其孩子节点都为黑色
     * 若破坏了性质 4，则新的节点 node 及其父节点 node->parent都为红色
     */

    while (node != *root && ngx_rbt_is_red(node->parent)) {

        /**
         * 若 node 的父节点是其 祖父节点的左孩子
         */
        if (node->parent == node->parent->parent->left) {
            /**
             * temp 节点为 node 的叔叔节点
             */
            temp = node->parent->parent->right;

            /**
             * case 1 : node 的叔叔节点是红色
             *
             * 此时,node 的父亲及叔叔节点都是红色；
             *
             * 解决办法：
             *
             * node 的父亲及叔叔节点着色为黑色，将node的祖父节点着色为红色；
             *
             * 然后沿着祖父节点向上判断是否会破坏红黑树的性质
             *
             */
            if (ngx_rbt_is_red(temp)) {
                ngx_rbt_black(node->parent);
                ngx_rbt_black(temp);
                ngx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {

                /**
                 * case 2:
                 *
                 * node 的叔叔节点是黑色且 node是父节点的右孩子
                 *
                 * 则此时，以 node 父亲节点进行左旋转，使 case 2 转变为 case 3;
                 */
                if (node == node->parent->right) {
                    node = node->parent;
                    ngx_rbtree_left_rotate(root, sentinel, node);
                }

                /**
                 * case 3:
                 *
                 * node 的叔叔节点是黑色 且 node 是父亲节点的左孩子
                 *
                 * 首先，将 node 的父亲节点着为黑色，祖父亲节点着为红色；
                 *
                 * 然后以祖父节点进行依次右旋转
                 *
                 */
                ngx_rbt_black(node->parent);
                ngx_rbt_red(node->parent->parent);
                ngx_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            /**
             * 若 node 的父节点是其 祖父节点的右孩子
             *
             * 这里跟 上面的情况是对称的，就不再进行讲解了
             */
            temp = node->parent->parent->left;

            if (ngx_rbt_is_red(temp)) {
                ngx_rbt_black(node->parent);
                ngx_rbt_black(temp);
                ngx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    ngx_rbtree_right_rotate(root, sentinel, node);
                }

                ngx_rbt_black(node->parent);
                ngx_rbt_red(node->parent->parent);
                ngx_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    /**
     * 根节点必须是黑色
     */
    ngx_rbt_black(*root);
}


/**
 * 这里只是将节点插入红黑树中，并没有判断是否满足红黑树的性质；
 * 类似于二叉查找树的插入操作，这个函数为 红黑树插入操作的函数指针；
 *
 * @param temp
 * @param node
 * @param sentinel     NIL【T】 空节点
 *
 *                                【 P 】
 *                     _____________|_____________
 *                    |                           |
 *                 【 Y 】(temp)                【 M 】
 *               _____|_____          ____________|________
 *              |           |        |                     |
 *     (node)【 T 】      【 T 】   【 T 】                【 T 】
 *
 *===============插入node= 变成如下结构========================================
 *
 *                                  【 P 】
 *                        _____________|_____________
 *                       |                           |
 *       (node->parent)【 Y 】(temp)                【 M 】
 *                  ______|______          ____________|________
 *                 |             |        |                     |
 *    (red)(*p)【 node 】      【 T 】   【 T 】                【 T 】
 *             ____|_____
 *            |          |
 *          【 T 】     【 T 】
 */
void
ngx_rbtree_insert_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    /**
     * 初始化 node 节点，并着色为红色
     */
    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    ngx_rbt_red(node);
}

/**
 * 这个 原理 参考  ngx_rbtree_insert_value
 * @param temp
 * @param node
 * @param sentinel
 */
void
ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;

    for ( ;; ) {

        /*
         * Timer values
         * 1) are spread in small range, usually several minutes,
         * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
         * The comparison takes into account that overflow.
         */

        /*  node->key < temp->key */

        p = ((ngx_rbtree_key_int_t) (node->key - temp->key) < 0)
            ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    ngx_rbt_red(node);
}

/**
 * 删除节点
 *
 * @param tree
 * @param node
 */

void
ngx_rbtree_delete(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    ngx_uint_t           red;
    ngx_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = &tree->root;
    sentinel = tree->sentinel;

    /**
     * 下面是获取 temp 节点值，temp保存的节点是准备替换节点 node;
     * subst 是保存要替换的节点的后继节点
     *
     * case 1: 若 node 节点没有左孩子，（这里包含了存在和不存在右孩子的情况）
     *
     */
    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        /**
         * case 2: node 节点 存在左孩子，但是不存在右孩子
         */
        temp = node->left;
        subst = node;

    } else {
        /**
         * case 3: node 节点 既没有 左孩子，又没有右孩子
         *
         * 获取 node 节点的后续节点
         */
        subst = ngx_rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    /**
     *
     * 若被替换的节点 subst 是根节点，则 temp 直接替换 subst称为 根节点
     */
    if (subst == *root) {
        *root = temp;
        /** 把 temp 节点设在 为 黑色 */
        ngx_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    /**
     * red 记录 subst 节点的颜色
     */
    red = ngx_rbt_is_red(subst);

    /**
     * temp 节点替换 subst 节点
     */
    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    /**
     * 根据 sub 是否为 node节点进行处理
     */
    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        /**
         * 复制 node 节点属性
         */
        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        /** 把 node 的颜色 赋给 subst */
        ngx_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
        return;
    }


    /**
     * 下面开始是调整 红黑树的性质
     *
     * a delete fixup
     */

    /** 根据 temp 节点*/
    while (temp != *root && ngx_rbt_is_black(temp)) {

        if (temp == temp->parent->left) {
            /**
             * 若 temp 是其父亲节点的左孩子
             *
             * w 为 temp 的兄弟节点
             */
            w = temp->parent->right;

            /**
             * case A : temp 兄弟节点为红色
             *
             * 解决办法
             * 1、改变 w 节点及 temp 父亲节点的颜色；
             * 2、对 temp 父亲节点做一次左旋转，此时
             *  temp的兄弟节点是旋转之前的 w 的某个子节点，该 子节点颜色为黑色；
             * 3、此时，case A 已经转换为 case B ,case C 或者 case D ;
             */
            if (ngx_rbt_is_red(w)) {
                ngx_rbt_black(w);
                ngx_rbt_red(temp->parent);
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            /**
             * case B: temp的兄弟节点 w 是黑色，且 w 的 两个子节点都是 黑色
             * 解决办法：
             * 1、改变 w 节点的颜色
             * 2、把 temp 的父亲节点作为 temp 节点；
             */
            if (ngx_rbt_is_black(w->left) && ngx_rbt_is_black(w->right)) {
                ngx_rbt_red(w);
                temp = temp->parent;

            } else {
                /**
                 * case C: temp 的兄弟节点是黑色，且 w 的左孩子是红色，右孩子是黑色
                 *
                 * 解决办法：
                 * 1、将改变 w 及其左孩子的颜色
                 * 2、对 w 节点进行依次右旋转
                 * 3、此时 temp 新的兄弟节点 w 有着一个红色右孩子的黑色节点，转为 case D
                 */
                if (ngx_rbt_is_black(w->right)) {
                    ngx_rbt_black(w->left);
                    ngx_rbt_red(w);
                    ngx_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                /**
                 * case D : temp 的兄弟节点 w 为黑色，且 w的右孩子是红色
                 *
                 * 解决办法：
                 * 1、 将 w 节点设置 temp 父亲节点的颜色， temp 父亲节点 设置为黑色；
                 * 2、 w 的右孩子设置为黑色；
                 * 3、 对 temp 的父亲节点左一次左旋转；
                 * 4、 最后把 根节点 root 设置为 temp 节点
                 *
                 */
                ngx_rbt_copy_color(w, temp->parent);
                ngx_rbt_black(temp->parent);
                ngx_rbt_black(w->right);
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            /**
             * 这里是针对的是 temp 节点为其父亲节点的左孩子情况
             */
            w = temp->parent->left;

            if (ngx_rbt_is_red(w)) {
                ngx_rbt_black(w);
                ngx_rbt_red(temp->parent);
                ngx_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (ngx_rbt_is_black(w->left) && ngx_rbt_is_black(w->right)) {
                ngx_rbt_red(w);
                temp = temp->parent;

            } else {
                if (ngx_rbt_is_black(w->left)) {
                    ngx_rbt_black(w->right);
                    ngx_rbt_red(w);
                    ngx_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                ngx_rbt_copy_color(w, temp->parent);
                ngx_rbt_black(temp->parent);
                ngx_rbt_black(w->left);
                ngx_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    ngx_rbt_black(temp);
}

/**
 * 左旋转操作              【   P  】                                                                                 【  P  】
 *                    _______|__________                                                                       ________|________
 *                   |                  |                                                                     |                 |
 *       （node）【  pivot  】          【 d 】                                     (temp) OR (node->parent) 【 Y 】            【 d 】
 *              _____|______                                                                         _________|_______
 *             |            |                                              (temp->left)             |                 |
 *          【 a 】       【 Y 】(node->right)OR(temp)               (temp->left->right->parent) 【 pivot  】(node)   【 c  】
 *                     _____|______                                                         ________|_______
 *                    |            |                                                       |                |
 *     (temp->left) 【 b 】      【 c 】(node->right->right)                 (node->left) 【 a 】           【 b 】(temp->left->right)
 *    OR (node->right->left)          OR (temp->right)                     (temp->left->left)                   (node->right)
 *
 * @param root 祖父节点-根结点
 * @param sentinel 空节点 NIL【T】
 * @param node 红黑树节点
 * @return
 */
static ngx_inline void
ngx_rbtree_left_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel, ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp;

    temp = node->right; /** temp 为 node 节点的 右 孩子*/
    node->right = temp->left; /** 设置 node 节点的右孩子 是 temp 的左孩子 */

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}

/**
 * 右 旋转操作
 *
 * @param root
 * @param sentinel
 * @param node
 * @return
 */
static ngx_inline void
ngx_rbtree_right_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel,
    ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}


ngx_rbtree_node_t *
ngx_rbtree_next(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *root, *sentinel, *parent;

    sentinel = tree->sentinel;

    if (node->right != sentinel) {
        return ngx_rbtree_min(node->right, sentinel);
    }

    root = tree->root;

    for ( ;; ) {
        parent = node->parent;

        if (node == root) {
            return NULL;
        }

        if (node == parent->left) {
            return parent;
        }

        node = parent;
    }
}
