# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void hebi_psqr(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN psqr, _x86_64, 16

    test        %rdx, %rdx
    jz          5f

.align 8
    push        %rbx
    push        %rbp

    xor         %rbx, %rbx
    mov         (%rsi), %r9
    xor         %rcx, %rcx
    lea         (,%rdx,4), %rbp
    xor         %r10, %r10

.align 16
1:  mov         (%rsi,%rbx,8), %rax
    add         $2, %rbx
    mul         %r9
    xor         %r11, %r11
    add         %rax, %r10
    mov         -8(%rsi,%rbx,8), %rax
    adc         %rdx, %r11
    mov         %r10, -16(%rdi,%rbx,8)
    mul         %r9
    xor         %r10, %r10
    add         %rax, %r11
    mov         %r11, -8(%rdi,%rbx,8)
    adc         %rdx, %r10
    cmp         %rbx, %rbp
    jne         1b
    inc         %rcx
    mov         %r10, (%rdi,%rbx,8)

.align 8
2:  mov         (%rsi,%rcx,8), %r9
    add         $8, %rdi
    xor         %rbx, %rbx
    xor         %r10, %r10

.align 16
3:  mov         (%rsi,%rbx,8), %rax
    add         $2, %rbx
    mul         %r9
    mov         -16(%rdi,%rbx,8), %r8
    xor         %r11, %r11
    add         %rax, %r10
    adc         %rdx, %r11
    add         %r8, %r10
    mov         -8(%rsi,%rbx,8), %rax
    adc         $0, %r11
    mov         %r10, -16(%rdi,%rbx,8)
    mul         %r9
    mov         -8(%rdi,%rbx,8), %r8
    xor         %r10, %r10
    add         %rax, %r11
    adc         %rdx, %r10
    add         %r8, %r11
    mov         %r11, -8(%rdi,%rbx,8)
    adc         $0, %r10
    cmp         %rbx, %rbp
    jne         3b
    inc         %rcx
    mov         %r10, (%rdi,%rbx,8)
    cmp         %rcx, %rbp
    jne         2b

4:  pop         %rbp
    pop         %rbx
5:  retq

MVFUNC_END psqr, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR psqr, _x86_64

.endif
