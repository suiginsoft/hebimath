# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t
# hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HWCAP_AVX && HWCAP_BMI1
MVFUNC_BEGIN pctz, avx_bmi1

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          3f
    xor         %eax, %eax
1:  tzcnt       8(%rdi), %rcx
    tzcnt       (%rdi), %rdx
    mov         $0, %esi
    sbb         $0, %rsi
    add         %rdx, %rax
    and         %rsi, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
2:  vmovq       %xmm0, %rdx
    tzcnt       %rcx, %rcx
    tzcnt       %rdx, %rdx
    mov         $0, %esi
    sbb         $0, %rsi
    add         %rdx, %rax
    and         %rsi, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  shr         %rsi
    mov         $-128, %rax
    vpcmpeqd    %xmm1, %xmm1, %xmm1

    .p2align 4,,15
4:  vmovdqa     (%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vpextrq     $1, %xmm0, %rcx
    jnz         2b
    vmovdqa     16(%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vpextrq     $1, %xmm0, %rcx
    jnz         2b
    add         $32, %rdi
    dec         %rsi
    jnz         4b
    add         $128, %rax
    test        $1, %dl
    jnz         1b
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pctz, avx

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          5f
    xor         %eax, %eax
1:  mov         (%rdi), %rcx
    mov         8(%rdi), %rdx
    jmp         4f

    .p2align 4,,7
2:  bsf         %rcx, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  vpextrq     $1, %xmm0, %rdx
4:  test        %rcx, %rcx
    jnz         2b
    mov         %rdx, %rcx
    add         $64, %rax
    test        %rdx, %rdx
    jnz         2b
    add         $64, %rax
    ret

    .p2align 4,,7
5:  shr         %rsi
    mov         $-128, %rax
    vpcmpeqd    %xmm1, %xmm1, %xmm1

    .p2align 4,,15
6:  vmovdqa     (%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vmovq       %xmm0, %rcx
    jnz         3b
    vmovdqa     16(%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vmovq       %xmm0, %rcx
    jnz         3b
    add         $32, %rdi
    dec         %rsi
    jnz         6b
    add         $128, %rax
    test        $1, %dl
    jnz         1b
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE41
MVFUNC_BEGIN pctz, sse41

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          5f
    xor         %eax, %eax
1:  mov         (%rdi), %rcx
    mov         8(%rdi), %rdx
    jmp         4f

    .p2align 4,,7
2:  bsf         %rcx, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  pextrq      $1, %xmm0, %rdx
4:  test        %rcx, %rcx
    jnz         2b
    mov         %rdx, %rcx
    add         $64, %rax
    test        %rdx, %rdx
    jnz         2b
    add         $64, %rax
    ret

    .p2align 4,,7
5:  shr         %rsi
    mov         $-128, %rax
    pcmpeqd     %xmm1, %xmm1

    .p2align 4,,15
6:  movdqa      (%rdi), %xmm0
    add         $128, %rax
    ptest       %xmm1, %xmm0
    movq        %xmm0, %rcx
    jnz         3b
    movdqa      16(%rdi), %xmm0
    add         $128, %rax
    ptest       %xmm1, %xmm0
    movq        %xmm0, %rcx
    jnz         3b
    add         $32, %rdi
    dec         %rsi
    jnz         6b
    add         $128, %rax
    test        $1, %dl
    jnz         1b
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pctz, sse2

    cmp         $1, %rsi
    ja          4f
    mov         (%rdi), %rcx
    xor         %eax, %eax
    test        %rcx, %rcx
    jnz         1f
    mov         8(%rdi), %rcx
    add         $64, %rax
    test        %rcx, %rcx
    jnz         1f
    add         $64, %rax
    ret

    .p2align 4,,7
1:  bsf         %rcx, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
2:  cmp         $0xFF, %cl
    je          3f
    movq        %xmm0, %rcx
    jmp         1b
3:  punpckhqdq  %xmm0, %xmm0
    add         $64, %rax
    movq        %xmm0, %rcx
    jmp         1b

    .p2align 4,,7
4:  mov         $-128, %rax
    pxor        %xmm1, %xmm1

    .p2align 4,,15
5:  movdqa      (%rdi), %xmm0
    add         $128, %rax
    pcmpeqd     %xmm0, %xmm1
    pmovmskb    %xmm1, %ecx
    pxor        %xmm1, %xmm1
    cmp         $0xFFFF, %ecx
    jne         2b
    add         $16, %rdi
    dec         %rsi
    jnz         5b
    add         $128, %rax
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
.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         hebi_pctz_avx__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          3f
    lea         hebi_pctz_sse41__(%rip), %r10
    BREAK
.endif

3:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pctz_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
