#ifndef _GMOS_H_
#define _GMOS_H_

#include <BootInfo.h>
#include <datatype.h>

#include <core/smp.h>

#include <drivers/blk_drv/disk.h>

#include <drivers/chr_drv/iox64.h>
#include <drivers/chr_drv/iox86.h>
#include <drivers/chr_drv/mmio.h>
#include <drivers/chr_drv/keyboard.h>
#include <drivers/chr_drv/VGAText.h>

#include <drivers/video/Font.h>
#include <drivers/video/TheEastIsRed.h>
#include <drivers/video/VGAVideo.h>

#include <fs/fat32/fat32.h>

#include <init/acpi.h>
#include <init/apic.h>
#include <init/idt.h>
#include <init/pci.h>
#include <init/pcie.h>
#include <init/pic.h>
#include <init/x86_isr.h>

#include <lib/math.h>
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

#include <x86_64/asm.h>
#include <x86_64/desc.h>
#include <x86_64/tss.h>

#define SYSTEMPAGETABLEDIRECTMAPPING 0xffff800000000000

#endif
