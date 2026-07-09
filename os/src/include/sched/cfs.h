/*
 * cfs.h
 * (C) 2026 Guo Fu
 */
#ifndef _CFS_H_
#define _CFS_H_

/*
 * 这里的实现方式主要是采用以下的设计思路：
 * 1、采用了三层结构的方式，即slab->entity->pcb的组织方式；slab是对entity的申请，entity是CFS的基本运算单元。
 *  对于entity，CFS的vruntime存在于entity中，而slab对象存在的只是entity的物理地址。
 *  对于pcb，它的物理地址会被entity中的指针所指。
 *  对于以上的操作，均需要向mm申请必要的空间。
 * 优先级值越大，优先级越低
 */

#include <mm/slab.h>
#include <sys/fpu.h>
#include <lib/rbtree.h>

// CFS权值映射表
const int sched_prio_to_weight[40];

void cfs_sched_kmem_cache_init();

phys_addr_t cfs_sched_kmem_cache;

#endif
