#include <gmos.h>

char *string = "Hello World...\n";

// 只有这一个静态全局瞬时RSDP 这也是热插拔的入口
RSDPDescriptor_t *g_rdsp;
mmnode_t *g_first_node;

// 早期内存分配器信息 用物理内存
memblock_head_t *memory_new_use_memblock;

// 系统虚拟终端缓冲区基地址
phys_addr system_vconsole;

// FAT32文件系统全局缓存
fat32_global_fat_t global_fat;

// BSP IDT指针
void *g_idt_ptr;

per_cpu_t cpu;

void kernel_init(BootInfoHead *Info)
{
    rsdp_init(Info);
    memblock_init(Info);
    mm_init(Info);
    mmzone_init(g_first_node);
    // kmem_cache_head_t *m = create_kmem_cache(g_first_node, 8, 8);
    // reload_gdt(g_first_node, get_core_count_init());
    // local_tss_init(g_first_node, 0);
    // idt_init((mmzone_t *)g_first_node->mmzone[2], (void **)&g_idt_ptr);
    // idt_register_interrupt_trap(g_idt_ptr, 0x21, (phys_addr)&keyboard_handler, GATE_TYPE_INTERRUPT);
    // pic_init();
    // VGAVideoInit(Info->VGAInfo.FrameBufferBase, Info->VGAInfo.FrameBufferSize);
    // SystemGlobalConsoleTextBuffer((mmzone_t *)g_first_node->mmzone[2], &system_vconsole);

    if (get_fat32_status((mmzone_t *)g_first_node->mmzone[2]))
        fat32_mount((mmzone_t *)g_first_node->mmzone[2]);
    fat32_global_fat_init((mmzone_t *)g_first_node->mmzone[2]);
    get_free_cluster(global_fat);

    asm volatile("sti");
    DEBUG;
    while (TRUE)
    {
        DEBUG;
    }
}