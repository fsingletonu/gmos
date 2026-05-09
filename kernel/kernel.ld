ENTRY(_start)

SECTIONS
{
    . = 0xFFFFFFFF80000000;

    /* 强制 _start 在 0xFFFFFFFF80000000 */
    .text._start : {
        *(.text._start)
    }

    /* 其余内核代码 */
    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
