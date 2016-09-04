# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_pmulu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN pmulu, _x86_64, 16

    xor         %r8, %r8
    mov         %rdx, %r10
    shl         $1, %rcx
    jz          2f

.align 16
1:  mov         (%rsi), %rax
    mul         %r10
    xor         %r9, %r9
    add         $16, %rsi
    add         %rax, %r8
    adc         %rdx, %r9
    mov         %r8, (%rdi)
    mov         -8(%rsi), %rax
    mul         %r10
    xor         %r8, %r8
    add         $16, %rdi
    add         %rax, %r9
    adc         %rdx, %r8
    mov         %r9, -8(%rdi)
    dec         %rcx
    jnz         1b

2:  mov         %r8, %rax
    retq

MVFUNC_END pmulu, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR pmulu, _x86_64

.endif
