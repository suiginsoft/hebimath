# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

    # Add/subtract word to/from first packet, propagating carry/borrow

    xor         %rax, %rax
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    ADDSUB      %rdx, %r8
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
    jnz         1f
    setc        %al
    ret

    # Propagation main loop: propagate carry/borrow into remaining packets

    .p2align 4,,15
1:  jnc         2f
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
    jnz         1b
    setc        %al
    ret

    # Carry no longer set, simply copy remaining data to destination

    .p2align 4,,7
2:  cmp         %rsi, %rdi
    jne         3f
    ret

3:
.if USE_MULTI_VERSIONING && USE_PIC
    mov         hebi_pcopy_ptr__@GOTPCREL(%rip), %r11
    mov         (%r11), %r10
.endif
    mov         %rcx, %rdx
.if USE_MULTI_VERSIONING && USE_PIC
    jmp         *%r10
.elseif USE_MULTI_VERSIONING
    jmp         *hebi_pcopy_ptr__(%rip)
.elseif USE_PIC
    jmp         hebi_pcopy@PLT
.else
    jmp         hebi_pcopy
.endif
