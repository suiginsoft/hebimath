# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "src/p/x86_64/x86_64.inc"

.macro BITWISE_LOGIC Name, OpSSE2, OpAVX

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN \Name, sse2

    # Input:
    # %rdi %rsi %rdx %rcx
    #    r    a    b    n

1:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    \OpSSE2     (%rdx), %xmm0
    \OpSSE2     16(%rdx), %xmm1
    add         $32, %rsi
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    add         $32, %rdx
    add         $32, %rdi
    dec         %rcx
    jnz         1b
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN \Name

    push        %rcx
    push        %rdx
    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi
    pop         %rdx
    pop         %rcx

.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_\Name\()_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif

#-------------------------------------------------------------------------------

.endm
