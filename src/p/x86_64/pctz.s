# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t
# hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HWCAP_AVX && HWCAP_BMI1
MVFUNC_BEGIN pctz, avx_bmi1

    xor         %edx, %edx
    vpcmpeqd    %xmm4, %xmm4, %xmm4

    .p2align 4,,15
2:  vmovdqa     (%rdi,%rdx), %xmm0
    vmovdqa     16(%rdi,%rdx), %xmm1
    vptest      %xmm4, %xmm0
    vmovq       %xmm0, %rax
    jnz         4f
    vptest      %xmm4, %xmm1
    vmovq       %xmm1, %rax
    jnz         3f
    add         $32, %rdx
    dec         %rsi
    jnz         2b
    lea         (,%rdx,8), %rax
    ret

    .p2align 4,,15
3:  vmovdqa     %xmm1, %xmm0
    add         $16, %rdx
4:  test        %rax, %rax
    jnz         5f
    vpextrq     $1, %xmm0, %rax
    add         $8, %rdx
5:  tzcnt       %rax, %rax
    lea         (%rax,%rdx,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pctz, sse41

    xor         %edx, %edx
    pcmpeqd     %xmm4, %xmm4

    .p2align 4,,15
2:  movdqa      (%rdi,%rdx), %xmm0
    movdqa      16(%rdi,%rdx), %xmm1
    ptest       %xmm4, %xmm0
    movq        %xmm0, %rax
    jnz         4f
    ptest       %xmm4, %xmm1
    movq        %xmm1, %rax
    jnz         3f
    add         $32, %rdx
    dec         %rsi
    jnz         2b
    lea         (,%rdx,8), %rax
    ret

    .p2align 4,,15
3:  movdqa      %xmm1, %xmm0
    add         $16, %rdx
4:  test        %rax, %rax
    jnz         5f
    pextrq      $1, %xmm0, %rax
    add         $8, %rdx
5:  bsf         %rax, %rax
    lea         (%rax,%rdx,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pctz, sse2

    xor         %ecx, %ecx
    pxor        %xmm4, %xmm4

    .p2align 4,,15
2:  movdqa      (%rdi,%rcx), %xmm0
    movdqa      16(%rdi,%rcx), %xmm1
    movdqa      %xmm0, %xmm2
    movdqa      %xmm1, %xmm3
    pcmpeqd     %xmm4, %xmm2
    pcmpeqd     %xmm4, %xmm3
    pmovmskb    %xmm2, %eax
    pmovmskb    %xmm3, %edx
    cmp         $0xFFFF, %eax
    jne         4f
    cmp         $0xFFFF, %edx
    jne         3f
    add         $32, %rcx
    dec         %rsi
    jnz         2b
    lea         (,%rcx,8), %rax
    ret

3:  movdqa      %xmm1, %xmm0
    mov         %edx, %eax
    add         $16, %rcx
4:  cmp         $0xFF, %al
    jne         5f
    punpckhqdq  %xmm0, %xmm0
    add         $8, %rcx
5:  movq        %xmm0, %rax
    bsf         %rax, %rax
    lea         (%rax,%rcx,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pctz

    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi

.if HWCAP_AVX && HWCAP_BMI1
    mov         %eax, %r11d
    and         $(hebi_hwcap_avx+hebi_hwcap_bmi1), %r11d
    cmp         $(hebi_hwcap_avx+hebi_hwcap_bmi1), %r11d
    jne         1f
    lea         hebi_pctz_avx_bmi1__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pctz_sse41__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pctz_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
