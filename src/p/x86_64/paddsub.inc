# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

    mov         %rcx, %rax
    mov         %r8, %rcx
    sub         %r8, %rax
    bt          $0, %edi
    jrcxz       2f

    # Operation main loop: process 256-bit chunks at a time

.align 16
1:  mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    ADCSBB      (%rdx), %r8
    ADCSBB      8(%rdx), %r9
    ADCSBB      16(%rdx), %r10
    ADCSBB      24(%rdx), %r11
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    mov         %r10, 16(%rdi)
    mov         %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdx), %rdx
    lea         32(%rdi), %rdi
    dec         %rcx
    jne         1b

2:  xchg        %rax, %rcx
    jrcxz       4f

    # Propagation main loop: propagate carry/borrow in 256-bit chunks at a time

.align 16
3:  jnc         5f
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    ADCSBB      %rax, %r8
    ADCSBB      %rax, %r9
    ADCSBB      %rax, %r10
    ADCSBB      %rax, %r11
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    mov         %r10, 16(%rdi)
    mov         %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdi), %rdi
    dec         %rcx
    jne         3b

4:  setc        %al
    retq

    # Carry no longer set, simply copy remaining data to destination

.align 16
5:  cmp         %rsi, %rdi
    je          6f
    mov         %rcx, %rdx
    MVFUNC_CALL pcopy, %r10
6:  xor         %rax, %rax
    retq
