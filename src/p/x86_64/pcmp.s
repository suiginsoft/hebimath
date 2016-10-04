# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_normp(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX2
MVFUNC_BEGIN pcmp, avx2

    mov         %rdx, %rcx
    shl         $5, %rcx
    jz          2f
    sub         $32, %rdi
    sub         $32, %rsi
    vpxor       %ymm1, %ymm1, %ymm1

    .p2align 4,,15
1:  vmovdqa     (%rdi,%rcx), %ymm0
    vpxor       (%rsi,%rcx), %ymm0, %ymm0
    vptest      %ymm0, %ymm1
    jnc         3f
    sub         $32, %rcx
    jnz         1b
    VZEROUPPER

2:  xor         %eax, %eax
    ret

    .p2align 4,,7
3:  vpcmpeqd    %ymm3, %ymm3, %ymm3
    vmovdqa     (%rsi,%rcx), %ymm1
    vpslld      $31, %ymm3, %ymm3
    vpxor       %ymm3, %ymm1, %ymm1
    vpxor       %ymm1, %ymm0, %ymm0
    vpcmpgtd    %ymm1, %ymm0, %ymm2
    vpcmpgtd    %ymm0, %ymm1, %ymm1
    vpmovmskb   %ymm2, %eax
    vpmovmskb   %ymm1, %edx
    sub         %edx, %eax
    VZEROUPPER
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pcmp, avx

    mov         %rdx, %rcx
    shl         $5, %rcx
    jz          2f
    sub         $16, %rdi
    sub         $16, %rsi
    vpxor       %xmm1, %xmm1, %xmm1

    .p2align 4,,15
1:  vmovdqa     (%rdi,%rcx), %xmm0
    vpxor       (%rsi,%rcx), %xmm0, %xmm0
    vptest      %xmm0, %xmm1
    jnc         4f
    vmovdqa     -16(%rdi,%rcx), %xmm0
    vpxor       -16(%rsi,%rcx), %xmm0, %xmm0
    vptest      %xmm0, %xmm1
    jnc         3f
    sub         $32, %rcx
    jnz         1b

2:  xor         %eax, %eax
    ret

    .p2align 4,,7
3:  sub         $16, %rcx
4:  vpcmpeqd    %xmm3, %xmm3, %xmm3
    vmovdqa     (%rsi,%rcx), %xmm1
    vpslld      $31, %xmm3, %xmm3
    vpxor       %xmm3, %xmm1, %xmm1
    vpxor       %xmm1, %xmm0, %xmm0
    vpcmpgtd    %xmm1, %xmm0, %xmm2
    vpcmpgtd    %xmm0, %xmm1, %xmm1
    vpmovmskb   %xmm2, %eax
    vpmovmskb   %xmm1, %edx
    sub         %edx, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pcmp, sse41

    mov         %rdx, %rcx
    shl         $5, %rcx
    jz          2f
    sub         $16, %rdi
    sub         $16, %rsi
    pxor        %xmm1, %xmm1

    .p2align 4,,15
1:  movdqa      (%rdi,%rcx), %xmm0
    pxor        (%rsi,%rcx), %xmm0
    ptest       %xmm0, %xmm1
    jnc         4f
    movdqa      -16(%rdi,%rcx), %xmm0
    pxor        -16(%rsi,%rcx), %xmm0
    ptest       %xmm0, %xmm1
    jnc         3f
    sub         $32, %rcx
    jnz         1b

2:  xor         %eax, %eax
    ret

    .p2align 4,,7
3:  sub         $16, %rcx
4:  pcmpeqd     %xmm3, %xmm3
    movdqa      (%rsi,%rcx), %xmm1
    pslld       $31, %xmm3
    pxor        %xmm3, %xmm1
    pxor        %xmm1, %xmm0
    movdqa      %xmm0, %xmm2
    pcmpgtd     %xmm1, %xmm0
    pcmpgtd     %xmm2, %xmm1
    pmovmskb    %xmm0, %eax
    pmovmskb    %xmm1, %edx
    sub         %edx, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pcmp, sse2

    mov         %rdx, %rcx
    shl         $5, %rcx
    jz          2f
    sub         $16, %rdi
    sub         $16, %rsi

    .p2align 4,,15
1:  movdqa      (%rdi,%rcx), %xmm0
    pcmpeqb     (%rsi,%rcx), %xmm0
    pmovmskb    %xmm0, %eax
    sub         $0xFFFF, %eax
    jnz         4f
    movdqa      -16(%rdi,%rcx), %xmm0
    pcmpeqb     -16(%rsi,%rcx), %xmm0
    pmovmskb    %xmm0, %eax
    sub         $0xFFFF, %eax
    jnz         3f
    sub         $32, %rcx
    jnz         1b

2:  xor         %eax, %eax
    ret

    .p2align 4,,7
3:  sub         $16, %rcx
4:  pcmpeqd     %xmm3, %xmm3
    movdqa      (%rdi,%rcx), %xmm0
    pslld       $31, %xmm3
    movdqa      (%rsi,%rcx), %xmm1
    pxor        %xmm3, %xmm0
    pxor        %xmm3, %xmm1
    movdqa      %xmm0, %xmm2
    pcmpgtd     %xmm1, %xmm0
    pcmpgtd     %xmm2, %xmm1
    pmovmskb    %xmm0, %eax
    pmovmskb    %xmm1, %edx
    sub         %edx, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pcmp

    push        %rdx
    push        %rsi
    push        %rdi
    call        hebi_hwcaps__
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rdx

.if HWCAP_AVX2
    test        $hebi_hwcap_avx2, %eax
    jz          1f
    lea         hebi_pcmp_avx2__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         hebi_pcmp_avx__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          3f
    lea         hebi_pcmp_sse41__(%rip), %r10
    BREAK
.endif

3:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pcmp_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
