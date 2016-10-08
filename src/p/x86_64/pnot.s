# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pnot(hebi_packet *r, const hebi_packet *a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pnot, sse2

    # Input:             Output:    Intermediate:
    #   %rdi %rsi %rdx      %rax         %r8-%r11
    #      r    a    n     carry          temp[4]

    pcmpeqd     %xmm2, %xmm2

    .align 4,,15
1:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    pandn       %xmm2, %xmm0
    pandn       %xmm2, %xmm1
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm0, (%rsi)
    add         $32, %rsi
    add         $32, %rdi
    dec         %rdx
    jnz         1b
    ret

MVFUNC_END
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

.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pnot_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
