#include <lib/rbtree.h>

void rbtree_virtual_node_init()
{
    virtual_node->val = 19761226;
    virtual_node->color = black;
    virtual_node->p_node = nullptr;
    virtual_node->l_node = nullptr;
    virtual_node->r_node = nullptr;
}

phys_addr_t create_rb_root_node(phys_addr_t kmem_cache_item, enum rbtree_node_type type, float val)
{
    phys_addr_t rbroot_node_addr = kmem_cache_alloc((zone_t *)first_node->normal_zone, (kmem_cache_t *)kmem_cache_item, sizeof(rbtree_node_t));
    rbtree_node_t *node = (rbtree_node_t *)rbroot_node_addr;
    if (type == rbtree_root)
        node->color = black;
    else if (type == rbtree_node)
        node->color = red;
    node->p_node = virtual_node;
    node->l_node = virtual_node;
    node->r_node = virtual_node;
    node->val = val;
    return rbroot_node_addr;
}

rbtree_node_t *seek_root(rbtree_node_t *rbtree_node)
{
    rbtree_node_t *root = rbtree_node;
    while (root->p_node)
        root = root->p_node;
    return root;
}

void leftRotate(rbtree_node_t *rbtree_node)
{
    rbtree_node_t *father = rbtree_node->p_node;
    rbtree_node_t *grandfather = father->p_node;
    if (grandfather)
    {
        rbtree_node->p_node = grandfather;
        grandfather->l_node = rbtree_node;
    }
    else
        rbtree_node->p_node = nullptr;
    if (rbtree_node->l_node)
    {
        father->r_node = rbtree_node->l_node;
        rbtree_node->l_node->p_node = father;
    }
    else
        father->r_node = virtual_node;
    rbtree_node->l_node = father;
    father->p_node = rbtree_node;
}

void rightRotate(rbtree_node_t *rbtree_node)
{
    rbtree_node_t *father = rbtree_node->p_node;
    rbtree_node_t *grandfather = father->p_node;
    if (grandfather)
    {
        rbtree_node->p_node = grandfather;
        grandfather->r_node = rbtree_node;
    }
    else
        rbtree_node->p_node = nullptr;
    if (rbtree_node->r_node)
    {
        father->l_node = rbtree_node->r_node;
        rbtree_node->r_node->p_node = father;
    }
    else
        father->l_node = virtual_node;
    rbtree_node->r_node = father;
    father->p_node = rbtree_node;
}

rbtree_node_t *insert_rbtree(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node)
{
    rbtree_node_t *keep = rbtree_root;
    while (keep)
    {
        if (rbtree_node->val <= keep->val && keep->l_node)
            keep = keep->l_node;
        else if (rbtree_node->val > keep->val && keep->r_node)
            keep = keep->r_node;
        else
            break;
    }
    if (rbtree_node->val <= keep->val)
        keep->l_node = rbtree_node;
    else if (rbtree_node->val > keep->val)
        keep->r_node = rbtree_node;
    rbtree_node->p_node = keep;
    return rbtree_root;
}

rbtree_node_t *insert_rbtree_fixup(rbtree_node_t *rbtree_node)
{
    rbtree_node_t *keep = rbtree_node->p_node;
    if (rbtree_root != rbtree_node)
    {
        if (keep->color == red && rbtree_node->color == red)
        {
            rbtree_node_t *grandfather = keep->p_node;
            rbtree_node_t *tmp = grandfather->r_node;
            rbtree_node_t *uncle = virtual_node;
            if (tmp != keep)
            {
                // 父亲是左子树，叔叔是右子树
                uncle = tmp;
                if (uncle->color == black)
                {
                    if (keep->l_node == rbtree_node)
                    {
                        // LL
                        rbtree_node->p_node->color = black;
                        grandfather->color = red;
                        rightRotate(rbtree_node->p_node);
                        rbtree_node_t *new_root = seek_root(rbtree_node);
                        new_root->color = black;
                        return new_root;
                    }
                    else if (keep->r_node == rbtree_node)
                    {
                        // LR
                        leftRotate(rbtree_node);
                        rightRotate(rbtree_node);
                        rbtree_node->color = black;
                        grandfather->color = red;
                        rbtree_node_t *new_root = seek_root(rbtree_node);
                        new_root->color = black;
                        return new_root;
                    }
                }
                else if (uncle->color == red)
                {
                    grandfather->color = red;
                    keep->color = black;
                    uncle->color = black;
                    // 这里需要递归
                    insert_rbtree_fixup(grandfather);
                }
            }
            else
            {
                // 父亲是右子树，叔叔是左子树
                uncle = grandfather->l_node;
                if (uncle->color == black)
                {
                    if (keep->l_node == rbtree_node)
                    {
                        // RL
                        rightRotate(rbtree_node);
                        leftRotate(rbtree_node);
                        rbtree_node->color = black;
                        grandfather->color = red;
                        rbtree_node_t *new_root = seek_root(rbtree_node);
                        new_root->color = black;
                        return new_root;
                    }
                    else if (keep->r_node == rbtree_node)
                    {
                        // RR
                        rbtree_node->p_node->color = black;
                        grandfather->color = red;
                        leftRotate(rbtree_node->p_node);
                        rbtree_node_t *new_root = seek_root(rbtree_node);
                        new_root->color = black;
                        return new_root;
                    }
                }
                else if (uncle->color == red)
                {
                    grandfather->color = red;
                    keep->color = black;
                    uncle->color = black;
                    // 这里需要递归
                    insert_rbtree_fixup(grandfather);
                }
            }
        }
    }
    rbtree_node_t *new_root = seek_root(rbtree_node);
    new_root->color = black;
    return new_root;
}

rbtree_node_t *insert_rb_root_node(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node)
{
    insert_rbtree(rbtree_root, rbtree_node);
    insert_rbtree_fixup(rbtree_node);
    return seek_root(rbtree_node);
}

void remove_rb_root_node(rbtree_node_t *rbtree_root, rbtree_node_t *rbtree_node)
{
}

rbtree_node_t *get_left_node(rbtree_node_t *rbtree_root)
{
}

rbtree_node_t *get_right_node(rbtree_node_t *rbtree_root)
{
}

void print_all_rbtree_node(rbtree_node_t *rbtree_root)
{
}