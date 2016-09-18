# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HAS_HWCAP_AVX && HAS_HWCAP_LZCNT
MVFUNC_BEGIN pclz, avx_lzcnt

    mov         %rsi, %rcx
    shl         $5, %rsi
    vpcmpeqd    %xmm4, %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

    .p2align 4,,15
2:  vmovdqa     (%rdi), %xmm0
    vmovdqa     -16(%rdi), %xmm1
    vptest      %xmm4, %xmm0
    jnz         4f
    vptest      %xmm4, %xmm1
    jnz         6f
    sub         $32, %rdi
    dec         %rcx
    jnz         2b
3:  lea         (,%rsi,8), %rax
    ret

4:  vpextrq     $1, %xmm0, %rax
    shl         $5, %rcx
    test        %rax, %rax
    jnz         5f
    vmovq       %xmm0, %rax
    add         $8, %rsi
5:  lzcnt       %rax, %rax
    sub         %rcx, %rsi
    lea         (%rax,%rsi,8), %rax
    ret

6:  add         $16, %rsi
    vmovdqa     %xmm1, %xmm0
    jmp         4b

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE41
MVFUNC_BEGIN pclz, sse41

    mov         %rsi, %rcx
    shl         $5, %rsi
    pcmpeqd     %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

    .p2align 4,,15
2:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    ptest       %xmm4, %xmm0
    jnz         4f
    ptest       %xmm4, %xmm1
    jnz         6f
    sub         $32, %rdi
    dec         %rcx
    jnz         2b
3:  lea         (,%rsi,8), %rax
    ret

4:  pextrq      $1, %xmm0, %rax
    shl         $5, %rcx
    test        %rax, %rax
    jnz         5f
    movq        %xmm0, %rax
    add         $8, %rsi
5:  bsr         %rax, %rax
    sub         %rcx, %rsi
    xor         $63, %rax
    lea         (%rax,%rsi,8), %rax
    ret

6:  add         $16, %rsi
    movdqa      %xmm1, %xmm0
    jmp         4b

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pclz, sse2

    mov         %rsi, %rcx
    shl         $5, %rsi
    pxor        %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

    .p2align 4,,15
2:  movdqa      (%rdi), %xmm0
    movdqa      -16(%rdi), %xmm1
    movdqa      %xmm0, %xmm2
    movdqa      %xmm1, %xmm3
    pcmpeqd     %xmm4, %xmm2
    pcmpeqd     %xmm4, %xmm3
    pmovmskb    %xmm2, %eax
    pmovmskb    %xmm3, %edx
    cmp         $0xFFFF, %eax
    jne         4f
    cmp         $0xFFFF, %edx
    jne         6f
    sub         $32, %rdi
    dec         %rcx
    jnz         2b
3:  lea         (,%rsi,8), %rax
    ret

4:  shl         $5, %rcx
    cmp         $0xFF, %ah
    je          5f
    punpckhqdq  %xmm0, %xmm0
    sub         $8, %rsi
5:  movq        %xmm0, %rax
    bsr         %rax, %rax
    sub         %rcx, %rsi
    xor         $63, %rax
    lea         64(%rax,%rsi,8), %rax
    ret

6:  add         $16, %rsi
    mov         %edx, %eax
    movdqa      %xmm1, %xmm0
    jmp         4b

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
