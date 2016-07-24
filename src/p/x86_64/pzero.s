# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_packet *hebi_pzero(hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX2
MVFUNC_BEGIN pzero, _avx2, 16

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
    retq

MVFUNC_END pzero, _avx2
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pzero, _avx, 16

    mov         %rsi, %rcx
    vpxor       %xmm0, %xmm0, %xmm0
    shr         $1, %rcx
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
    retq

MVFUNC_END pzero, _avx
.endif


#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pzero, _sse2, 16

    mov         %rsi, %rcx
    pxor        %xmm0, %xmm0
    shr         $1, %rcx
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
    retq

MVFUNC_END pzero, _sse2
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

.text
.align 16, 0x90
.type pzero_select, @function
pzero_select:

    push        %rsi
    push        %rdi
    call        hebi_hwcaps__
    popq        %rdi
    popq        %rsi
    xor         %r10, %r10

.if HAS_HWCAP_AVX2
    test        $hebi_hwcap_avx2, %eax
    jz          1f
    lea         pzero_avx2(%rip), %r10
    jmp         3f
.endif

1:
.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         pzero_avx(%rip), %r10
    jmp         3f
.endif

2:
.if HAS_HWCAP_SSE2
    lea         pzero_sse2(%rip), %r10
.endif

3:  test        %r10, %r10
    jz          4f
    MVFUNC_USE  pzero, %r10, %rax
4:  jmp         hebi_hwcaps_fatal__

.size pzero_select, .-pzero_select

MVFUNC_PTR pzero, _select

.endif
