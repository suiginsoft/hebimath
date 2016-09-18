# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_recipu64_3x2__(uint64_t d1, uint64_t d0);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.if HAS_HWCAP_BMI2
.extern hebi_recipu64_2x1_bmi2__
.hidden hebi_recipu64_2x1_bmi2__
MVFUNC_BEGIN recipu64_3x2, bmi2, @private, @explicit

    push        %rbp
    push        %rbx
    mov         %rdi, %rbx
    mov         %rsi, %rbp
    sub         $8, %rsp
    call        hebi_recipu64_2x1_bmi2__
    mov         %rbx, %rdx
    imul        %rax, %rdx
    add         $8, %rsp
    add         %rbp, %rdx
    mov         %rdx, %r8
    jnc         2f
    cmp         %rdx, %rbx
    jbe         3f
    dec         %rax
1:  sub         %rbx, %r8
2:  mov         %rax, %rdx
    mulx        %rbp, %rsi, %rdi
    mov         %r8, %rdx
    add         %rdi, %rdx
    jc          4f
    pop         %rbx
    pop         %rbp
    ret

    .p2align 4,,7
3:  sub         $2, %rax
    sub         %rbx, %r8
    jmp         1b
4:  cmp         %rdx, %rbx
    jb          5f
    jne         6f
    cmp         %rsi, %rbp
    ja          6f
5:  sub         $2, %rax
    pop         %rbx
    pop         %rbp
    ret

    .p2align 4,,7
6:  dec         %rax
    pop         %rbx
    pop         %rbp
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_X86_64
.extern hebi_recipu64_2x1_x86_64__
.hidden hebi_recipu64_2x1_x86_64__
MVFUNC_BEGIN recipu64_3x2, x86_64, @private, @explicit

    push        %rbp
    push        %rbx
    mov         %rsi, %rbp
    mov         %rdi, %rbx
    sub         $8, %rsp
    call        hebi_recipu64_2x1_x86_64__
    mov         %rbx, %rcx
    imul        %rax, %rcx
    add         $8, %rsp
    mov         %rax, %rdi
    add         %rbp, %rcx
    jnc         2f
    cmp         %rcx, %rbx
    jbe         3f
    dec         %rdi
1:  sub         %rbx, %rcx
2:  mov         %rdi, %rax
    mul         %rbp
    add         %rdx, %rcx
    jc          4f
    mov         %rdi, %rax
    pop         %rbx
    pop         %rbp
    ret

    .p2align 4,,7
3:  sub         $2, %rdi
    sub         %rbx, %rcx
    jmp         1b
4:  cmp         %rcx, %rbx
    jb          5f
    jne         6f
    cmp         %rax, %rbp
    ja          6f
5:  sub         $2, %rdi
    pop         %rbx
    mov         %rdi, %rax
    pop         %rbp
    ret

    .p2align 4,,7
6:  dec         %rdi
    pop         %rbx
    mov         %rdi, %rax
    pop         %rbp
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN recipu64_3x2

    push        %rdi
    push        %rsi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rsi
    pop         %rdi

.if HAS_HWCAP_BMI2
    test        $hebi_hwcap_bmi2, %eax
    jz          1f
    lea         recipu64_3x2_bmi2__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_X86_64
    lea         recipu64_3x2_x86_64__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
