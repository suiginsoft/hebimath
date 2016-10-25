# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_psqr(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN psqr, x86_64

    push        %rbx
    xor         %ecx, %ecx
    push        %rbp
    xor         %ebx, %ebx
    mov         (%rsi), %r9
    xor         %r10, %r10
    lea         (,%rdx,2), %rbp

    .p2align 4,,15
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

    .p2align 4,,7
2:  inc         %rcx
    mov         %r10, (%rdi,%rbx,8)
    cmp         %rcx, %rbp
    je          6f
3:  mov         (%rsi,%rcx,8), %r9
    add         $8, %rdi
    xor         %r10, %r10
    test        %r9, %r9
    jz          2b
    xor         %ebx, %ebx

    .p2align 4,,15
4:  mov         (%rsi,%rbx,8), %rax
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
    jne         4b
    inc         %rcx
    mov         %r10, (%rdi,%rbx,8)
    cmp         %rcx, %rbp
    jne         3b

    .p2align 4,,7
6:  pop         %rbp
    pop         %rbx
    ret

MVFUNC_END

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_PTR psqr, x86_64
.endif
