[bits 32]

global _int_0x21
global _int_0x2e

extern kb_handler
extern ide_master_dma_handler

_int_0x21:
    pushad
    call kb_handler
    popad
    iret

_int_0x2e:
    pushad
    call ide_master_dma_handler
    popad
    iret