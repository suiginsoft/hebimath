# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro PADDSUB Name, Propagate, AdcSbb
FUNC_EXTERN \Propagate, @private
FUNC_BEGIN \Name

    # Setup for main loop and tail

    sub         %r8, %rcx
    mov         %r8, %rax
    mov         %rcx, %r9
    shr         %r8
    and         $1, %rcx
    shr         %r9
    push        %rcx
    and         $1, %rax
    mov         %r8, %rcx
    push        %r9
    bt          $0, %edi            # address is aligned, lowest bit is zero
    jrcxz       2f

    # Main loop: process 4x quadwords per iteration

    .p2align 4,,15
1:  mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    \AdcSbb     (%rdx), %r8
    \AdcSbb     8(%rdx), %r9
    \AdcSbb     16(%rdx), %r10
    \AdcSbb     24(%rdx), %r11
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    mov         %r10, 16(%rdi)
    mov         %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdx), %rdx
    lea         32(%rdi), %rdi
    dec         %rcx
    jnz         1b

    # Tail: process last packet if there's an odd number of packets

    .p2align 4,,7
2:  dec         %rax
    jnz         3f
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    \AdcSbb     (%rdx), %r8
    \AdcSbb     8(%rdx), %r9
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    lea         16(%rsi), %rsi
    lea         16(%rdi), %rdi

    # Propagate first operand and carry/borrow into remainder of destination

    .p2align 4,,7
3:  pop         %rcx
    pop         %rdx
    mov         $0, %eax
    jmp         hebi_\Propagate\()__

FUNC_END
.endm

#-------------------------------------------------------------------------------

.macro PADDSUBU Name, Propagate, AddSub, AdcSbb
FUNC_EXTERN \Propagate, @private
FUNC_BEGIN \Name

    # Add/subtract word to/from first packet, propagating carry/borrow

    dec         %rcx
    xor         %eax, %eax
    mov         %rcx, %r11
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    shr         %rcx
    and         $1, %r11
    \AddSub     %rdx, %r8
    \AdcSbb     %rax, %r9
    mov         %r11, %rdx
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    lea         16(%rsi), %rsi
    lea         16(%rdi), %rdi
    jmp         hebi_\Propagate\()__

FUNC_END
.endm

#-------------------------------------------------------------------------------

.macro PADDSUBC Name, AdcSbb
FUNC_BEGIN \Name, @private

    jrcxz       2f

    # Propagation main loop: propagate carry/borrow into remaining packets

1:  jnc         4f
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    \AdcSbb     %rax, %r8
    \AdcSbb     %rax, %r9
    \AdcSbb     %rax, %r10
    \AdcSbb     %rax, %r11
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
    mov         %r10, 16(%rdi)
    mov         %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdi), %rdi
    dec         %rcx
    jnz         1b

    # Propagation end: propagate carry/borrow into last packet

    .p2align 4,,7
2:  jnc         4f
    dec         %rdx
    jnz         3f
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    \AdcSbb     %rax, %r8
    \AdcSbb     %rax, %r9
    mov         %r8, (%rdi)
    mov         %r9, 8(%rdi)
3:  setc        %al
    ret

    # Carry no longer set, simply copy remaining data to destination

    .p2align 4,,7
4:
.if USE_MULTI_VERSIONING && USE_PIC
    mov         hebi_pcopy_ptr__@GOTPCREL(%rip), %r11
.endif
    lea         (%rdx,%rcx,2), %rdx
.if USE_MULTI_VERSIONING && USE_PIC
    mov         (%r11), %r10
.endif
    test        %rdx, %rdx
    jz          5f
.if USE_MULTI_VERSIONING && USE_PIC
    jmp         *%r10
.elseif USE_MULTI_VERSIONING
    jmp         *hebi_pcopy_ptr__(%rip)
.elseif USE_PIC
    jmp         hebi_pcopy@PLT
.else
    jmp         hebi_pcopy
.endif
5:  ret

FUNC_END
.endm

#-------------------------------------------------------------------------------

.macro PADDSUBA Name, Propagate, AdcSbb
FUNC_EXTERN \Propagate, @private
FUNC_BEGIN \Name

    # Setup for main loop and tail

    sub         %rcx, %rdx
    mov         %rcx, %rax
    mov         %rdx, %r8
    shr         %rcx
    and         $1, %rdx
    shr         %r8
    push        %rdx
    and         $1, %rax
    push        %r8
    bt          $0, %edi            # address is aligned, lowest bit is zero
    jrcxz       2f

    # Main loop: process 4x quadwords per iteration

    .p2align 4,,15
1:  mov         (%rsi), %r8
    mov         8(%rsi), %r9
    mov         16(%rsi), %r10
    mov         24(%rsi), %r11
    \AdcSbb     %r8, (%rdi)
    \AdcSbb     %r9, 8(%rdi)
    \AdcSbb     %r10, 16(%rdi)
    \AdcSbb     %r11, 24(%rdi)
    lea         32(%rsi), %rsi
    lea         32(%rdi), %rdi
    dec         %rcx
    jnz         1b

    # Tail: process last packet if there's an odd number of packets

    .p2align 4,,7
2:  dec         %rax
    jnz         3f
    mov         (%rsi), %r8
    mov         8(%rsi), %r9
    \AdcSbb     %r8, (%rdi)
    \AdcSbb     %r9, 8(%rdi)
    lea         16(%rdi), %rdi

    # Propagate first operand and carry/borrow into remainder of destination

    .p2align 4,,7
3:  pop         %rcx
    pop         %rdx
    mov         $0, %eax
    jmp         hebi_\Propagate\()__

FUNC_END
.endm

#-------------------------------------------------------------------------------

.macro PADDSUBAC Name, AdcSbb
FUNC_BEGIN \Name, @private

    jrcxz       2f

    # Propagation main loop: propagate carry/borrow into remaining packets

1:  jnc         4f
    \AdcSbb     %rax, (%rdi)
    \AdcSbb     %rax, 8(%rdi)
    \AdcSbb     %rax, 16(%rdi)
    \AdcSbb     %rax, 24(%rdi)
    lea         32(%rdi), %rdi
    dec         %rcx
    jnz         1b

    # Propagation end: propagate carry/borrow into last packet

    .p2align 4,,7
2:  jnc         4f
    dec         %rdx
    jnz         3f
    \AdcSbb     %rax, (%rdi)
    \AdcSbb     %rax, 8(%rdi)
3:  setc        %al
4:  ret

FUNC_END
.endm
