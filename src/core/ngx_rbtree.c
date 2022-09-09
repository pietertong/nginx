
/*
 * https://blog.csdn.net/dearQiHao/article/details/102878306
 * NGINX下的红黑树源码详解
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
 * 插入节点 ： 插入步骤
 * 1、首先按照二叉查找树的插入操作插入新节点
 * 2、然后把新节点着色为红色（避免破坏红黑树的新增5）
 * 3、为维持红黑树的性质，调整红黑树的节点（着色并旋转）
 * @param tree
 * @param node
 */
void ngx_rbtree_insert(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  **root, *temp, *sentinel; // 这三个都有独立的结构

    /**
     * 一个二叉树的插入
     * a binary tree insert
     */
    root = &tree->root; //树根指针赋给了root
    sentinel = tree->sentinel; // 哨兵指针赋给了哨兵指针
    /**
     * 若红黑树为空，则比较简单，把新节点作为根节点，
     * 并初始化该节点事情满足红黑树的性质
     */
    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        // 根节点必须是黑色
        ngx_rbt_black(node);
        *root = node;
        return;
    }
    /** 若红黑树不为空，则按照二叉查找树的插入进行操作 ，
     * 树初始化时给了insert指针一个函数地址
     * 该操作由函数指针提供
     * 插入操作(通过这个函数指针的调用就能找到我们的插入点了)
     * */
    tree->insert(*root, node, sentinel);
    /* re-balance tree */
    /** 调整红黑树，使其满足特性
     * 其实这里只破坏了性质4： 若一个节点为红色，则孩子节点都是黑色
     * 若破坏了性质4，则新的节点 node及其父亲节点 node-parent都为红色
     * */
    while (node != *root && ngx_rbt_is_red(node->parent)) {
        /** 若node的父亲节点是其祖父亲节点的左孩子*/
        if (node->parent == node->parent->parent->left) {
            /** temp 节点为node的叔叔节点*/
            temp = node->parent->parent->right;
            /** case 1: node的叔叔节点是红色
             *  此时： node的父亲及叔叔节点都为红色
             *  解决办法：
             *  将node 的父亲及叔叔节点着色为黑色， 将 node的祖父节点着色为红色
             *  然后沿着祖父节点相似判断是否会破坏红黑树的性质
             */
            if (ngx_rbt_is_red(temp)) {
                ngx_rbt_black(node->parent);
                ngx_rbt_black(temp);
                ngx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                /** case2 : node 的叔叔节点是黑色且 node是父亲节点的右孩子
                 *  此时 以 node 父节点进行左旋转，是其 case2 变成 case3;
                 */
                if (node == node->parent->right) {
                    node = node->parent;
                    // 左旋转
                    ngx_rbtree_left_rotate(root, sentinel, node);
                }
                /** case3 : node的叔叔节点是黑色，且 node是 父亲节点的左孩子
                 * 首先 ， 将node的父亲节点着色为黑色，祖父节点着色为红色
                 * 然后 以 父亲节点进行一次右旋转
                 */
                ngx_rbt_black(node->parent);
                ngx_rbt_red(node->parent->parent);
                // 右旋转
                ngx_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        }
        else {
            /** 若node的父亲节点是其祖父节点的右孩子，*/
            temp = node->parent->parent->left;

            if (ngx_rbt_is_red(temp)) {
                ngx_rbt_black(node->parent);
                ngx_rbt_black(temp);
                ngx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    // 右旋转
                    ngx_rbtree_right_rotate(root, sentinel, node);
                }

                ngx_rbt_black(node->parent);
                ngx_rbt_red(node->parent->parent);
                // 左旋转
                ngx_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    // 根节点必须是黑色
    ngx_rbt_black(*root);
}


/**
 * 这里只是将节点插入到红黑树中，并没有判断是否满足了红黑树的性质
 * 类似于 二叉查找树的插入动作，这个函数为红黑树插入操作的函数指针
 * 插入的节点都为红色
 * @param temp
 * @param node
 * @param sentinel
 */
void ngx_rbtree_insert_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,ngx_rbtree_node_t *sentinel)
{
    // 根节点 temp ，插入节点 node,哨兵节点指针 sentinel
    ngx_rbtree_node_t  **p;
    // 无条件循环或者说是死循环，等同于while(1)但是节省了1个字符，为了找到插入点
    for ( ;; ) {
        // 三目运算符，为了找到合适的插入点
        p = (node->key < temp->key) ? &temp->left : &temp->right;
        // 在 二叉树中查找新节点合适的叶节点位置
        if (*p == sentinel) {
            break; // 跳出循环，此时就可以进行插入了
        }

        temp = *p;
    }// 当跳出这个循环的时候，那么我们就找到了合适的插入位置
    // 令新节点占据合适的哨兵位置，称为新的节点，染红，产生新的哨兵
    *p = node;
    // 插入操作
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    // 新插入的节点都为红色
    ngx_rbt_red(node);
}


void ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;
    for ( ;; ) {
        /*
         * Timer values
         * 1) are spread in small range, usually several minutes,
         *  分布范围很小，通常是几分钟
         * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
         * 每 49天 溢出一次，如果毫秒存储在32bit
         * The comparison takes into account that overflow.
         * 比较考虑到了溢出。
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
 * 删除操作
 *
 * @param tree
 * @param node
 */
void ngx_rbtree_delete(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    ngx_uint_t           red;
    ngx_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;
    /* a binary tree delete */
    root = &tree->root;
    sentinel = tree->sentinel;
    /** 下面是获取 temp 节点值， temp 保存的节点是准备替换节点的node;
     * subst 是保存要被替换的节点的后续节点
     */
     /** case1 : 若node 节点没有左孩子，（这里包含了存在或不存在右孩子的情况）*/
    if (node->left == sentinel) {
        temp = node->right;
        subst = node;
    } else if (node->right == sentinel) {
        /**case2: node 节点存在左孩子，但是不存在右孩子 */
        temp = node->left;
        subst = node;
    } else {
        /**case3: node 节点既有左孩子，又有右孩子 */
        subst = ngx_rbtree_min(node->right, sentinel); // 获取node 节点的后续节点
        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }
    /** 若被替换的节点 subst 是根节点，则temp直接替换subst称为根节点*/
    if (subst == *root) {
        *root = temp;
        ngx_rbt_black(temp);
        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;
        return;
    }
    /** red记录 subst 节点的颜色 */
    red = ngx_rbt_is_red(subst);
    /**temp 节点替换subst 节点*/
    if (subst == subst->parent->left) {
        subst->parent->left = temp;
    } else {
        subst->parent->right = temp;
    }
    /**根据subst是否为node节点进行处理*/
    if (subst == node) {
        temp->parent = subst->parent;
    } else {
        if (subst->parent == node) {
            temp->parent = subst;
        } else {
            temp->parent = subst->parent;
        }
        /**赋值node的节点属性*/
        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
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
    /**调整红黑树的性质*/
    /* a delete fixup */
    /** 根据temp节点进行处理，若temp 不是根节点且为黑色*/
    while (temp != *root && ngx_rbt_is_black(temp)) {
        /** 若temp是其父亲节点的左孩子*/
        if (temp == temp->parent->left) {
            w = temp->parent->right;
            /** caseA： temp 兄弟节点为红色
             * 解决办法：
             * 1、改变W节点既temp父亲节点的颜色
             * 2、对temp父亲节的做一次左旋转，此时 temp的兄弟节点是旋转之前w的某个子节点 该子节点颜色是黑色
             * 3、此时 ，caseA  已经转化为caseB ,caseC 或者caseD;
             * */
            if (ngx_rbt_is_red(w)) {
                ngx_rbt_black(w);
                ngx_rbt_red(temp->parent);
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }
            /** caseB : temp的兄弟节点w是黑色，且w的两个子节点都是黑色
             * 解决办法
             * 1、改变W节点的颜色
             * 2、把temp的父亲节点作为新的temp节点
             * */
            if (ngx_rbt_is_black(w->left) && ngx_rbt_is_black(w->right)) {
                ngx_rbt_red(w);
                temp = temp->parent;
            } else {
                /**caseC : temp 的兄弟节点是黑色， 且W的左孩子是红色，右孩子是黑色
                 * 解决办法：
                 * 1、将改变w及其左孩子的颜色
                 * 2、对w节点进行一次右旋转
                 * 3、此时 temp新的兄弟节点w有着一个红色右孩子的黑色节点，转为caseD;
                 * */
                if (ngx_rbt_is_black(w->right)) {
                    ngx_rbt_black(w->left);
                    ngx_rbt_red(w);
                    ngx_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }
                /**caseD : temp的兄弟节点w为黑色，且w的右孩子是红色
                 * 解决办法：
                 * 1、将W节点设置为temp父亲节点的颜色，temp父亲节点置为黑色
                 * 2、w的右孩子设置为黑色
                 * 3、对temp的父亲节点做一次左旋转
                 * 4、最后把根节点root设置为temp节点
                 * */
                ngx_rbt_copy_color(w, temp->parent);
                ngx_rbt_black(temp->parent);
                ngx_rbt_black(w->right);
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            /**这里针对的是temp节点为其父亲节点的左孩子的情况*/
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
 * 左旋转操作
 * 就是以一个节点P和她的右孩子Y做为支轴进行，让Y称为新的根，
 * P称为Y的左孩子，
 * Y的左孩子称为P的右孩子
 * @param root
 * @param sentinel
 * @param node
 */
static ngx_inline void ngx_rbtree_left_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel,ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp; // 定义临时变量
    // 获取当前节点的右节点，此时 temp为当前节点额右节点了
    temp = node->right; /*temp 为node 节点的右孩子*/
    //node的右节点设置为他原来右节点的左节点
    node->right = temp->left; /*设置node节点的右孩子为tmp的左孩子*/

    //如果右子节点的左节点不为 哨兵（nil)
    if (temp->left != sentinel) {
        //右子节点的左子节点挂在左旋节点上
        temp->left->parent = node;
    }
    // 右节点将会变成原来node的父节点
    temp->parent = node->parent;
    // 判断是不是根节点的判断
    if (node == *root) {
        *root = temp;
    } else if (node == node->parent->left) {
        // 把右节点的信息和原来node的parent 进行维护
        node->parent->left = temp;
    } else {
        node->parent->right = temp;
    }
    // 现在的node 变回原来右节点的子节点了
    temp->left = node;
    // 所以他的parent变成了temp 节点
    node->parent = temp;
}


/**
 * 右旋操作
 * @param root
 * @param sentinel
 * @param node
 */
static ngx_inline void ngx_rbtree_right_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel,ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp;
    temp = node->left;
    node->left = temp->right; // 左子节点执行原左子节点的右节点
    if (temp->right != sentinel) { // 如果左子节点的右节点不为哨兵（nil）
        temp->right->parent = node; // 左子节点的右子节点在右旋节点上
    }
    temp->parent = node->parent; // 左子节点挂在右旋节点的父节点上
    if (node == *root) {
        // 如果 右旋节点为根节点，根节点赋为左子节点
        *root = temp;
    } else if (node == node->parent->right) {
        // 如果右旋节点在右子节点，左子节点挂父节点右边
        node->parent->right = temp;
    } else {
        // 否则 左子节点挂父节点左边
        node->parent->left = temp;
    }
    // 现在node 变回他原来左子节点的子节点
    temp->right = node;
    // 所以他的parent变成了temp
    node->parent = temp;
}

/**
 * 在红黑树上找节点的后继节点
 * @param tree
 * @param node
 * @return
 */
ngx_rbtree_node_t * ngx_rbtree_next(ngx_rbtree_t *tree, ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *root, *sentinel, *parent;
    sentinel = tree->sentinel;
    if (node->right != sentinel) {
        // 在有右子树的情况下，找到 node 节点的最小值
        return ngx_rbtree_min(node->right, sentinel);
    }
    // 没有右子树，查找
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
