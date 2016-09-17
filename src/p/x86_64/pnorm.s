# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pnorm(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pnorm, avx

    mov         %rsi, %rax
    mov         %rsi, %rcx
    shl         $5, %rsi
    and         $3, %rax
    lea         -16(%rdi,%rsi), %rdi
    shr         $2, %rcx
    jz          3f

.align 16
2:  vmovdqa     (%rdi), %xmm0
    vpor        -16(%rdi), %xmm0, %xmm0
    vmovdqa     -32(%rdi), %xmm1
    vpor        -48(%rdi), %xmm1, %xmm1
    vpor        %xmm0, %xmm1, %xmm1
    vptest      %xmm1, %xmm1
    jnz         6f
    vmovdqa     -64(%rdi), %xmm0
    vpor        -80(%rdi), %xmm0, %xmm0
    vmovdqa     -96(%rdi), %xmm1
    vpor        -112(%rdi), %xmm1, %xmm1
    vpor        %xmm0, %xmm1, %xmm1
    vptest      %xmm1, %xmm1
    jnz         7f
    sub         $128, %rdi
    dec         %rcx
    jnz         2b

3:  test        $3, %rax
    jz          5f
4:  vmovdqa     (%rdi), %xmm0
    vpor        -16(%rdi), %xmm0, %xmm0
    vptest      %xmm0, %xmm0
    jnz         5f
    sub         $32, %rdi
    dec         %eax
    jnz         4b
5:  retq

7:  sub         $2, %rax
6:  lea         (%rax,%rcx,4), %rax
    xor         %rcx, %rcx
    vptest      %xmm0, %xmm0
    setz        %cl
    sub         %rcx, %rax
    retq

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE41
MVFUNC_BEGIN pnorm, sse41

    mov         %rsi, %rax
    mov         %rsi, %rcx
    shl         $5, %rsi
    and         $3, %rax
    lea         -16(%rdi,%rsi), %rdi
    shr         $2, %rcx
    jz          3f

.align 16
2:  movdqa      (%rdi), %xmm0
    por         -16(%rdi), %xmm0
    movdqa      -32(%rdi), %xmm1
    por         -48(%rdi), %xmm1
    por         %xmm0, %xmm1
    ptest       %xmm1, %xmm1
    jnz         6f
    movdqa      -64(%rdi), %xmm0
    por         -80(%rdi), %xmm0
    movdqa      -96(%rdi), %xmm1
    por         -112(%rdi), %xmm1
    por         %xmm0, %xmm1
    ptest       %xmm1, %xmm1
    jnz         7f
    sub         $128, %rdi
    dec         %rcx
    jnz         2b

3:  test        $3, %rax
    jz          5f
4:  movdqa      (%rdi), %xmm0
    por         -16(%rdi), %xmm0
    ptest       %xmm0, %xmm0
    jnz         5f
    sub         $32, %rdi
    dec         %eax
    jnz         4b
5:  retq

7:  sub         $2, %rax
6:  lea         (%rax,%rcx,4), %rax
    xor         %rcx, %rcx
    ptest       %xmm0, %xmm0
    setz        %cl
    sub         %rcx, %rax
    retq

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pnorm, sse2

    mov         %rsi, %rcx
    shl         $5, %rsi
    lea         -16(%rdi,%rsi), %rdi
    pxor        %xmm2, %xmm2
    test        $1, %rcx
    jz          1f
    movdqa      (%rdi), %xmm0
    por         -16(%rdi), %xmm0
    pcmpeqd     %xmm2, %xmm0
    pmovmskb    %xmm0, %eax
    cmp         $0xFFFF, %eax
    jne         3f
    sub         $32, %rdi
1:  shr         $1, %rcx
    jrcxz       3f

.align 16
2:  movdqa      (%rdi), %xmm0
    por         -16(%rdi), %xmm0
    movdqa      -32(%rdi), %xmm1
    por         -48(%rdi), %xmm1
    por         %xmm0, %xmm1
    pcmpeqd     %xmm2, %xmm1
    pmovmskb    %xmm1, %eax
    cmp         $0xFFFF, %eax
    jne         4f
    sub         $64, %rdi
    dec         %rcx
    jnz         2b

3:  mov         %rcx, %rax
    retq

4:  shl         $1, %rcx
    pcmpeqd     %xmm2, %xmm0
    pmovmskb    %xmm0, %eax
    cmp         $0xFFFF, %eax
    jne         3b
    lea         -1(%rcx), %rax
    retq

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pnorm

    pushq       %rsi
    pushq       %rdi
    call        hebi_hwcaps__
    popq        %rdi
    popq        %rsi
    xor         %r10, %r10

.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pnorm_avx__(%rip), %r10
    jmp         3f
.endif

1:
.if HAS_HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pnorm_sse41__(%rip), %r10
    jmp         3f
.endif

2:
.if HAS_HWCAP_SSE2
    lea         hebi_pnorm_sse2__(%rip), %r10
.endif

3:  test        %r10, %r10
    jz          4f
    MVFUNC_USE  %r10
4:  jmp         hebi_hwcaps_fatal__

MVFUNC_DISPATCH_END
.endif
