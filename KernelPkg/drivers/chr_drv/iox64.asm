[bits 64]
default rel

global in_byte_x64
global in_word_x64
global in_dword_x64
global out_byte_x64
global out_word_x64
global out_dword_x64

in_byte_x64:
    mov dx,di
    in al,dx
    jmp $+2
    jmp $+2
    jmp $+2
    ret

in_word_x64:
    mov dx,di
    in ax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    ret

in_dword_x64:
    mov dx,di
    in eax,dx
    jmp $+2
    jmp $+2
    jmp $+2
    ret

out_byte_x64:
    mov dx,di
    mov al,sil
    out dx,al
    jmp $+2
    jmp $+2
    jmp $+2
    ret

out_word_x64:
    mov dx,di
    mov ax,si
    out dx,ax
    jmp $+2
    jmp $+2
    jmp $+2
    ret

out_dword_x64:
    mov dx,di
    mov eax,esi
    out dx,eax
    jmp $+2
    jmp $+2
    jmp $+2
    ret