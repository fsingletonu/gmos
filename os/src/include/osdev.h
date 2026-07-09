#ifndef _OSDEV_H_
#define _OSDEV_H_

#include <datatype.h>

#include <blk_drv/dma.h>

#include <chr_drv/console.h>
#include <chr_drv/io.h>
#include <chr_drv/keyboard.h>

#include <fs/ext/super_block.h>

#include <init/acpi.h>
#include <init/ards.h>
#include <init/desc.h>
#include <init/handler.h>
#include <init/i386.h>
#include <init/ide.h>
#include <init/interrupt.h>
#include <init/panic.h>
#include <init/pci.h>
#include <init/trap.h>

#include <lib/bitmap.h>
#include <lib/math.h>
#include <lib/rbtree.h>
#include <lib/string.h>

#include <mm/alloc_pages.h>
#include <mm/memory.h>
#include <mm/mmzone.h>
#include <mm/numa.h>
#include <mm/page.h>
#include <mm/slab.h>

#include <sched/cfs.h>
#include <sched/fcfs.h>
#include <sched/task.h>

#include <sys/fpu.h>

void kernel_init(void);

#endif
