# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pshl(hebi_packet *r, const hebi_packet *a, size_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_AVX
MVFUNC_BEGIN pshl, avx

    # Check if we have any packets to shift

    mov         %rcx, %rax
    test        %rcx, %rcx
    jz          5f

    # Compute number of bits and quadwords to shift

    mov         %rdx, %r8
    mov         %rdx, %r10
    add         $255, %r8
    shr         $6, %r10                # r10 := words = b / QWORD_BITS
    and         $63, %edx               # edx := b % QWORD_BITS
    shr         $8, %r8
    vpxor       %xmm0, %xmm0, %xmm0     # xmm0 := q = 0
    add         %rcx, %r8               # r8 := rn = n + (b + PACKET_BITS - 1) / PACKET_BITS
    mov         %rdi, %r9               # r9 := r
    mov         %r8, %r11
    shl         $5, %r11                # r11 := rn * PACKET_SIZE

    # Clear top packet if necessary, compute destination start
    # address, and check if shift is aligned on quadword boundary

    cmp         %rcx, %r8               # if rn > n then
    jle         1f                      #    r[rn-1] = 0
    vmovdqa     %xmm0, -32(%rdi,%r11)
    vmovdqa     %xmm0, -16(%rdi,%r11)
1:  lea         (%rdi,%r10,8), %rdi     # rdi := rw = r + words * 8
    test        %edx, %edx
    jz          6f

    # Bit-shifting loop

    shl         $5, %rax
    shl         $1, %rcx
    lea         -16(%rsi,%rax), %rsi    # rsi := aw + n * PACKET_SIZE - 16
    lea         8(%rdi,%rax), %rdi      # rdi := rw + n * PACKET_SIZE + 8
    mov         $64, %eax
    vpxor       %xmm4, %xmm4, %xmm4     # xmm4 := |00 00|
    sub         %edx, %eax
    vmovd       %edx, %xmm5             # xmm5 := bits = b % QWORD_BITS
    vmovd       %eax, %xmm6             # xmm6 := rbits = QWORD_BITS - (b % QWORD_BITS)

.align 16
2:  vmovdqa     (%rsi), %xmm2           # xmm2 := s = aw[i]
    sub         $16, %rdi
    vpsrlq      %xmm6, %xmm2, %xmm1     # xmm1 := |0R 0r| = s >> rbits
    vpsllq      %xmm5, %xmm2, %xmm2     # xmm2 := |L0 l0| = s << bits
    vpor        %xmm0, %xmm1, %xmm1     # xmm1 := |qR 0r|
    vpunpckhqdq %xmm4, %xmm2, %xmm3     # xmm3 := |00 L0|
    sub         $16, %rsi
    vpor        %xmm3, %xmm1, %xmm1     # xmm1 := |qR Lr|
    vpunpcklqdq %xmm2, %xmm4, %xmm0     # xmm0 := |l0 00|
    vmovdqu     %xmm1, (%rdi)           # rw[i+1] = s >> rbits | q
    dec         %rcx
    jnz         2b
    vpunpckhqdq %xmm0, %xmm0, %xmm0     # xmm0 := |q0 q0|
    vmovq       %xmm0, -8(%rdi)         # rw[0] := q

    # Zero the quadwords at beginning of result

3:  mov         %r10, %rcx
    jrcxz       4f
    cld
    mov         %r9, %rdi
    xor         %rax, %rax
    rep
    stosq

    # Determine length of result, only need to check last packet

4:  vmovdqa     -32(%r9,%r11), %xmm0
    mov         %r11, %rax
    vpor        -16(%r9,%r11), %xmm0, %xmm0
    xor         %rcx, %rcx
    shr         $5, %rax
    vptest      %xmm0, %xmm0
    setz        %cl
5:  sub         %rcx, %rax
    retq

    # Shift is aligned on quadword boundary, move packets (note
    # that hebi_pmove can handle unaligned destination address)

6:  mov         %rcx, %rdx
    pushq       %r9
    pushq       %r10
    pushq       %r11
    MVFUNC_CALL pmove, %rax
    popq        %r11
    popq        %r10
    popq        %r9
    jmp         3b

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE2
MVFUNC_BEGIN pshl, sse2

    # Check if we have any packets to shift

    mov         %rcx, %rax
    test        %rcx, %rcx
    jz          5f

    # Compute number of bits and quadwords to shift by

    mov         %rdx, %r8
    mov         %rdx, %r10
    add         $255, %r8
    shr         $6, %r10                # r10 := words = b / QWORD_BITS
    and         $63, %edx               # edx := b % QWORD_BITS
    shr         $8, %r8
    pxor        %xmm0, %xmm0            # xmm0 := q = 0
    add         %rcx, %r8               # r8 := rn = n + (b + PACKET_BITS - 1) / PACKET_BITS
    mov         %rdi, %r9               # r9 := r
    mov         %r8, %r11
    shl         $5, %r11                # r11 := rn * PACKET_SIZE

    # Clear top packet if necessary, compute destination start
    # address, and check if shift is aligned on quadword boundary

    cmp         %rcx, %r8               # if rn > n then
    jle         1f                      #    r[rn-1] = 0
    movdqa      %xmm0, -32(%rdi,%r11)
    movdqa      %xmm0, -16(%rdi,%r11)
1:  lea         (%rdi,%r10,8), %rdi     # rdi := rw = r + words * 8
    test        %edx, %edx
    jz          6f

    # Bit-shifting loop

    shl         $5, %rax
    shl         $1, %rcx
    lea         -16(%rsi,%rax), %rsi    # rsi := aw + n * PACKET_SIZE - 16
    lea         8(%rdi,%rax), %rdi      # rdi := rw + n * PACKET_SIZE + 8
    mov         $64, %eax
    sub         %edx, %eax
    movd        %edx, %xmm5             # xmm7 := bits = b % QWORD_BITS
    movd        %eax, %xmm6             # xmm6 := rbits = QWORD_BITS - (b % QWORD_BITS)

.align 16
2:  movdqa      (%rsi), %xmm1           # xmm1 := s = aw[i]
    sub         $16, %rdi
    pxor        %xmm4, %xmm4            # xmm4 := |00 00|
    movdqa      %xmm1, %xmm2            # xmm2 := s = aw[i]
    psrlq       %xmm6, %xmm1            # xmm1 := |0R 0r| = s >> rbits
    psllq       %xmm5, %xmm2            # xmm2 := |L0 l0| = s << bits
    por         %xmm0, %xmm1            # xmm1 := |qR 0r|
    movdqa      %xmm2, %xmm3
    punpckhqdq  %xmm4, %xmm2            # xmm2 := |00 L0|
    punpcklqdq  %xmm3, %xmm4            # xmm4 := |l0 00|
    por         %xmm2, %xmm1            # xmm1 := |qR Lr|
    sub         $16, %rsi
    movdqa      %xmm4, %xmm0            # xmm0 := q = s << bits
    movdqu      %xmm1, (%rdi)           # rw[i+1] = s >> rbits | q
    dec         %rcx
    jnz         2b
    punpckhqdq  %xmm0, %xmm0            # xmm0 := |q0 q0|
    movq        %xmm0, -8(%rdi)         # rw[0] := q

    # Zero the quadwords at beginning of result

3:  mov         %r10, %rcx
    jrcxz       4f
    cld
    mov         %r9, %rdi
    xor         %rax, %rax
    rep
    stosq

    # Determine length of result, only need to check last packet

4:  movdqa      -32(%r9,%r11), %xmm1
    pxor        %xmm0, %xmm0
    por         -16(%r9,%r11), %xmm1
    mov         %r11, %rax
    pcmpeqd     %xmm0, %xmm1
    shr         $5, %rax
    pmovmskb    %xmm1, %edx
    xor         %rcx, %rcx
    cmp         $0xFFFF, %edx
    setz        %cl
5:  sub         %rcx, %rax
    retq

    # Shift is aligned on quadword boundary, move packets (note
    # that hebi_pmove can handle unaligned destination address)

6:  mov         %rcx, %rdx
    pushq       %r9
    pushq       %r10
    pushq       %r11
    MVFUNC_CALL pmove, %rax
    popq        %r11
    popq        %r10
    popq        %r9
    jmp         3b

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pshl

    pushq       %rdi
    pushq       %rsi
    pushq       %rdx
    pushq       %rcx
    call        hebi_hwcaps__
    popq        %rcx
    popq        %rdx
    popq        %rsi
    popq        %rdi
    xor         %r10, %r10

.if HAS_HWCAP_AVX
    test        $hebi_hwcap_avx, %eax
    jz          1f
    lea         hebi_pshl_avx__(%rip), %r10
    jmp         2f
.endif

1:
.if HAS_HWCAP_SSE2
    lea         hebi_pshl_sse2__(%rip), %r10
.endif

2:  test        %r10, %r10
    jz          3f
    MVFUNC_USE  %r10
3:  jmp         hebi_hwcaps_fatal__

MVFUNC_DISPATCH_END
.endif
