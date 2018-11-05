.intel_syntax noprefix

.global asm_mod

asm_mod:
    mov r9, 134217728
    mov rax, 2
l1:
    mul rax
    div rsi

    test r9, rdi
    jz l2

    add rdx, rdx
    sub rdx, rsi
    jnc l2
    add rdx, rsi
    clc
l2:
    rcr r9
    mov rax, rdx
    test r9, r9
    jnz  l1

    ret
