[bits 64]

section .text._start
extern kernel_init

global _start

_start:
    cli
    lea rsp, [kernel_stack_top]
    mov rdi, rbx
    call kernel_init
    jmp $

; 系统临时栈
section .stack
align 4096
kernel_stack:
    resb 4096
kernel_stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits