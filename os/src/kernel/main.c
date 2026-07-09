#include <osdev.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    console_init();
    rsdp_init();
    bump_allocator();
    ide_init();
    fpu_init();
    cfs_sched_kmem_cache_init();
    rbtree_virtual_node_init();
    rbtree_node_t *root = (rbtree_node_t *)create_rb_root_node(cfs_sched_kmem_cache, rbtree_root, 10);
    // rbtree_node_t *node2 = (rbtree_node_t *)create_rb_root_node(cfs_sched_kmem_cache, rbtree_node, 6);
    // rbtree_node_t *node3 = (rbtree_node_t *)create_rb_root_node(cfs_sched_kmem_cache, rbtree_node, 8);
    // root = insert_rb_root_node(root, node2);
    // root = insert_rb_root_node(root, node3);
    float num[10] = {5, 15, 3, 7, 12, 18, 1, 4, 6, 8};
    for (size_t i = 0; i < 10; i++)
    {
        rbtree_node_t *node = (rbtree_node_t *)create_rb_root_node(cfs_sched_kmem_cache, rbtree_node, num[i]);
        root = insert_rb_root_node(root, node);
    }
    while (true)
    {
    }
}