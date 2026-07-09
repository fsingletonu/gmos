[bits 32]

global _lidt
global _invlpg
global _fninit

_lidt:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    lidt [edx]
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

_invlpg:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    invlpg [edx]
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret