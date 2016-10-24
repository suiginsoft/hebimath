# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# int
# hebi_pcmp(const hebi_packet *a, const hebi_packet *b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pcmp, avx

    xor         %eax, %eax
    cmp         $1, %rdx
    ja          3f
1:  mov         8(%rdi), %rcx
    cmp         8(%rsi), %rcx
    jne         2f
    mov         (%rdi), %rcx
    cmp         (%rsi), %rcx
2:  seta        %al
    sbb         $0, %eax
    ret
3:  shl         $4, %rdx
    vpcmpeqd    %xmm2, %xmm2, %xmm2

    .p2align 4,,15
4:  vmovdqa     -16(%rdi,%rdx), %xmm0
    vpxor       -16(%rsi,%rdx), %xmm0, %xmm1
    vptest      %xmm1, %xmm2
    vpxor       %xmm0, %xmm1, %xmm1
    jnz         5f
    vmovdqa     -32(%rdi,%rdx), %xmm0
    vpxor       -32(%rsi,%rdx), %xmm0, %xmm1
    vptest      %xmm1, %xmm2
    vpxor       %xmm0, %xmm1, %xmm1
    jnz         5f
    sub         $32, %rdx
    cmp         $32, %rdx
    ja          4b
    test        %dl, %dl
    jnz         1b
    ret

    .p2align 4,,7
5:  vpextrq     $1, %xmm0, %rcx
    vpextrq     $1, %xmm1, %rdx
    vmovq       %xmm0, %rdi
    vmovq       %xmm1, %rsi
    cmp         %rdx, %rcx
    jne         6f
    cmp         %rsi, %rdi
6:  sbb         %eax, %eax
    or          $1, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pcmp, sse41

    xor         %eax, %eax
    cmp         $1, %rdx
    ja          3f
1:  mov         8(%rdi), %rcx
    cmp         8(%rsi), %rcx
    jne         2f
    mov         (%rdi), %rcx
    cmp         (%rsi), %rcx
2:  seta        %al
    sbb         $0, %eax
    ret
3:  shl         $4, %rdx
    pcmpeqd     %xmm2, %xmm2

    .p2align 4,,15
4:  movdqa      -16(%rdi,%rdx), %xmm0
    movdqa      -16(%rsi,%rdx), %xmm1
    pxor        %xmm0, %xmm1
    ptest       %xmm1, %xmm2
    pxor        %xmm0, %xmm1
    jnz         5f
    movdqa      -32(%rdi,%rdx), %xmm0
    movdqa      -32(%rsi,%rdx), %xmm1
    pxor        %xmm0, %xmm1
    ptest       %xmm1, %xmm2
    pxor        %xmm0, %xmm1
    jnz         5f
    sub         $32, %rdx
    cmp         $32, %rdx
    ja          4b
    test        %dl, %dl
    jnz         1b
    ret

    .p2align 4,,15
5:  pextrq      $1, %xmm0, %rcx
    pextrq      $1, %xmm1, %rdx
    movq        %xmm0, %rdi
    movq        %xmm1, %rsi
    cmp         %rdx, %rcx
    jne         6f
    cmp         %rsi, %rdi
6:  sbb         $0, %eax
    or          $1, %eax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pcmp, sse2

    xor         %eax, %eax
    cmp         $16, %rdx
    ja          3f
1:  mov         8(%rdi), %rcx
    cmp         8(%rsi), %rcx
    jne         2f
    mov         (%rdi), %rcx
    cmp         (%rsi), %rcx
2:  seta        %al
    sbb         $0, %eax
    ret
3:  pcmpeqd     %xmm3, %xmm3
    shl         $4, %rdx
    pslld       $31, %xmm3

    .p2align 4,,15
4:  movdqa      -16(%rdi,%rdx), %xmm0
    movdqa      -16(%rsi,%rdx), %xmm1
    movdqa      %xmm0, %xmm2
    pcmpeqb     %xmm1, %xmm2
    pmovmskb    %xmm2, %ecx
    sub         $0xFFFF, %ecx
    jnz         5f
    movdqa      -32(%rdi,%rdx), %xmm0
    movdqa      -32(%rsi,%rdx), %xmm1
    movdqa      %xmm0, %xmm2
    pcmpeqb     %xmm1, %xmm2
    pmovmskb    %xmm2, %ecx
    sub         $0xFFFF, %ecx
    jnz         5f
    sub         $32, %rdx
    cmp         $32, %rdx
    ja          4b
    test        %dl, %dl
    jnz         1b
    ret

    .p2align 4,,15
5:  pxor        %xmm3, %xmm0
    pxor        %xmm3, %xmm1
    movdqa      %xmm0, %xmm2
    pcmpgtd     %xmm1, %xmm0
    pcmpgtd     %xmm2, %xmm1
    pmovmskb    %xmm0, %ecx
    pmovmskb    %xmm1, %edx
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
