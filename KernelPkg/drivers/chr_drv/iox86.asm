[bits 32]
global in_byte_x86
global in_word_x86
global in_dword_x86
global out_byte_x86
global out_word_x86
global out_dword_x86

in_byte_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in al,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

in_word_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in ax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

in_dword_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    in eax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_byte_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_word_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,ax
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret

out_dword_x86:
    push ebp
    mov ebp,esp
    push edx
    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,eax
    jmp $+2
    jmp $+2
    jmp $+2
    pop edx
    leave
    ret