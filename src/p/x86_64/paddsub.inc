# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

    # Setup for main loop

    mov         %rcx, %rax
    mov         %r8, %rcx
    sub         %r8, %rax
    bt          $0, %edi            # address is aligned, lowest bit is zero

    # Operation main loop: process 256-bit chunks at a time

    .p2align 4,,15
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

    mov         %rax, %rcx
    mov         $0, %eax
    jrcxz       3f
    jc          5f

    # Carry not set, copy remaining data to destination

2:  mov         %rcx, %rdx
    MVFUNC_JMP  pcopy, %r10
3:  setc        %al
    ret

    # Propagation main loop: propagate carry/borrow in 256-bit chunks at a time

    .p2align 4,,15
4:  jnc         2b
5:  mov         (%rsi), %r8
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
    jne         4b
    setc        %al
    ret
