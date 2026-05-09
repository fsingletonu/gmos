#include <gmos.h>

char *string = "Hello World...\n";

// 只有这一个静态全局瞬时RSDP 这也是热插拔的入口
RSDPDescriptor_t *g_rdsp = NULL;
mmnode_t *g_first_node = NULL;

// 早期内存分配器信息 用物理内存
memblock_head_t *memory_new_use_memblock = NULL;

void kernel_init(BootInfoHead *Info)
{
    // VGAVideoInit(Info->VGAInfo.FrameBufferBase, Info->VGAInfo.FrameBufferSize);
    rsdp_init(Info);
    memblock_init(Info);
    mm_init(Info);
    mmzone_init(g_first_node);
    DEBUG;
    while (TRUE)
    {
        asm volatile("nop");
    }
}