# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void hebi_pzero(hebi_packet *r, size_t n);

# NOTE: hebi_pzero must not modify the rax register, as other x86-64 driver
# functions rely on being able to tail call pzero while passing a return
# value through without modification.

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pzero, avx

    mov         %rsi, %rcx
    vpxor       %xmm0, %xmm0, %xmm0
    shr         %rcx
    jnz         2f
1:  vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm0, 16(%rdi)
    ret

    .p2align 4,,15
2:  vmovdqa     %xmm0, (%rdi)
    vmovdqa     %xmm0, 16(%rdi)
    vmovdqa     %xmm0, 32(%rdi)
    vmovdqa     %xmm0, 48(%rdi)
    add         $64, %rdi
    dec         %rcx
    jnz         2b
    test        $1, %sil
    jnz         1b
    ret

MVFUNC_END
.endif


#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pzero, sse2

    mov         %rsi, %rcx
    pxor        %xmm0, %xmm0
    shr         %rcx
    jnz         2f
1:  movdqa      %xmm0, (%rdi)
    movdqa      %xmm0, 16(%rdi)
    ret

    .align 4,,15
2:  movdqa      %xmm0, (%rdi)
    movdqa      %xmm0, 16(%rdi)
    movdqa      %xmm0, 32(%rdi)
    movdqa      %xmm0, 48(%rdi)
    add         $64, %rdi
    dec         %rcx
    jnz         2b
    test        $1, %sil
    jz          1b
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pzero

    push        %rax
    push        %rsi
    push        %rdi
    call        hebi_hwcaps__
    mov         %eax, %ecx
    pop         %rdi
    pop         %rsi
    xor         %r10, %r10
    pop         %rax

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %ecx
    jz          1f
    lea         hebi_pzero_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %ecx
    BREAKZ
    lea         hebi_pzero_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
