# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pzero(hebi_packet *r, size_t n);

# NOTE: hebi_pzero must not modify the rax, rdx, r8, or r9 registers.
# other x86-64 driver functions rely on being able to tail call pzero while
# passing a return value through without modification

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro PZERO128 Version, MoveOp, XorOp
MVFUNC_BEGIN pzero, \Version, @public, @explicit

    \XorOp      %xmm0, %xmm0
    mov         %rsi, %rcx
    cmp         $1, %rsi
    ja          2f
1:  \MoveOp     %xmm0, (%rdi)
    ret
2:  shr         $2, %rcx
    jnz         4f
3:  \MoveOp     %xmm0, (%rdi)
    \MoveOp     %xmm0, 16(%rdi)
    add         $32, %rdi
    test        $1, %sil
    jnz         1b
    ret

    .p2align 4,,15
4:  \MoveOp     %xmm0, (%rdi)
    \MoveOp     %xmm0, 16(%rdi)
    \MoveOp     %xmm0, 32(%rdi)
    \MoveOp     %xmm0, 48(%rdi)
    add         $64, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %sil
    jnz         3b
    test        $1, %sil
    jnz         1b
    ret

MVFUNC_END
.endm

#-------------------------------------------------------------------------------

.macro PXOR_AVX Src, Dst
    vpxor       \Src, \Dst, \Dst
.endm

.if HWCAP_AVX
PZERO128 avx, vmovdqa, PXOR_AVX
.endif

#-------------------------------------------------------------------------------

.macro PXOR_SSE2 Src, Dst
    pxor        \Src, \Dst
.endm

.if HWCAP_SSE2
PZERO128 sse2, movdqa, PXOR_SSE2
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pzero

    push        %rax
    push        %rdx
    push        %rdi
    push        %rsi
    push        %r8
    push        %r9
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    pop         %r9
    pop         %r8
    mov         %eax, %ecx
    pop         %rsi
    pop         %rdi
    xor         %r10, %r10
    pop         %rdx
    pop         %rax

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %ecx
    jz          1f
    lea         hebi_pzero_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %ecx
    BREAKZ
    lea         hebi_pzero_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
