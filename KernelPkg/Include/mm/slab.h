/*
 * slab机制（这个实现的更准确地说是slub）
 * 把它设计成为可以挂载的 可以使用或者不使用的
 * 这是针对小对象的 如果是大对象 直接分配页
 * 大对象的标准就定为>=2048B
 * 采用嵌入式指针
 * 都存虚拟地址
 */

#ifndef _SLAB_H_
#define _SLAB_H_

#include <datatype.h>
#include <lib/string.h>
#include <mm/zone.h>

/*
 * 这个东西有两种解释
 * 1、算出块首到第一个object的距离
 * 2、算出一个对象包含其的嵌入式指针的总大小
 */
#define ALIGN_UP(size, align) (((size) + (align) - 1) & ~((align) - 1))

typedef enum
{
    PAGE_4K = 0, // やなみあんな
    PAGE_8K = 1, // ひめみやかれん
    PAGE_16K = 2
} SLAB_BLOCK_SIZE;

/*
 * kmem_cache的入口谁申请谁维护 内核没有义务维护
 * 每次延迟初始化应该为kmem_cache或者slab块的承载量的一半
 * free_list每次为空 就直接初始化
 */

// 这个的保证就是伙伴系统 对分配了的object的管理权和buddy system一样 这个里面没必要保存结构体大小
typedef struct kmem_cache_head
{
    bool head;
    size_t pages; // 一共有多少页
    size_t all_objects;
    size_t uninit_objects;
    size_t init_per_objects; // 这个值是可变的 为的是负载均衡
    size_t stride;           // 这个字段是有用的 给下一层的初始化用
    size_t slab_obj_size;    // 规定全局
    size_t slab_obj_align;   // 规定全局
    size_t inuse;
    void *free_list;
    void *uninit_ptr;
    struct kmem_cache_head *next; // 用于扩展 一般用不上
    mmnode_t *node;
    mmzone_t *zone;
} kmem_cache_head_t;

/*
 * 被分配到的slab块要懒初始化 得不够了且slab块中还有未初始化页才可以进行懒初始化（延迟初始化）
 * 每个被分配到的页都要有slab_head 这样可以保证只要对齐就能找到对应object->slab 为释放提供便利
 *
 */
typedef struct slab_head
{
    bool head;    // slab_head本身的地址就是首页基地址
    size_t pages; // 用来记录这个slab块被分配了多少个页
    size_t all_objects;
    size_t uninit_objects;
    size_t init_per_objects;
    size_t size;
    size_t stride;
    size_t inuse;

    // 这两个指针是给slabs_full slabs_partial slabs_free用的
    struct slab_head *prev;
    struct slab_head *next;

    // 这两个都存虚拟地址 因为malloc()最后会给进程一个映射后的地址 直接加free_list - slab_base就行
    void *free_list;
    void *uninit_ptr;
    mmnode_t *node;
    mmzone_t *zone;
    void *kmem_cache;
} slab_head_t;

// 这个就是kmem_cache块中的object
typedef struct kmem_cache
{
    size_t size;   // 每个slab object的大小
    size_t align;  // 对齐要求
    size_t stride; // 对齐后的步长
    slab_head_t *slabs_full;
    slab_head_t *slabs_partial;
    slab_head_t *slabs_free;
    mmnode_t *node;
    mmzone_t *zone;
} kmem_cache_t;

typedef struct free_object
{
    kmem_cache_t header;
    struct free_object *prev;
    struct free_object *next;
} free_object_t;

/*
 * 这里 我的想法是head和kmem_cache_base合起来判断一个页的使用性质
 * 而且全部都是延迟分配
 */
kmem_cache_head_t *create_kmem_cache(mmnode_t *node, size_t slab_obj_size, size_t slab_obj_align);

bool free_kmem_cache(kmem_cache_head_t *kmem_cache);

void set_objs_per_init_kmem_cache(kmem_cache_head_t *kmem_cache_blk, size_t val);

kmem_cache_t *create_kmem_cache_object(kmem_cache_head_t *kmem_cache);

bool free_kmem_cache_object(kmem_cache_head_t *kmem_cache_blk, kmem_cache_t *kmem_cache_obj);

slab_head_t *create_slab(kmem_cache_t *kmem_cache_obj);

bool free_slab(slab_head_t *slab);

void set_objs_per_init_slab(slab_head_t *slab, size_t val);

void *create_slab_object(kmem_cache_t *kmem_cache_head);

bool free_slab_object(void *obj, size_t size);

#endif
