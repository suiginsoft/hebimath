# hebimath - arbitrary precision arithmetic library

# See LICENSE file for copyright and license details

# uint64_t
# hebi_pdivremru64_2x1__(
#       uint64_t *q,
#       const uint64_t *a,
#       size_t n,
#       int bits,
#       uint64_t d,
#       uint64_t v);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_X86_64
MVFUNC_BEGIN pdivremru64_2x1, x86_64, @private, @explicit

    push        %rbx
    push        %rbp
    sub         $8, %rsi
    mov         %rdx, %rbx
    push        %r12
    mov         (%rsi,%rbx,8), %rax
    mov         %ecx, %edx
    xor         %r11, %r11
    mov         %rax, %r12
    neg         %ecx
    jz          8f

    # Setup for division with shifting

    dec         %rbx
    shr         %cl, %rax
    mov         %edx, %ecx
    jnz         1f
    mov         %r12, %r10
    shl         %cl, %r10
    cmp         %r8, %r10
    jae         1f
    mov         %r12, %rax
    mov         (%rsi,%rbx,8), %rbp
    shld        %cl, %rbp, %rax
    jmp         2f

    # Division loop with shifting

    .p2align 4
1:  mov         (%rsi,%rbx,8), %rbp
    lea         1(%rax), %r10
    shld        %cl, %rbp, %r12
    mul         %r9
    add         %r12, %rax
    adc         %r10, %rdx
    mov         %rax, %r10
    mov         %rdx, %r11
    imul        %r8, %rdx
    mov         %r8, %rax
    sub         %rdx, %r12
    add         %r12, %rax
    cmp         %r10, %r12
    cmovb       %r12, %rax
    adc         $-1, %r11
    cmp         %r8, %rax
    jae         4f
2:  mov         %r11, (%rdi,%rbx,8)
    mov         %rbp, %r12
    dec         %rbx
    jnz         1b

    # Division of final limb

    lea         1(%rax), %r10
    shl         %cl, %r12
    mul         %r9
    add         %r12, %rax
    adc         %r10, %rdx
    mov         %rax, %r10
    mov         %rdx, %r11
    imul        %r8, %rdx
    mov         %r8, %rax
    sub         %rdx, %r12
    add         %r12, %rax
    cmp         %r10, %r12
    cmovb       %r12, %rax
    adc         $-1, %r11
    cmp         %r8, %rax
    jae         5f
3:  mov         %r11, (%rdi,%rbx,8)
    shr         %cl, %rax

    # All done remainder is in %rax

    pop         %r12
    pop         %rbp
    pop         %rbx
    ret

    # Remainders were too large, adjust and continue

    .p2align 4,,7
4:  inc         %r11
    sub         %r8, %rax
    jmp         2b

    .p2align 4,,7
5:  inc         %r11
    sub         %r8, %rax
    jmp         3b

    # Division loop without shifting

    .p2align 4
6:  mov         (%rsi,%rbx,8), %r12
    lea         1(%rax), %r10
    mul         %r9
    add         %r12, %rax
    adc         %r10, %rdx
    mov         %rax, %r10
    mov         %rdx, %r11
    imul        %r8, %rdx
    mov         %r8, %rax
    sub         %rdx, %r12
    add         %r12, %rax
    cmp         %r10, %r12
    cmovb       %r12, %rax
    adc         $-1, %r11
    cmp         %r8, %rax
    jae         9f
7:  mov         %r11, (%rdi,%rbx,8)
    dec         %rbx
    jnz         6b

    # All done remainder is in %rax

    pop         %r12
    pop         %rbp
    pop         %rbx
    ret

    # Entry point if 'bits 'is zero, start division without shifting

    .p2align 4,,7
8:  sub         $8, %rdi
    cmp         %r8, %rax
    jb          7b

    # Remainder was too large, adjust and continue

    .p2align 4,,7
9:  inc         %r11
    sub         %r8, %rax
    jmp         7b

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_PTR pdivremru64_2x1, x86_64
.endif
