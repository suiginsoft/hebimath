# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_recipu64_3x2__(uint64_t d1, uint64_t d0);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

# NOTE: Our implementation of hebi_recipu64_2x1_x86_64__ doesn't touch registers
# r8 through r11, so use these registers instead of the stack to save state.

.extern hebi_recipu64_2x1_x86_64__
.hidden hebi_recipu64_2x1_x86_64__

#-------------------------------------------------------------------------------

.if HWCAP_BMI2
MVFUNC_BEGIN recipu64_3x2, bmi2, @private

    mov         %rdi, %r10
    mov         %rsi, %r11
    call        hebi_recipu64_2x1_x86_64__
    mov         %r10, %rdx
    imul        %rax, %rdx
    add         %r11, %rdx
    mov         %rdx, %r8
    jnc         2f
    cmp         %rdx, %r10
    jbe         3f
    dec         %rax
1:  sub         %r10, %r8
2:  mov         %rax, %rdx
    mulx        %r11, %rsi, %rdi
    mov         %r8, %rdx
    add         %rdi, %rdx
    jc          4f
    ret

    .p2align 4,,7
3:  sub         $2, %rax
    sub         %r10, %r8
    jmp         1b

    .p2align 4,,7
4:  cmp         %rdx, %r10
    jb          5f
    jne         6f
    cmp         %rsi, %r11
    ja          6f
5:  sub         $2, %rax
    ret

    .p2align 4,,7
6:  dec         %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_X86_64
MVFUNC_BEGIN recipu64_3x2, x86_64, @private

    mov         %rdi, %r8
    mov         %rsi, %r9
    call        hebi_recipu64_2x1_x86_64__
    mov         %r8, %rcx
    imul        %rax, %rcx
    mov         %rax, %rdi
    add         %r9, %rcx
    jnc         2f
    cmp         %rcx, %r8
    jbe         3f
    dec         %rdi
1:  sub         %r8, %rcx
2:  mov         %rdi, %rax
    mul         %r9
    add         %rdx, %rcx
    jc          4f
    mov         %rdi, %rax
    ret

    .p2align 4,,7
3:  sub         $2, %rdi
    sub         %r8, %rcx
    jmp         1b

    .p2align 4,,7
4:  cmp         %rcx, %r8
    jb          5f
    jne         6f
    cmp         %rax, %r9
    ja          6f
5:  lea         -2(,%rdi), %rax
    ret

    .p2align 4,,7
6:  lea         -1(,%rdi), %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN recipu64_3x2

    push        %rdi
    push        %rsi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rsi
    pop         %rdi

.if HWCAP_BMI2
    test        $hebi_hwcap_bmi2, %eax
    jz          1f
    lea         hebi_recipu64_3x2_bmi2__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_X86_64
    lea         hebi_recipu64_3x2_x86_64__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
