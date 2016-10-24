# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pshr(hebi_packet *r, const hebi_packet *a, size_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX
MVFUNC_BEGIN pshr, avx

    # Compute number of limbs to shift

    mov         %rdx, %r8
    shl         %rcx
    shr         $6, %r8                 # r8 := b / LIMB_BIT
    sub         %r8, %rcx               # rcx := n * LIMB_PER_PACKET - b / LIMB_BIT
    jle         4f

    # Compute number of bits to shift, source start offset, load first quadword
    # from source and setup for bit-shifting loop

    lea         (%rsi,%r8,8), %rsi      # rsi := al + limbs shifted
    and         $63, %edx               # edx := b % LIMB_BIT
    mov         $64, %eax
    dec         %rcx
    sub         %edx, %eax
    vmovq       (%rsi), %xmm0
    vmovd       %edx, %xmm5             # xmm5 := b % LIMB_BIT
    vmovd       %eax, %xmm6             # xmm6 := LIMB_BIT - (b % LIMB_BIT)
    add         $8, %rsi                # rsi := al + n * PACKET_SIZE - 16
    sub         $16, %rdi               # rdi := rl + n * PACKET_SIZE + 8
    mov         %rcx, %rdx
    vpsrlq      %xmm5, %xmm0, %xmm0     # xmm0 := |00 0q|
    vpxor       %xmm4, %xmm4, %xmm4
    shr         %rcx
    jz          2f

    # Bit-shifting loop, process one octaword at a time

    .p2align 4,,15
1:  vmovdqu     (%rsi), %xmm2           # xmm2 := al[i+1]
    add         $16, %rdi
    vpsllq      %xmm6, %xmm2, %xmm1     # xmm1 := |L0 l0|
    vpsrlq      %xmm5, %xmm2, %xmm2     # xmm2 := |0R 0r|
    vpor        %xmm0, %xmm1, %xmm1     # xmm1 := |L0 lq|
    vpunpcklqdq %xmm2, %xmm4, %xmm3     # xmm0 := |0r 00|
    add         $16, %rsi
    vpor        %xmm3, %xmm1, %xmm1     # xmm1 := |Lr lq|
    vpunpckhqdq %xmm4, %xmm2, %xmm0     # xmm0 := |00 0R|
    vmovdqa     %xmm1, (%rdi)           # rw[i] = al[i+1] << rbits | q
    dec         %rcx
    jnz         1b

    # Load last quadword from source if there is one, otherwise store
    # the final octaword to destination

2:  test        $1, %dl
    jz          3f
    vmovq       (%rsi), %xmm2           # xmm2 := al[i+1]
    vpsllq      %xmm6, %xmm2, %xmm3     # xmm3 := |00 l0|
    vpsrlq      %xmm5, %xmm2, %xmm2     # xmm2 := |00 0r|
    vpor        %xmm3, %xmm0, %xmm0     # xmm0 := |00 lq|
    vpunpcklqdq %xmm2, %xmm4, %xmm2     # xmm4 := |0r 00|
    vpor        %xmm2, %xmm0, %xmm0     # xmm0 := |0r lq|
3:  add         $2, %rdx
    vmovdqa     %xmm0, 16(%rdi)

    # Assuming input length is normalized, determine length of result in
    # number of packets by examining the last packet

    shr         %rdx
    vptest      %xmm0, %xmm4
    mov         %rdx, %rax
    sbb         $0, %rax
    ret

    # Shifted more bits than in input, return zero length result

4:  xor         %rax, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
MVFUNC_BEGIN pshr, sse2

    # Compute number of quadwords to shift

    mov         %rdx, %r8
    shl         %rcx
    shr         $6, %r8                 # r8 := b / LIMB_BIT
    sub         %r8, %rcx               # rcx := n * LIMB_PER_PACKET - b / LIMB_BIT
    jle         4f

    # Compute number of bits to shift, source start offset, load first quadword
    # from source and setup for bit-shifting loop

    lea         (%rsi,%r8,8), %rsi      # rsi := al + number of limbs shifted
    and         $63, %edx               # edx := b % LIMB_BIT
    mov         $64, %eax
    dec         %rcx
    sub         %edx, %eax
    movq        (%rsi), %xmm0
    movd        %edx, %xmm5             # xmm5 := b % LIMB_BIT
    movd        %eax, %xmm6             # xmm6 := LIMB_BIT - (b % LIMB_BIT)
    add         $8, %rsi                # rsi := al + n * PACKET_SIZE - 16
    sub         $16, %rdi               # rdi := rl + n * PACKET_SIZE + 8
    mov         %rcx, %rdx
    psrlq       %xmm5, %xmm0            # xmm0 := |00 0q|
    shr         %rcx
    jz          2f

    # Bit-shifting loop, process one octaword at a time

    .p2align 4,,15
1:  movdqu      (%rsi), %xmm1           # xmm2 := al[i+1]
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
    movdqa      %xmm1, (%rdi)           # rl[i] = al[i+1] << rbits | q
    dec         %rcx
    jnz         1b

    # Load last quadword from source if there is one, otherwise store
    # the final octaword to destination

2:  test        $1, %dl
    jz          3f
    movq        (%rsi), %xmm2           # xmm2 := al[i+1]
    pxor        %xmm4, %xmm4
    movdqa      %xmm2, %xmm3
    psllq       %xmm6, %xmm2            # xmm2 := |00 l0|
    psrlq       %xmm5, %xmm3            # xmm3 := |00 0r|
    por         %xmm2, %xmm0            # xmm0 := |00 lq|
    punpcklqdq  %xmm3, %xmm4            # xmm2 := |0r 00|
    por         %xmm4, %xmm0            # xmm0 := |0r lq|
3:  add         $2, %rdx
    pxor        %xmm4, %xmm4
    movdqa      %xmm0, 16(%rdi)

    # Determine length of result in number of packets, only need to check
    # last packet of destination

    pcmpeqd     %xmm4, %xmm0
    pmovmskb    %xmm0, %eax
    shr         $2, %rdx
    cmp         $0xFFFF, %eax
    mov         %rdx, %rax
    setz        %cl
    sub         %rcx, %rax
    ret

    # Shifted more bits than in input, return zero length result

4:  xor         %rax, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
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

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pshr_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pshr_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
