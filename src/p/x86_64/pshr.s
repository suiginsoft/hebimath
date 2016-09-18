# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pshr(hebi_packet *r, const hebi_packet *a, size_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pshr, avx

    # Compute number of quadwords to shift

    mov         %rdx, %r8
    shl         $2, %rcx                # rcx := nqwords = n * PACKET_QWORDS
    shr         $6, %r8                 # r8 := bqwords = b / WORD_BITS
    mov         %rcx, %rax
    cmp         %r8, %rcx
    jle         5f

    # Compute number of bits to shift, source start offset, load first quadword
    # from source and setup for bit-shifting loop

    sub         %r8, %rcx               # rcx := nqwords - bqwords
    and         $63, %edx               # edx := b % QWORD_BITS
    lea         (%rsi,%r8,8), %rsi      # rsi := aw = aw + nqwords
    mov         $64, %eax
    vmovq       (%rsi), %xmm0
    sub         %edx, %eax
    dec         %rcx
    vmovd       %edx, %xmm5             # xmm5 := b % QWORD_BITS
    vmovd       %eax, %xmm6             # xmm6 := QWORD_BITS - (b % QWORD_BITS)
    add         $8, %rsi                # rsi := aw + n * PACKET_SIZE - 16
    sub         $16, %rdi               # rdi := rw + n * PACKET_SIZE + 8
    mov         %rcx, %rdx
    vpsrlq      %xmm5, %xmm0, %xmm0     # xmm0 := |00 0q|
    vpxor       %xmm4, %xmm4, %xmm4
    shr         $1, %rcx
    jz          2f

    # Bit-shifting loop, process one octaword at a time

.align 16
1:  vmovdqu     (%rsi), %xmm2           # xmm2 := aw[i+1]
    add         $16, %rdi
    vpsllq      %xmm6, %xmm2, %xmm1     # xmm1 := |L0 l0|
    vpsrlq      %xmm5, %xmm2, %xmm2     # xmm2 := |0R 0r|
    vpor        %xmm0, %xmm1, %xmm1     # xmm1 := |L0 lq|
    vpunpcklqdq %xmm2, %xmm4, %xmm3     # xmm0 := |0r 00|
    add         $16, %rsi
    vpor        %xmm3, %xmm1, %xmm1     # xmm1 := |Lr lq|
    vpunpckhqdq %xmm4, %xmm2, %xmm0     # xmm0 := |00 0R|
    vmovdqa     %xmm1, (%rdi)           # rw[i] = aw[i+1] << rbits | q
    dec         %rcx
    jnz         1b

    # Load last quadword from source if there is one, otherwise store
    # the final octaword to destination

2:  test        $1, %dl
    jz          3f
    vmovq       (%rsi), %xmm2           # xmm2 := aw[i+1]
    vpsllq      %xmm6, %xmm2, %xmm3     # xmm3 := |00 l0|
    vpsrlq      %xmm5, %xmm2, %xmm2     # xmm2 := |00 0r|
    vpor        %xmm3, %xmm0, %xmm0     # xmm0 := |00 lq|
    vpunpcklqdq %xmm2, %xmm4, %xmm4     # xmm4 := |0r 00|
    vpor        %xmm4, %xmm0, %xmm0     # xmm0 := |0r lq|
3:  vmovdqa     %xmm0, 16(%rdi)

    # Zero last octaword of destination, if shifted odd number of octawords

    test        $2, %dl
    jnz         4f
    vmovdqa     %xmm4, %xmm1
    vmovdqa     %xmm4, 32(%rdi)

    # Determine length of result in number of packets, only need to check
    # last packet of destination

4:  lea         4(,%rdx), %rax
    vpor        %xmm1, %xmm0, %xmm0
    shr         $2, %rax
    vptest      %xmm0, %xmm0
    setz        %cl
5:  sub         %rcx, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pshr, sse2

    # Compute number of quadwords to shift

    mov         %rdx, %r8
    shl         $2, %rcx                # rcx := nqwords = n * PACKET_QWORDS
    shr         $6, %r8                 # r8 := bqwords = b / WORD_BITS
    mov         %rcx, %rax
    cmp         %r8, %rcx
    jle         5f

    # Compute number of bits to shift, source start offset, load first quadword
    # from source and setup for bit-shifting loop

    sub         %r8, %rcx               # rcx := nqwords - bqwords
    and         $63, %edx               # edx := b % QWORD_BITS
    lea         (%rsi,%r8,8), %rsi      # rsi := aw = aw + nqwords
    mov         $64, %eax
    movq        (%rsi), %xmm0
    sub         %edx, %eax
    dec         %rcx
    movd        %edx, %xmm5             # xmm5 := b % QWORD_BITS
    movd        %eax, %xmm6             # xmm6 := QWORD_BITS - (b % QWORD_BITS)
    add         $8, %rsi                # rsi := aw + n * PACKET_SIZE - 16
    sub         $16, %rdi               # rdi := rw + n * PACKET_SIZE + 8
    mov         %rcx, %rdx
    psrlq       %xmm5, %xmm0            # xmm0 := |00 0q|
    shr         $1, %rcx
    jz          2f

    # Bit-shifting loop, process one octaword at a time

.align 16
1:  movdqu      (%rsi), %xmm1           # xmm2 := aw[i+1]
    add         $16, %rdi
    pxor        %xmm4, %xmm4
    movdqa      %xmm1, %xmm2
    psllq       %xmm6, %xmm1            # xmm1 := |L0 l0|
    psrlq       %xmm5, %xmm2            # xmm2 := |0R 0r|
    por         %xmm0, %xmm1            # xmm1 := |L0 lq|
    movdqa      %xmm2, %xmm3
    punpckhqdq  %xmm4, %xmm2            # xmm2 := |00 0R|
    punpcklqdq  %xmm3, %xmm4            # xmm4 := |0r 00|
    add         $16, %rsi
    por         %xmm4, %xmm1            # xmm1 := |Lr lq|
    movdqa      %xmm2, %xmm0            # xmm0 := q = s >> bits
    movdqa      %xmm1, (%rdi)           # rw[i] = aw[i+1] << rbits | q
    dec         %rcx
    jnz         1b

    # Load last quadword from source if there is one, otherwise store
    # the final octaword to destination

2:  test        $1, %dl
    jz          3f
    movq        (%rsi), %xmm2           # xmm2 := aw[i+1]
    pxor        %xmm4, %xmm4
    movdqa      %xmm2, %xmm3
    psllq       %xmm6, %xmm2            # xmm2 := |00 l0|
    psrlq       %xmm5, %xmm3            # xmm3 := |00 0r|
    por         %xmm2, %xmm0            # xmm0 := |00 lq|
    punpcklqdq  %xmm2, %xmm4            # xmm2 := |0r 00|
    por         %xmm4, %xmm0            # xmm0 := |0r lq|
3:  movdqa      %xmm0, 16(%rdi)

    # Zero last octaword of destination, if shifted odd number of octawords

    pxor        %xmm4, %xmm4
    test        $2, %dl
    jnz         4f
    movdqa      %xmm4, %xmm1
    movdqa      %xmm4, 32(%rdi)

    # Determine length of result in number of packets, only need to check
    # last packet of destination

4:  por         %xmm1, %xmm0
    lea         4(,%rdx), %rax
    pcmpeqd     %xmm4, %xmm0
    pmovmskb    %xmm0, %edx
    shr         $2, %rax
    cmp         $0xFFFF, %edx
    setz        %cl
5:  sub         %rcx, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pshr

    push        %rdi
    push        %rsi
    push        %rdx
    push        %rcx
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rcx
    pop         %rdx
    pop         %rsi
    pop         %rdi

.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pshr_avx__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pshr_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
