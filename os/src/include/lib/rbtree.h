#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <datatype.h>
#include <mm/slab.h>

enum rbtree_node_color
{
    red = 0,
    black
};

enum rbtree_node_type
{
    rbtree_root = 0,
    rbtree_node
};

typedef struct rbtree_node
{
    float val;
    enum rbtree_node_color color;
    struct rbtree_node *p_node;
    struct rbtree_node *l_node;
    struct rbtree_node *r_node;
} rbtree_node_t;

rbtree_node_t *virtual_node;

void rbtree_virtual_node_init();
phys_addr_t create_rb_root_node(phys_addr_t kmem_cache_item, enum rbtree_node_type type, float val);
rbtree_node_t *insert_rbtree(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node);
rbtree_node_t *insert_rbtree_fixup(rbtree_node_t *rbtree_node);
rbtree_node_t *insert_rb_root_node(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node);
void remove_rb_root_node(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node);
rbtree_node_t *get_left_node(rbtree_node_t *rbtree_root);
rbtree_node_t *get_right_node(rbtree_node_t *rbtree_root);
void print_all_rbtree_node(rbtree_node_t *rbtree_root);

#endif
