# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_packet *hebi_pzero(hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX2
MVFUNC_BEGIN pzero, avx2

    mov         %rsi, %rcx
    vpxor       %ymm0, %ymm0, %ymm0
    shr         $2, %rcx
    jz          2f

.align 16
1:  vmovdqa     %ymm0, (%rdi)
    vmovdqa     %ymm0, 32(%rdi)
    vmovdqa     %ymm0, 64(%rdi)
    vmovdqa     %ymm0, 96(%rdi)
    add         $128, %rdi
    dec         %rcx
    jnz         1b

2:  and         $3, %esi
    jz          4f

3:  vmovdqa     %ymm0, (%rdi)
    add         $32, %rdi
    dec         %esi
    jnz         3b

4:  mov         %rdi, %rax
    VZEROUPPER
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pzero, avx

    mov         %rsi, %rcx
    vpxor       %xmm0, %xmm0, %xmm0
    shr         %rcx
    jz          2f

.align 16
1:  vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm0, 16(%rdi)
    vmovdqa     %xmm0, 32(%rdi)
    vmovdqa     %xmm0, 48(%rdi)
    add         $64, %rdi
    dec         %rcx
    jnz         1b

2:  test        $1, %esi
    jz          3f
    vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm0, 16(%rdi)
    add         $32, %rdi

3:  mov         %rdi, %rax
    ret

MVFUNC_END
.endif


#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pzero, sse2

    mov         %rsi, %rcx
    pxor        %xmm0, %xmm0
    shr         %rcx
    jz          2f

.align 16
1:  movdqa      %xmm0, (%rdi)
    movdqa      %xmm0, 16(%rdi)
    movdqa      %xmm0, 32(%rdi)
    movdqa      %xmm0, 48(%rdi)
    add         $64, %rdi
    dec         %rcx
    jnz         1b

2:  test        $1, %esi
    jz          3f
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm0, 16(%rdi)
    add         $32, %rdi

3:  mov         %rdi, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pzero

    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi

.if HAS_HWCAP_AVX2
    test        $hebi_hwcap_avx2, %eax
    jz          1f
    lea         hebi_pzero_avx2__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         hebi_pzero_avx__(%rip), %r10
    BREAK
.endif

2:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pzero_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
