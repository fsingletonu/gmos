#include <init/pic.h>

void pic_init()
{
    out_byte_x64(HIMR, 0xff);
    out_byte_x64(SIMR, 0xff);

    out_byte_x64(HICW1, 0x11);
    out_byte_x64(HICW2, 0x20);
    out_byte_x64(HICW3, 0x04);
    out_byte_x64(HICW4, 0x01);

    out_byte_x64(SICW1, 0x11);
    out_byte_x64(SICW2, 0x28);
    out_byte_x64(SICW3, 0x02);
    out_byte_x64(SICW4, 0x01);

    out_byte_x64(HIMR, 0xfd);
    out_byte_x64(SIMR, 0xff);

    idt_register_eoi(pic_eoi);
}

void pic_eoi(size_t irq)
{
    out_byte_x64(HPCP, 0x20);
    out_byte_x64(SPCP, 0x20);
}

