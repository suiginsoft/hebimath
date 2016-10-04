# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void hebi_pmul(hebi_packet *r, const hebi_packet *a, const hebi_packet *b, size_t an, size_t bn);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN pmul, x86_64

    test        %r8, %r8
    jnz         1f
    mov         %rcx, %rsi
    MVFUNC_JMP  pzero, %r10

.align 8
1:  push        %rbx
    push        %rbp
    push        %r12
    push        %r13
    push        %r14

    mov         %rdx, %rbp
    mov         %rdi, %r12
    shl         %r8
    mov         %rdx, %r13
    mov         %r8, %r14
    mov         (%rsi), %r9
    shl         $2, %rcx
    xor         %r10, %r10

.align 16
2:  mov         (%rbp), %rax
    mul         %r9
    xor         %r11, %r11
    add         $16, %rbp
    add         %rax, %r10
    adc         %rdx, %r11
    mov         %r10, (%rdi)
    mov         -8(%rbp), %rax
    mul         %r9
    xor         %r10, %r10
    add         $16, %rdi
    add         %rax, %r11
    adc         %rdx, %r10
    mov         %r11, -8(%rdi)
    dec         %r8
    jnz         2b

    mov         %r10, (%rdi)
    dec         %rcx
    jz          5f

.align 8
3:  add         $8, %r12
    add         $8, %rsi
    mov         %r12, %rdi
    mov         %r13, %rbp
    mov         %r14, %r8
    mov         (%rsi), %r9
    xor         %r10, %r10

.align 16
4:  mov         (%rbp), %rax
    mov         (%rdi), %rbx
    mul         %r9
    xor         %r11, %r11
    add         $16, %rbp
    add         %rax, %r10
    adc         %rdx, %r11
    add         %rbx, %r10
    adc         $0, %r11
    mov         -8(%rbp), %rax
    mov         %r10, (%rdi)
    mov         8(%rdi), %rbx
    mul         %r9
    xor         %r10, %r10
    add         $16, %rdi
    add         %rax, %r11
    adc         %rdx, %r10
    add         %rbx, %r11
    adc         $0, %r10
    mov         %r11, -8(%rdi)
    dec         %r8
    jnz         4b
    mov         %r10, (%rdi)
    dec         %rcx
    jnz         3b

5:  pop         %r14
    pop         %r13
    pop         %r12
    pop         %rbp
    pop         %rbx
    retq

MVFUNC_END

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_PTR pmul, x86_64
.endif
