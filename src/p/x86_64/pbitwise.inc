# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro BITWISE128 Name, Version, BitwiseOp
MVFUNC_BEGIN \Name, \Version

    mov         %rcx, %rax
    cmp         $1, %rcx
    ja          2f
1:  movdqa      (%rsi), %xmm0
    \BitwiseOp  (%rdx), %xmm0
    movdqa      %xmm0, (%rdi)
    ret
2:  shr         $2, %rcx
    jnz         4f
3:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    \BitwiseOp  (%rdx), %xmm0
    \BitwiseOp  16(%rdx), %xmm1
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    add         $32, %rsi
    add         $32, %rdx
    add         $32, %rdi
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,15
4:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      32(%rsi), %xmm2
    movdqa      48(%rsi), %xmm3
    \BitwiseOp  (%rdx), %xmm0
    \BitwiseOp  16(%rdx), %xmm1
    \BitwiseOp  32(%rdx), %xmm2
    \BitwiseOp  48(%rdx), %xmm3
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    movdqa      %xmm2, 32(%rdi)
    movdqa      %xmm3, 48(%rdi)
    add         $32, %rsi
    add         $32, %rdx
    add         $32, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %al
    jnz         3b
    test        $1, %al
    jnz         1b
    ret

MVFUNC_END
.endm

#-------------------------------------------------------------------------------

.macro BITWISE_LOGIC Name, SseOp, AvxOp

.macro BITWISEAVXOP Src, Dst
    \AvxOp      \Src, \Dst, \Dst
.endm

.if HWCAP_AVX
BITWISE128 \Name, avx, BITWISEAVXOP
.endif

.purgem BITWISEAVXOP

.if HWCAP_SSE2
BITWISE128 \Name, sse2, \SseOp
.endif

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

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_\Name\()_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_\Name\()_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif

.endm
