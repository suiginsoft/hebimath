# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HAS_HWCAP_AVX && HAS_HWCAP_LZCNT
MVFUNC_BEGIN pclz, avx_lzcnt

    shl         $5, %rsi
    vpcmpeqd    %xmm4, %xmm4, %xmm4
    mov         %rsi, %rcx

    .p2align 4
1:  vmovdqa     -16(%rdi,%rsi), %xmm0
    vmovdqa     -32(%rdi,%rsi), %xmm1
    vptest      %xmm4, %xmm0
    vpextrq     $1, %xmm0, %rax
    jnz         3f
    vptest      %xmm4, %xmm1
    vpextrq     $1, %xmm1, %rax
    jnz         2f
    sub         $32, %rsi
    jnz         1b
    lea         (,%rcx,8), %rax
    ret

    .p2align 4,,7
2:  vmovdqa     %xmm1, %xmm0
    sub         $16, %rsi
3:  test        %rax, %rax
    jnz         4f
    vmovq       %xmm0, %rax
    sub         $8, %rsi
4:  lzcnt       %rax, %rax
    sub         %rsi, %rcx
    lea         (%rax,%rcx,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE41
MVFUNC_BEGIN pclz, sse41

    shl         $5, %rsi
    pcmpeqd     %xmm4, %xmm4
    mov         %rsi, %rcx

    .p2align 4
1:  movdqa      -16(%rdi,%rsi), %xmm0
    movdqa      -32(%rdi,%rsi), %xmm1
    ptest       %xmm4, %xmm0
    pextrq      $1, %xmm0, %rax
    jnz         3f
    ptest       %xmm4, %xmm1
    pextrq      $1, %xmm1, %rax
    jnz         2f
    sub         $32, %rsi
    jnz         1b
    lea         (,%rcx,8), %rax
    ret

    .p2align 4,,7
2:  movdqa      %xmm1, %xmm0
    sub         $16, %rsi
3:  test        %rax, %rax
    jnz         4f
    movq        %xmm0, %rax
    sub         $8, %rsi
4:  bsr         %rax, %rax
    sub         %rsi, %rcx
    xor         $63, %rax
    lea         (%rax,%rcx,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pclz, sse2

    mov         %rsi, %rcx
    shl         $5, %rsi
    pxor        %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi

    .p2align 4
1:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    movdqa      %xmm0, %xmm2
    movdqa      %xmm1, %xmm3
    pcmpeqd     %xmm4, %xmm2
    pcmpeqd     %xmm4, %xmm3
    pmovmskb    %xmm2, %eax
    pmovmskb    %xmm3, %edx
    cmp         $0xFFFF, %eax
    jne         3f
    cmp         $0xFFFF, %edx
    jne         2f
    sub         $32, %rdi
    dec         %rcx
    jnz         1b
    lea         (,%rsi,8), %rax
    ret

    .p2align 4,,7
2:  add         $16, %rsi
    mov         %edx, %eax
    movdqa      %xmm1, %xmm0
3:  shl         $5, %rcx
    cmp         $0xFF, %ah
    je          4f
    punpckhqdq  %xmm0, %xmm0
    sub         $8, %rsi
4:  movq        %xmm0, %rax
    bsr         %rax, %rax
    sub         %rcx, %rsi
    xor         $63, %rax
    lea         64(%rax,%rsi,8), %rax
    ret

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pclz

    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi

.if HAS_HWCAP_AVX && HAS_HWCAP_LZCNT
    mov         %eax, %r11d
    and         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    cmp         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    jne         1f
    lea         hebi_pclz_avx_lzcnt__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pclz_sse41__(%rip), %r10
    BREAK
.endif

2:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pclz_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
