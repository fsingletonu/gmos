#ifndef _GMOS_H_
#define _GMOS_H_

#include <BootInfo.h>
#include <datatype.h>

#include <drivers/chr_drv/iox64.h>
#include <drivers/chr_drv/iox86.h>

#include <drivers/video/VGAVideo.h>

#include <init/acpi.h>
#include <init/apic.h>
#include <init/pci.h>
#include <init/pcie.h>
#include <init/pic.h>

#include <lib/string.h>

#include <mm/early_alloc.h>
#include <mm/mapping.h>
#include <mm/memblock.h>
#include <mm/numa.h>
#include <mm/page.h>
#include <mm/slab.h>
#include <mm/zone.h>

#include <sched/task.h>

#include <syshd/fpu.h>

#define SYSTEMPAGETABLEDIRECTMAPPING 0xffff800000000000

#endif
