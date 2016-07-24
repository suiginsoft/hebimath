# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

    xor         %rax, %rax
    test        %rcx, %rcx
    jnz         1f
    MOVSUB      %rdx, %rax
    retq

    # Add/subtract word to/from first packet, propagating carry/borrow

1:  bt          $0, %eax
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    ADCSBB      %rdx, %r8
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
    jz          3f

    # Propagation main loop: propagate carry/borrow into remaining packets

.align 16
2:  jnc         4f
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
    jnz         2b

3:  setc        %al
    retq

    # Carry no longer set, simply copy remaining data to destination

.align 16
4:  cmp         %rsi, %rdi
    je          5f
    mov         %rcx, %rdx
    MVFUNC_CALL pcopy, %r10
5:  xor         %rax, %rax
    retq
