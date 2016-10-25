# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t
# hebi_pnorm(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pnorm, avx

    mov         %rsi, %rcx
    cmp         $1, %rsi
    ja          2f
1:  vmovdqa     (%rdi), %xmm0
    vptest      %xmm0, %xmm0
    mov         $0, %eax
    setnz       %al
    ret
2:  shl         $4, %rsi
    mov         %rcx, %rax
    lea         -16(%rdi,%rsi), %rdi
    shr         $2, %rcx
    jnz         4f
3:  vmovdqa     (%rdi), %xmm0
    vpor        -16(%rdi), %xmm0, %xmm1
    vptest      %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    sub         $32, %rdi
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,15
4:  vmovdqa     (%rdi), %xmm0
    vpor        -16(%rdi), %xmm0, %xmm1
    vptest      %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    vmovdqa     -32(%rdi), %xmm0
    vpor        -48(%rdi), %xmm0, %xmm1
    vptest      %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    sub         $64, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %al
    jnz         3b
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,7
5:  vptest      %xmm0, %xmm0
    mov         $0, %edx
    lea         2(%rax), %rax
    setz        %dl
    sub         %rdx, %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pnorm, sse41

    mov         %rsi, %rcx
    cmp         $1, %rsi
    ja          2f
1:  movdqa      (%rdi), %xmm0
    ptest       %xmm0, %xmm0
    mov         $0, %eax
    setnz       %al
    ret
2:  shl         $4, %rsi
    mov         %rcx, %rax
    lea         -16(%rdi,%rsi), %rdi
    shr         $2, %rcx
    jnz         4f
3:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    por         %xmm0, %xmm1
    ptest       %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    sub         $32, %rdi
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,15
4:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    por         %xmm0, %xmm1
    ptest       %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    movdqa      -32(%rdi), %xmm0
    movdqa      -48(%rdi), %xmm1
    por         %xmm0, %xmm1
    ptest       %xmm1, %xmm1
    lea         -2(%rax), %rax
    jnz         5f
    sub         $64, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %al
    jnz         3b
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,7
5:  ptest       %xmm0, %xmm0
    mov         $0, %edx
    lea         2(%rax), %rax
    setz        %dl
    sub         %rdx, %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pnorm, sse2

    mov         %rsi, %rcx
    pxor        %xmm2, %xmm2
    cmp         $1, %rsi
    ja          2f
1:  movdqa      (%rdi), %xmm0
    pcmpeqd     %xmm2, %xmm0
    pmovmskb    %xmm0, %edx
    xor         %eax, %eax
    cmp         $0xFFFF, %edx
    setne       %al
    ret
2:  shl         $4, %rsi
    mov         %rcx, %rax
    lea         -16(%rdi,%rsi), %rdi
    shr         $2, %rcx
    jnz         4f
3:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    por         %xmm0, %xmm1
    pcmpeqd     %xmm2, %xmm1
    pmovmskb    %xmm1, %edx
    sub         $2, %rax
    cmp         $0xFFFF, %edx
    jne         5f
    sub         $32, %rdi
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,15
4:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    por         %xmm0, %xmm1
    pcmpeqd     %xmm2, %xmm1
    pmovmskb    %xmm1, %edx
    sub         $2, %rax
    cmp         $0xFFFF, %edx
    jne         5f
    movdqa      -32(%rdi), %xmm0
    movdqa      -48(%rdi), %xmm1
    por         %xmm0, %xmm1
    pcmpeqd     %xmm2, %xmm1
    pmovmskb    %xmm1, %edx
    sub         $2, %rax
    cmp         $0xFFFF, %edx
    jne         5f
    sub         $64, %rdi
    dec         %rcx
    jnz         4b
    test        $2, %al
    jnz         3b
    test        $1, %al
    jnz         1b
    ret

    .p2align 4,,7
5:  pcmpeqd     %xmm2, %xmm0
    pmovmskb    %xmm0, %edx
    xor         %ecx, %ecx
    add         $2, %rax
    cmp         $0xFFFF, %edx
    sete        %cl
    sub         %rcx, %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pnorm

    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pnorm_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pnorm_sse41__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pnorm_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
