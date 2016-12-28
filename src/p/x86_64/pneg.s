# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_pneg(hebi_packet *r, const hebi_packet *a, size_t n);

.include "src/p/x86_64/x86_64.inc"

FUNC_BEGIN pneg

    mov         %rdx, %rcx
    xor         %eax, %eax
    and         $1, %rdx
    shr         %rcx
    bt          $0, %edi        # clear carry, address is aligned, lsb is 0
    jrcxz       2f

    .p2align 4,,15
1:  mov         $0, %r8d
    mov         $0, %r9d
    mov         $0, %r10d
    mov         $0, %r11d
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
    dec         %rcx
    jnz         1b

    .p2align 4,,7
2:  dec         %rdx
    jnz         3f
    mov         $0, %r8d
    mov         $0, %r9d
    sbb         (%rsi), %r8
    sbb         8(%rsi), %r9
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
3:  setc        %al
    ret

FUNC_END
