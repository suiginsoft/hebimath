# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_pneg(hebi_packet *r, const hebi_packet *a size_t n);

.include "src/p/x86_64/x86_64.inc"

FUNC_BEGIN pneg

    # Negation loop unrolled 4x
    #
    # Input:             Output:    Intermediate:
    #   %rdi %rsi %rdx      %rax         %r8-%r11
    #      r    a    n     carry          temp[4]

    xor         %eax, %eax
    bt          $0, %edi        # clear carry, address is aligned, lsb is 0

    .p2align 4,,15
1:  mov         %eax, %r8d
    mov         %eax, %r9d
    mov         %eax, %r10d
    mov         %eax, %r11d
    sbb         (%rsi), %r8
    sbb         8(%rsi), %r9
    sbb         16(%rsi), %r10
    sbb         24(%rsi), %r11
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    mov         %r10, 16(%rdi)
    mov         %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdi), %rdi
    dec         %rdx
    jnz         1b

    setc        %al
    ret

FUNC_END
