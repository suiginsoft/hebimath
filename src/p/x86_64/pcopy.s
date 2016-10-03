# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pcopy(hebi_packet *restrict r, const hebi_packet *restrict a, size_t n);

# NOTE: hebi_pcopy must not modify the rax register, as other x86-64 driver
# functions rely on being able to tail call pcopy while passing a return value
# through without modification.

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pcopy, avx

    mov         %rdx, %rcx
    shr         %rdx
    jnz         2f
1:  vmovdqa     (%rsi), %xmm0
    vmovdqa     16(%rsi), %xmm1
    vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm1, 16(%rdi)
    ret

    .p2align 4,,15
2:  vmovdqa     (%rsi), %xmm0
    vmovdqa     16(%rsi), %xmm1
    vmovdqa     32(%rsi), %xmm2
    vmovdqa     48(%rsi), %xmm3
    vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm1, 16(%rdi)
    vmovdqa     %xmm2, 32(%rdi)
    vmovdqa     %xmm3, 48(%rdi)
    add         $64, %rsi
    add         $64, %rdi
    dec         %rdx
    jnz         2b
    test        $1, %cl
    jnz         1b
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pcopy, sse2

    mov         %rdx, %rcx
    shr         %rdx
    jnz         2f
1:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    ret

    .p2align 4,,15
2:  movdqa      (%rsi), %xmm0
    movdqa      16(%rsi), %xmm1
    movdqa      32(%rsi), %xmm2
    movdqa      48(%rsi), %xmm3
    movdqa      %xmm0, (%rdi)
    movdqa      %xmm1, 16(%rdi)
    movdqa      %xmm2, 32(%rdi)
    movdqa      %xmm3, 48(%rdi)
    add         $64, %rsi
    add         $64, %rdi
    dec         %rdx
    jnz         2b
    test        $1, %cl
    jnz         1b
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pcopy

    push        %rax
    push        %rdx
    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    mov         %eax, %ecx
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rdx
    pop         %rax

.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %ecx
    jz          1f
    lea         hebi_pcopy_avx__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %ecx
    BREAKZ
    lea         hebi_pcopy_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
