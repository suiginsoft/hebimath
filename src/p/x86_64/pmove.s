# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_packet *hebi_pmove(hebi_packet *r, const hebi_packet *a, size_t n);

.include "src/p/x86_64/x86_64.inc"

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
4:  ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pmove, sse2

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
4:  ret

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

.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pmove_avx__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pmove_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
