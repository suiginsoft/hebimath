# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pcopy(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n);

# NOTE: hebi_pcopy must not modify the rax register, as other x86-64 driver
# functions rely on being able to tail call pcopy while passing a return value
# through without modification.

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro PCOPY128 Version, CopyOp
MVFUNC_BEGIN pcopy, \Version

    mov         %rdx, %rcx
    cmp         $1, %rdx
    ja          2f
1:  \CopyOp     (%rsi), %xmm0
    \CopyOp     %xmm0, (%rdi)
    ret
2:  shr         $2, %rcx
    jnz         4f
3:  \CopyOp     (%rsi), %xmm0
    \CopyOp     16(%rsi), %xmm1
    \CopyOp     %xmm0, (%rdi)
    \CopyOp     %xmm1, 16(%rdi)
    add         $32, %rsi
    add         $32, %rdi
    test        $1, %dl
    jnz         1b
    ret

    .p2align 4,,15
4:  \CopyOp     (%rsi), %xmm0
    \CopyOp     16(%rsi), %xmm1
    \CopyOp     32(%rsi), %xmm2
    \CopyOp     48(%rsi), %xmm3
    \CopyOp     %xmm0, (%rdi)
    \CopyOp     %xmm1, 16(%rdi)
    \CopyOp     %xmm2, 32(%rdi)
    \CopyOp     %xmm3, 48(%rdi)
    add         $64, %rsi
    add         $64, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %dl
    jnz         3b
    test        $1, %dl
    jnz         1b
    ret

MVFUNC_END
.endm

#-------------------------------------------------------------------------------

.if HWCAP_AVX
PCOPY128 avx, vmovdqa
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
PCOPY128 sse2, movdqa
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pcopy

    push        %rax
    push        %rdx
    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    mov         %eax, %ecx
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rdx
    pop         %rax

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %ecx
    jz          1f
    lea         hebi_pcopy_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %ecx
    BREAKZ
    lea         hebi_pcopy_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
