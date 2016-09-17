# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pclz(const hebi_packet *r, size_t n);

.include "src/p/x86_64/x86_64.inc"

#------------------------------------------------------------------------------

.if HAS_HWCAP_AVX && HAS_HWCAP_LZCNT
MVFUNC_BEGIN pctz, avx_lzcnt

    mov         %rsi, %rcx
    shl         $5, %rsi
    vpcmpeqd    %xmm4, %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

.align 16
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
    retq

4:  vpextrq     $1, %xmm0, %rax
    shl         $5, %rcx
    test        %rax, %rax
    jnz         5f
    vmovq       %xmm0, %rax
    add         $8, %rsi
5:  lzcnt       %rax, %rax
    sub         %rcx, %rsi
    lea         (%rax,%rsi,8), %rax
    retq

6:  add         $16, %rsi
    vmovdqa     %xmm1, %xmm0
    jmp         4b

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE41
MVFUNC_BEGIN pctz, sse41

    mov         %rsi, %rcx
    shl         $5, %rsi
    pcmpeqd     %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

.align 16
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
    retq

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
    retq

6:  add         $16, %rsi
    movdqa      %xmm1, %xmm0
    jmp         4b

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pctz, sse2

    mov         %rsi, %rcx
    shl         $5, %rsi
    pxor        %xmm4, %xmm4
    lea         -16(%rdi,%rsi), %rdi
    jrcxz       3f

.align 16
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
    retq

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
    retq

6:  add         $16, %rsi
    mov         %edx, %eax
    movdqa      %xmm1, %xmm0
    jmp         4b

MVFUNC_END
.endif

#------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pctz

    pushq       %rsi
    pushq       %rdi
    call        hebi_hwcaps__
    popq        %rdi
    popq        %rsi
    xor         %r10, %r10

.if HAS_HWCAP_AVX && HAS_HWCAP_LZCNT
    mov         %eax, %r11d
    and         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    cmp         $(hebi_hwcap_avx+hebi_hwcap_lzcnt), %r11d
    jne         4f
    lea         hebi_pctz_avx_lzcnt__(%rip), %r10
    jmp         1f
.endif

1:
.if HAS_HWCAP_SSE41
    test        $hebi_hwcap_sse41, %eax
    jz          2f
    lea         hebi_pctz_sse41__(%rip), %r10
    jmp         3f
.endif

2:
.if HAS_HWCAP_SSE2
    lea         hebi_pctz_sse2__(%rip), %r10
.endif

3:  test        %r10, %r10
    jz          4f
    MVFUNC_USE  %r10
4:  jmp         hebi_hwcaps_fatal__

MVFUNC_DISPATCH_END
.endif
