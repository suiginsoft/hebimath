# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HWCAP_AVX && HWCAP_LZCNT
MVFUNC_BEGIN pclz, avx_lzcnt

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          3f
    xor         %eax, %eax
1:  lzcnt       (%rdi), %rcx
    lzcnt       8(%rdi), %rdx
    mov         $0, %esi
    sbb         $0, %rsi
    add         %rdx, %rax
    and         %rsi, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
2:  vpextrq     $1, %xmm0, %rdx
    lzcnt       %rcx, %rcx
    lzcnt       %rdx, %rdx
    mov         $0, %esi
    sbb         $0, %rsi
    add         %rdx, %rax
    and         %rsi, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  shl         $4, %rsi
    lea         -16(%rdi,%rsi), %rdi
    mov         $-128, %rax
    vpcmpeqd    %xmm1, %xmm1, %xmm1
    shr         $5, %rsi

    .p2align 4,,15
4:  vmovdqa     (%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vmovq       %xmm0, %rcx
    jnz         2b
    vmovdqa     -16(%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vmovq       %xmm0, %rcx
    jnz         2b
    sub         $32, %rdi
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
MVFUNC_BEGIN pclz, avx

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          5f
    xor         %eax, %eax
1:  mov         8(%rdi), %rcx
    mov         (%rdi), %rdx
    jmp         4f

    .p2align 4,,7
2:  bsr         %rcx, %rcx
    xor         $63, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  vmovq       %xmm0, %rdx
4:  test        %rcx, %rcx
    jnz         2b
    mov         %rdx, %rcx
    add         $64, %rax
    test        %rdx, %rdx
    jnz         2b
    add         $64, %rax
    ret

    .p2align 4,,7
5:  shl         $4, %rsi
    lea         -16(%rdi,%rsi), %rdi
    mov         $-128, %rax
    vpcmpeqd    %xmm1, %xmm1, %xmm1
    shr         $5, %rsi

    .p2align 4,,15
6:  vmovdqa     (%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vpextrq     $1, %xmm0, %rcx
    jnz         3b
    vmovdqa     -16(%rdi), %xmm0
    add         $128, %rax
    vptest      %xmm1, %xmm0
    vpextrq     $1, %xmm0, %rcx
    jnz         3b
    sub         $32, %rdi
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
MVFUNC_BEGIN pclz, sse41

    mov         %rsi, %rdx
    cmp         $1, %rsi
    ja          5f
    xor         %eax, %eax
1:  mov         8(%rdi), %rcx
    mov         (%rdi), %rdx
    jmp         4f

    .p2align 4,,7
2:  bsr         %rcx, %rcx
    xor         $63, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  movq        %xmm0, %rdx
4:  test        %rcx, %rcx
    jnz         2b
    mov         %rdx, %rcx
    add         $64, %rax
    test        %rdx, %rdx
    jnz         2b
    add         $64, %rax
    ret

    .p2align 4,,7
5:  shl         $4, %rsi
    lea         -16(%rdi,%rsi), %rdi
    mov         $-128, %rax
    pcmpeqd     %xmm1, %xmm1
    shr         $5, %rsi

    .p2align 4,,15
6:  movdqa      (%rdi), %xmm0
    add         $128, %rax
    ptest       %xmm1, %xmm0
    pextrq      $1, %xmm0, %rcx
    jnz         3b
    movdqa      -16(%rdi), %xmm0
    add         $128, %rax
    ptest       %xmm1, %xmm0
    pextrq      $1, %xmm0, %rcx
    jnz         3b
    sub         $32, %rdi
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
MVFUNC_BEGIN pclz, sse2

    cmp         $1, %rsi
    ja          5f
    mov         8(%rdi), %rcx
    xor         %eax, %eax
    test        %rcx, %rcx
    jnz         2f
    mov         (%rdi), %rcx
    add         $64, %rax
    test        %rcx, %rcx
    jnz         2f
    add         $64, %rax
    ret

    .p2align 4,,7
2:  bsr         %rcx, %rcx
    xor         $63, %rcx
    add         %rcx, %rax
    ret

    .p2align 4,,7
3:  cmp         $0xFF, %ch
    je          4f
    punpckhqdq  %xmm0, %xmm0
    movq        %xmm0, %rcx
    jmp         2b
4:  add         $64, %rax
    movq        %xmm0, %rcx
    jmp         2b

    .p2align 4,,7
5:  shl         $4, %rsi
    lea         -16(%rdi,%rsi), %rdi
    mov         $-128, %rax
    pxor        %xmm1, %xmm1
    shr         $4, %rsi

    .p2align 4,,15
6:  movdqa      (%rdi), %xmm0
    add         $128, %rax
    pcmpeqd     %xmm0, %xmm1
    pmovmskb    %xmm1, %ecx
    pxor        %xmm1, %xmm1
    cmp         $0xFFFF, %ecx
    jne         3b
    sub         $16, %rdi
    dec         %rsi
    jnz         6b
    add         $128, %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pclz

    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi

.if HWCAP_AVX && HWCAP_LZCNT
    mov         %eax, %r11d
    and         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    cmp         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    jne         1f
    lea         hebi_pclz_avx_lzcnt__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          2f
    lea         hebi_pclz_avx__(%rip), %r10
    BREAK
.endif

2:
.if HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          3f
    lea         hebi_pclz_sse41__(%rip), %r10
    BREAK
.endif

3:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pclz_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
