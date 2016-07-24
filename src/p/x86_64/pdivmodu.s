# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_word hebi_pdivmodu(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN pdivmodu, _x86_64, 16

    xor         %rax, %rax
    jrcxz       1f

    # Compute pseudo-inverse

    mov         %rdx, %r10
    dec         %rdx
    jz          8f
    push        %rbx
    lea         (,%rcx,4), %rbx
    or          $1, %rdx
    push        %r12
    bsr         %rdx, %rcx              # rcx := msb of divisor
    mov         $1, %rdx
    xor         %r8, %r8
    mov         %rdx, %r12
    shl         %cl, %rdx
    div         %r10                    # compute 2^(64+rcx) / b as pseudoinverse
    jmp         2f

    # Division by one, copy src to dst

8:  cmp         %rsi, %rdi
    je          5f
    mov         %rcx, %rdx
    MVFUNC_CALL pcopy, %r10
    xor         %rax, %rax
1:  retq

.align 16
2:  xor         $63, %cl
    push        %r13
    shl         %cl, %r12
    push        %r14
    dec         %r12
    ror         %cl, %r12               # r12 := mask
    mov         %rax, %r11              # rax := pseudoinverse

    # Division loop, compute quotient and remainder using pseudo-inverse

.align 16
3:  mov         -8(%rsi,%rbx,8), %r14
    mov         %r12, %rax
    and         %r14, %rax
    or          %r8, %rax
    rol         %cl, %rax
    mul         %r11
    mov         %r8, %r9
    mov         %r14, %r8
    rcl         $1, %rax
    rcl         $1, %rdx
    mov         %rdx, %rax
    mov         %rdx, %r13
    mul         %r10
    sub         %rax, %r8
    sbb         %rdx, %r9
    jz          5f

    # Adjust quotient and remainder on two words

4:  inc         %r13
    sub         %r10, %r8
    sbb         $0, %r9
    jnz         4b

    # Adjust quotient and remainder on one word

5:  cmp         %r10, %r8
    jl          6f
    inc         %r13
    sub         %r10, %r8
    jmp         5b

    # Store result and iterate

6:  mov         %r13, -8(%rdi,%rbx,8)
    dec         %rbx
    jnz         3b

    # Cleanup stack and return

    pop         %r14
    pop         %r13
    pop         %r12
    pop         %rbx
    mov         %r8, %rax
    retq

MVFUNC_END pdivmodu, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR pdivmodu, _x86_64

.endif
