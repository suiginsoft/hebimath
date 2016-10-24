# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pshl(hebi_packet *r, const hebi_packet *a, size_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HWCAP_AVX
FUNC_EXTERN pmove_avx, @private
FUNC_EXTERN pzero_avx, @private
MVFUNC_BEGIN pshl, avx

    # Compute number of bits and quadwords to shift

    mov         %rdx, %rax
    mov         %rdx, %r8
    mov         %rcx, %r10
    add         $127, %rax
    shr         $6, %r8                 # r8 := words = b / QWORD_BITS
    and         $63, %edx               # edx := b % QWORD_BITS
    shr         $7, %rax
    vpxor       %xmm4, %xmm4, %xmm4
    add         %rcx, %rax              # rax := rn = n + (b + PACKET_BITS - 1) / PACKET_BITS
    mov         %rdi, %r9               # r9 := r

    # Get destination start address of shifted data, and if shift
    # is aligned on quadword boundary use hebi_pmove

    lea         (%rdi,%r8,8), %rdi      # rdi := rw = r + words * 8
    shl         $4, %r10
    test        %edx, %edx
    jz          6f

    # Set up for bit-shifting loop

    vmovd       %edx, %xmm5             # xmm5 := bits = b % QWORD_BITS
    neg         %edx
    and         $63, %edx
    lea         -16(%rsi,%r10), %rsi    # rsi := aw + n * PACKET_SIZE - 16
    vmovd       %edx, %xmm6             # xmm6 := rbits = QWORD_BITS - (b % QWORD_BITS)
    lea         -8(%rdi,%r10), %rdi      # rdi := rw + n * PACKET_SIZE + 8

    # Process first octaword from input

    vmovdqa     (%rsi), %xmm2           # xmm2 := s = aw[i]
    vpxor       %xmm0, %xmm0, %xmm0     # xmm0 := |00 00| = q
    vpsrlq      %xmm6, %xmm2, %xmm1     # xmm1 := |0R 0r| = s >> rbits
    vpsllq      %xmm5, %xmm2, %xmm2     # xmm2 := |L0 l0| = s << bits
    vpunpckhqdq %xmm4, %xmm2, %xmm3     # xmm3 := |00 L0|
    sub         $16, %rsi
    vpor        %xmm3, %xmm1, %xmm1     # xmm1 := |0R Lr|
    vpunpcklqdq %xmm2, %xmm4, %xmm0     # xmm0 := |l0 00|

    # If shifting odd number of quadwords, store zero in top quadword of
    # destination and save destination in xmm7 for later size computation

    movdqa      %xmm1, %xmm7
    test        $1, %r8d
    jnz         2f
    vmovq       %xmm4, 16(%rdi)
    vpunpckhqdq %xmm4, %xmm7, %xmm7     # xmm7 := |00 0R|
    jmp         2f

    # Bit-shifting main loop

    .p2align 4,,15
1:  vmovdqa     (%rsi), %xmm2           # xmm2 := s = aw[i]
    sub         $16, %rdi
    vpsrlq      %xmm6, %xmm2, %xmm1     # xmm1 := |0R 0r| = s >> rbits
    vpsllq      %xmm5, %xmm2, %xmm2     # xmm2 := |L0 l0| = s << bits
    vpor        %xmm0, %xmm1, %xmm1     # xmm1 := |qR 0r|
    vpunpckhqdq %xmm4, %xmm2, %xmm3     # xmm3 := |00 L0|
    sub         $16, %rsi
    vpor        %xmm3, %xmm1, %xmm1     # xmm1 := |qR Lr|
    vpunpcklqdq %xmm2, %xmm4, %xmm0     # xmm0 := |l0 00|
2:  vmovdqu     %xmm1, (%rdi)           # rw[i+1] = s >> rbits | q
    dec         %rcx
    jnz         1b
    vmovq       %xmm2, -8(%rdi)         # rw[0] := q

    # Determine length of result in number of packets assuming that
    # input length was normalized. Zero the quadwords at beginning
    # of result (note that hebi_pzero does not modify rax and so it
    # is safe to tail-call while passing through result)

    .p2align 4,,7
3:  vptest      %xmm7, %xmm4
    mov         %r8, %rsi
    mov         %r9, %rdi
    sbb         $0, %rax
    test        $1, %sil
    jz          4f
    vmovq       %xmm4, -8(%rdi,%rsi,8)
4:  shr         %rsi
    jz          5f
    jmp         hebi_pzero_avx__
5:  ret

    # Shift is aligned on quadword boundary, move packets (note that
    # hebi_pmove can handle unaligned addresses, and does not modify
    # registers rax, r8, r9 or xmm4-xmm15)

    .p2align 4,,7
6:  vmovdqa     -16(%rsi,%r10), %xmm7
    mov         %rcx, %rdx
    test        $1, %r8d
    jz          7f
    vmovq       %xmm4, (%rdi,%r10)
    vmovdqu     %xmm7, -16(%rdi,%r10)
    vpunpckhqdq %xmm4, %xmm7, %xmm7
    dec         %rdx
    jz          3b
    call        hebi_pmove_avx__
    jmp         3b

    .p2align 4,,7
7:  vmovdqu     %xmm7, -16(%rdi,%r10)
    dec         %rdx
    jz          3b
    call        hebi_pmove_avx__
    jmp         3b

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
FUNC_EXTERN pmove_sse2, @private
FUNC_EXTERN pzero_sse2, @private
MVFUNC_BEGIN pshl, sse2

    # Check if we have any packets to shift

    mov         %rcx, %rax
    test        %rcx, %rcx
    jz          5f

    # Compute number of bits and quadwords to shift by

    mov         %rdx, %r10
    mov         %rdx, %r8
    add         $255, %r10
    shr         $6, %r8                 # r8 := words = b / QWORD_BITS
    and         $63, %edx               # edx := b % QWORD_BITS
    shr         $8, %r10
    pxor        %xmm0, %xmm0            # xmm0 := q = 0
    add         %rcx, %r10              # r10 := rn = n + (b + PACKET_BITS - 1) / PACKET_BITS
    mov         %rdi, %r9               # r9 := r
    mov         %r10, %r11
    shl         $5, %r11                # r11 := rn * PACKET_SIZE

    # Clear top packet if necessary, compute destination start
    # address, and check if shift is aligned on quadword boundary

    cmp         %rcx, %r10              # if rn > n then
    jle         1f                      #    r[rn-1] = 0
    movdqa      %xmm0, -32(%rdi,%r11)
    movdqa      %xmm0, -16(%rdi,%r11)
1:  lea         (%rdi,%r8,8), %rdi      # rdi := rw = r + words * 8
    test        %edx, %edx
    jz          6f

    # Bit-shifting loop

    shl         $5, %rax
    shl         %rcx
    lea         -16(%rsi,%rax), %rsi    # rsi := aw + n * PACKET_SIZE - 16
    lea         8(%rdi,%rax), %rdi      # rdi := rw + n * PACKET_SIZE + 8
    mov         $64, %eax
    sub         %edx, %eax
    movd        %edx, %xmm5             # xmm7 := bits = b % QWORD_BITS
    movd        %eax, %xmm6             # xmm6 := rbits = QWORD_BITS - (b % QWORD_BITS)

    .p2align 4,,15
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

3:  mov         %r8, %rcx
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
    ret

    # Shift is aligned on quadword boundary, move packets (note
    # that hebi_pmove can handle unaligned destination address)

    .p2align 4,,7
6:  mov         %rcx, %rdx
    push        %r11
    call        hebi_pmove_sse2__
    pop         %r11
    jmp         3b

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pshl

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
    lea         hebi_pshl_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %eax
    BREAKZ
    lea         hebi_pshl_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
