# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# size_t hebi_pdivmod_norm(
#         hebi_packet *restrict q,
#         hebi_packet *restrict u,
#         const hebi_packet *restrict v,
#         size_t m,
#         size_t n );

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

MVFUNC_BEGIN pdivmod_norm, _x86_64, 16

    mov         %rcx, %r9
    sub         %r8, %rcx

    mov         (%rbp), %r10
    xor         %rax, %rax
    dec         %rax
    mov         %rax, %rdx
    sub         %r10, %rdx
    div         %r10
    mov         %rax, %r11

    mov         %rdx, %r10
    xor         %rcx, %rax
    jrcxz       1f
    dec         %rdx
    lea         (,%rcx,4), %r8
    or          $1, %rdx
    bsr         %rdx, %rcx
    push        %r12
    mov         $1, %rdx
    push        %rbx
    mov         %rdx, %r12
    shl         %cl, %rdx
    mov         %r8, %rbx
    div         %r10
    neg         %cl
    push        %r13
    add         $63, %cl
    xor         %r8, %r8
    shl         %cl, %r12
    push        %r14
    dec         %r12
    ror         %cl, %r12
    mov         %rax, %r11
    jmp         2f
1:  retq

.align 16
2:  mov         -8(%rsi,%rbx,8), %r14
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
    jz          4f

3:  inc         %r13
    sub         %r10, %r8
    sbb         $0, %r9
    jnz         3b

4:  inc         %r13
    sub         %r10, %r8
    jae         4b

    dec         %r13
    add         %r10, %r8
    mov         %r13, -8(%rdi,%rbx,8)
    dec         %rbx
    jnz         2b

    pop         %r14
    pop         %r13
    pop         %rbx
    pop         %r12
    mov         %r8, %rax
    retq

MVFUNC_END pdivmod_norm, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR pdivmod_norm, _x86_64

.endif
