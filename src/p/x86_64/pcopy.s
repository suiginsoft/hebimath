# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_packet *hebi_pcopy(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX2
MVFUNC_BEGIN pcopy, _avx2, 16

    mov         %rdx, %rcx
    shr         $2, %rcx
    jz          2f

.align 16
1:  vmovdqa     (%rsi), %ymm0
    vmovdqa     32(%rsi), %ymm1
    vmovdqa     64(%rsi), %ymm2
    vmovdqa     96(%rsi), %ymm3
    vmovdqa     %ymm0, (%rdi)
    vmovdqa     %ymm1, 32(%rdi)
    vmovdqa     %ymm2, 64(%rdi)
    vmovdqa     %ymm3, 96(%rdi)
    add         $128, %rsi
    add         $128, %rdi
    dec         %rcx
    jnz         1b

2:  and         $3, %edx
    jz          4f

3:  vmovdqa     (%rsi), %ymm0
    vmovdqa     %ymm0, (%rdi)
    add         $32, %rsi
    add         $32, %rdi
    dec         %edx
    jnz         3b

4:  mov         %rdi, %rax
    VZEROPUPPER
    retq

MVFUNC_END pcopy, _avx2
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pcopy, _avx, 16

    mov         %rdx, %rcx
    shr         $1, %rcx
    jz          2f

.align 16
1:  vmovdqa     (%rsi), %xmm0
    vmovdqa     16(%rsi), %xmm1
    vmovdqa     32(%rsi), %xmm2
    vmovdqa     48(%rsi), %xmm3
    vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm1, 16(%rdi)
    vmovdqa     %xmm2, 32(%rdi)
    vmovdqa     %xmm3, 48(%rdi)
    add         $64, %rsi
    add         $64, %rdi
    dec         %rcx
    jnz         1b

2:  test        $1, %edx
    jz          3f
    vmovdqa     (%rsi), %xmm0
    vmovdqa     16(%rsi), %xmm1
    vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm1, 16(%rdi)
    add         $32, %rdi

3:  mov         %rdi, %rax
    retq

MVFUNC_END pcopy, _avx
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pcopy, _sse2, 16

    mov         %rdx, %rcx
    shr         $1, %rcx
    jz          2f

.align 16
1:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      32(%rsi), %xmm2
    movdqa      48(%rsi), %xmm3
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    movdqa      %xmm2, 32(%rdi)
    movdqa      %xmm3, 48(%rdi)
    add         $64, %rsi
    add         $64, %rdi
    dec         %rcx
    jnz         1b

2:  test        $1, %edx
    jz          3f
    movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    add         $32, %rdi

3:  mov         %rdi, %rax
    retq

MVFUNC_END pcopy, _sse2
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

.text
.align 16, 0x90
.type pcopy_select, @function
pcopy_select:

    pushq       %rdx
    pushq       %rsi
    pushq       %rdi
    call        hebi_hwcaps__
    popq        %rdi
    popq        %rsi
    popq        %rdx
    xor         %r10, %r10

.if HAS_HWCAP_AVX2
    test        $hebi_hwcap_avx2, %eax
    jz          1f
    lea         pcopy_avx2(%rip), %r10
    jmp         3f
.endif

1:
.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         pcopy_avx(%rip), %r10
    jmp         3f
.endif

2:
.if HAS_HWCAP_SSE2
    lea         pcopy_sse2(%rip), %r10
.endif

3:  test        %r10, %r10
    jz          4f
    MVFUNC_USE  pcopy, %r10, %rax
4:  jmp         hebi_hwcaps_fatal__

.size pcopy_select, .-pcopy_select

MVFUNC_PTR pcopy, _select

.endif
