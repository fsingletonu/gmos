#include <blk_drv/dma.h>

void master_clear_trigger()
{
    uint8_t value = 0x5a;
    asm volatile("outb %0,%1"
                 :
                 : "a"(value),
                   "i"(MASTER_DMA_CLEAR_REGISTER)
                 :);
}

void flip_flop_clear()
{
    uint8_t value = 0;
    asm volatile("outb %0,%1"
                 :
                 : "a"(value),
                   "i"(DMA_CLEAR_FLIP_FLOP_REGISTER)
                 :);
}
