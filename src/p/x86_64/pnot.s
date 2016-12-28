# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pnot(hebi_packet *r, const hebi_packet *a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro PNOT128 Version, CmpEqOp, AndnOp
MVFUNC_BEGIN pnot, \Version

    mov         %rdx, %rcx
    \CmpEqOp    %xmm4, %xmm4
    cmp         $1, %rdx
    ja          2f
1:  movdqa      (%rsi), %xmm0
    \AndnOp     %xmm4, %xmm0
    movdqa      %xmm0, (%rdi)
    ret
2:  shr         $2, %rcx
    jnz         4f
3:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    \AndnOp     %xmm4, %xmm0
    \AndnOp     %xmm4, %xmm1
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    add         $32, %rsi
    add         $32, %rdi
    test        $1, %dl
    jnz         1b
    ret

    .p2align 4,,15
4:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      32(%rsi), %xmm2
    movdqa      48(%rsi), %xmm3
    \AndnOp     %xmm4, %xmm0
    \AndnOp     %xmm4, %xmm1
    \AndnOp     %xmm4, %xmm2
    \AndnOp     %xmm4, %xmm3
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    movdqa      %xmm2, 32(%rdi)
    movdqa      %xmm3, 48(%rdi)
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

.macro AVXCMPEQD Src, Dst
    vpcmpeqd    \Src, \Dst, \Dst
.endm

.macro AVXANDN Src, Dst
    vpandn      \Src, \Dst, \Dst
.endm

.if HWCAP_AVX
PNOT128 avx, AVXCMPEQD, AVXANDN
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
PNOT128 sse2, pcmpeqd, pandn
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pnot

    push        %rdx
    push        %rsi
    push        %rdi
    call        hebi_hwcaps__
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rdx

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pnot_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pnot_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
