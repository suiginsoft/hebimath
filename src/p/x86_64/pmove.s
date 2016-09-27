# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_packet *hebi_pmove(hebi_packet *r, const hebi_packet *a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX2
MVFUNC_BEGIN pmove, avx2

    xor         %r8, %r8
    mov         %rdx, %r9
    mov         $128, %rax
    mov         %r9, %rcx
    cmp         %rdi, %rsi
    jae         1f
    mov         %r9, %r8
    shl         $5, %r8
    neg         %rax
    lea         -128(%rsi,%r8), %rsi
    lea         -128(%rdi,%r8), %rdi
    sub         %rax, %r8
1:  shr         $2, %rcx
    jz          3f

.align 16
2:  vmovdqa     (%rsi), %ymm0
    vmovdqa     32(%rsi), %ymm1
    vmovdqa     64(%rsi), %ymm2
    vmovdqa     96(%rsi), %ymm3
    vmovdqu     %ymm0, (%rdi)
    vmovdqu     %ymm1, 32(%rdi)
    vmovdqu     %ymm2, 64(%rdi)
    vmovdqu     %ymm3, 96(%rdi)
    add         %rax, %rsi
    add         %rax, %rdi
    dec         %rcx
    jnz         2b

3:  and         $3, %r9d
    jz          5f
    mov         %rax, %rcx
    cqo
    sar         $2, %rax
    neg         %rcx
    and         %rdx, %rcx
    add         %rax, %rcx

4:  vmovdqa     (%rsi,%rcx), %ymm0
    vmovdqu     %ymm0, (%rdi,%rcx)
    add         %rax, %rsi
    add         %rax, %rdi
    dec         %r9d
    jnz         4b

5:  lea         (%rdi,%r8), %rax
    VZEROUPPER
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pmove, avx

    xor         %r8, %r8
    mov         $64, %rax
    mov         %rdx, %rcx
    cmp         %rdi, %rsi
    jae         1f
    mov         %rdx, %r8
    shl         $5, %r8
    neg         %rax
    lea         -64(%rsi,%r8), %rsi
    lea         -64(%rdi,%r8), %rdi
    sub         %rax, %r8
1:  shr         %rcx
    jz          3f

.align 16
2:  vmovdqa     (%rsi), %xmm0
    vmovdqa     16(%rsi), %xmm1
    vmovdqa     32(%rsi), %xmm2
    vmovdqa     48(%rsi), %xmm3
    vmovdqu     %xmm0, (%rdi)
    vmovdqu     %xmm1, 16(%rdi)
    vmovdqu     %xmm2, 32(%rdi)
    vmovdqu     %xmm3, 48(%rdi)
    add         %rax, %rsi
    add         %rax, %rdi
    dec         %rcx
    jnz         2b

3:  test        $1, %edx
    jz          4f
    cqo
    sar         $1, %rax
    mov         %rax, %rcx
    neg         %rcx
    and         %rdx, %rcx
    vmovdqa     (%rsi,%rcx), %xmm0
    vmovdqa     16(%rsi,%rcx), %xmm1
    vmovdqu     %xmm0, (%rdi,%rcx)
    vmovdqu     %xmm1, 16(%rdi,%rcx)
    add         %rax, %rdi
4:  lea         (%rdi,%r8), %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pmove, _sse2

    xor         %r8, %r8
    mov         $64, %rax
    mov         %rdx, %rcx
    cmp         %rdi, %rsi
    jae         1f
    mov         %rdx, %r8
    shl         $5, %r8
    neg         %rax
    lea         -64(%rsi,%r8), %rsi
    lea         -64(%rdi,%r8), %rdi
    sub         %rax, %r8
1:  shr         %rcx
    jz          3f

.align 16
2:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      32(%rsi), %xmm2
    movdqa      48(%rsi), %xmm3
    movdqu      %xmm0, (%rdi)
    movdqu      %xmm1, 16(%rdi)
    movdqu      %xmm2, 32(%rdi)
    movdqu      %xmm3, 48(%rdi)
    add         %rax, %rsi
    add         %rax, %rdi
    dec         %rcx
    jnz         2b

3:  test        $1, %edx
    jz          4f
    cqo
    sar         $1, %rax
    mov         %rax, %rcx
    neg         %rcx
    and         %rdx, %rcx
    movdqa      (%rsi,%rcx), %xmm0
    movdqa      16(%rsi,%rcx), %xmm1
    movdqu      %xmm0, (%rdi,%rcx)
    movdqu      %xmm1, 16(%rdi,%rcx)
    add         %rax, %rdi
4:  lea         (%rdi,%r8), %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pmove

    push        %rdx
    push        %rsi
    push        %rdi
    call        hebi_hwcaps__
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rdx

.if HAS_HWCAP_AVX2
    test        $hebi_hwcap_avx2, %eax
    jz          1f
    lea         hebi_pmove_avx2__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         hebi_pmove_avx__(%rip), %r10
    BREAK
.endif

2:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pmove_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
