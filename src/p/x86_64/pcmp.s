# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_normp(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pcmp, avx

    shl         $5, %rdx
    vpcmpeqd    %xmm3, %xmm3, %xmm3

    .p2align 4,,15
1:  vmovdqa     -16(%rdi,%rdx), %xmm0
    vmovdqa     -16(%rsi,%rdx), %xmm1
    vpxor       %xmm1, %xmm0, %xmm2
    vptest      %xmm2, %xmm3
    vpextrq     $1, %xmm0, %rcx
    jnz         2f
    vmovdqa     -32(%rdi,%rdx), %xmm0
    vmovdqa     -32(%rsi,%rdx), %xmm1
    vpxor       %xmm1, %xmm0, %xmm2
    vptest      %xmm2, %xmm3
    vpextrq     $1, %xmm0, %rcx
    jnz         2f
    sub         $32, %rdx
    jnz         1b
    xor         %eax, %eax
    ret

    .p2align 4,,15
2:  vpextrq     $1, %xmm1, %rdx
    vmovq       %xmm0, %rdi
    vmovq       %xmm1, %rsi
    xor         %eax, %eax
    cmp         %rdx, %rcx
    jne         3f
    cmp         %rsi, %rdi
3:  sbb         $0, %eax
    or          $1, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pcmp, sse41

    shl         $5, %rdx
    pcmpeqd     %xmm2, %xmm2

    .p2align 4,,15
1:  movdqa      -16(%rdi,%rdx), %xmm0
    movdqa      -16(%rsi,%rdx), %xmm1
    pxor        %xmm0, %xmm1
    ptest       %xmm1, %xmm2
    pxor        %xmm0, %xmm1
    jnz         2f
    movdqa      -32(%rdi,%rdx), %xmm0
    movdqa      -32(%rsi,%rdx), %xmm1
    pxor        %xmm0, %xmm1
    ptest       %xmm1, %xmm2
    pxor        %xmm0, %xmm1
    jnz         2f
    sub         $32, %rdx
    jnz         1b
    xor         %eax, %eax
    ret

    .p2align 4,,15
2:  pextrq      $1, %xmm0, %rcx
    pextrq      $1, %xmm1, %rdx
    movq        %xmm0, %rdi
    movq        %xmm1, %rsi
    xor         %eax, %eax
    cmp         %rdx, %rcx
    jne         3f
    cmp         %rsi, %rdi
3:  sbb         $0, %eax
    or          $1, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pcmp, sse2

    shl         $5, %rdx

    .p2align 4,,15
1:  movdqa      -16(%rdi,%rdx), %xmm0
    movdqa      -16(%rsi,%rdx), %xmm1
    movdqa      %xmm0, %xmm2
    pcmpeqb     %xmm1, %xmm2
    pmovmskb    %xmm2, %eax
    sub         $0xFFFF, %eax
    jnz         2f
    movdqa      -32(%rdi,%rdx), %xmm0
    movdqa      -32(%rsi,%rdx), %xmm1
    movdqa      %xmm0, %xmm2
    pcmpeqb     %xmm1, %xmm2
    pmovmskb    %xmm2, %eax
    sub         $0xFFFF, %eax
    jnz         2f
    sub         $32, %rdx
    jnz         1b
    xor         %eax, %eax
    ret

    .p2align 4,,7
2:  pcmpeqd     %xmm3, %xmm3
    pslld       $31, %xmm3
    pxor        %xmm3, %xmm0
    pxor        %xmm3, %xmm1
    movdqa      %xmm0, %xmm2
    pcmpgtd     %xmm1, %xmm0
    pcmpgtd     %xmm2, %xmm1
    pmovmskb    %xmm0, %ecx
    pmovmskb    %xmm1, %edx
    xor         %eax, %eax
    cmp         %edx, %ecx
    sbb         $0, %eax
    or          $1, %eax
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

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pcmp_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pcmp_sse41__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pcmp_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
