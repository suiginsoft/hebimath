# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# void
# hebi_pmove(hebi_packet *r, const hebi_packet *a, size_t n);

# NOTE: hebi_pmove must not modify the rax, r8, r9 or xmm4-xmm15 registers, as
# other x86-64 driver functions rely on these registers being preserved

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro PMOVE128 Suffix, MoveOp

.if USE_PIC
    .text
.else
    .section .rodata,"a",@progbits
.endif
    .p2align 4,,15
.Lpmove_\Suffix\()_jump_table:
    .quad .Lpmove_\Suffix\()_1x
    .quad .Lpmove_\Suffix\()_1x
    .quad .Lpmove_\Suffix\()_2x
    .quad .Lpmove_\Suffix\()_3x

MVFUNC_BEGIN pmove, \Suffix, @public, @explicit

    cmp         $4, %rdx
    jae         1f
.if USE_PIC
    lea         .Lpmove_\Suffix\()_jump_table(%rip), %rcx
    mov         (%rcx,%rdx,8), %rcx
    jmp         *%rcx
.else
    jmp         *.Lpmove_\Suffix\()_jump_table(,%rdx,8)
.endif
1:  mov         %rax, %r11
    mov         %rdx, %rcx
    mov         %rdx, %r10
    shr         $2, %rcx
    mov         $64, %rax
    cmp         %rsi, %rdi
    jbe         2f
    shl         $4, %rdx
    neg         %rax
    lea         -64(%rsi,%rdx), %rsi
    lea         -64(%rdi,%rdx), %rdi

    .p2align 4,,15
2:  \MoveOp     (%rsi), %xmm0
    \MoveOp     16(%rsi), %xmm1
    \MoveOp     32(%rsi), %xmm2
    \MoveOp     48(%rsi), %xmm3
    \MoveOp     %xmm0, (%rdi)
    \MoveOp     %xmm1, 16(%rdi)
    \MoveOp     %xmm2, 32(%rdi)
    \MoveOp     %xmm3, 48(%rdi)
    add         %rax, %rsi
    add         %rax, %rdi
    dec         %rcx
    jnz         2b

    and         $3, %r10
    jz          3f
    lea         (,%r10,8), %rcx
    cqo
    lea         (%rax,%rcx,2), %rax
    and         %rax, %rdx
.if USE_PIC
    lea         .Lpmove_\Suffix\()_jump_table(%rip), %rax
    mov         (%rax,%rcx), %rcx
.endif
    neg         %rdx
    mov         %r11, %rax
    lea         (%rsi,%rdx), %rsi
    lea         (%rdi,%rdx), %rdi
.if USE_PIC
    jmp         *%rcx
.else
    jmp         *.Lpmove_\Suffix\()_jump_table(%rcx)
.endif
3:  mov         %r11, %rax
    ret

    .p2align 4,,15
.Lpmove_\Suffix\()_1x:
    \MoveOp     (%rsi), %xmm0
    \MoveOp     %xmm0, (%rdi)
    ret

    .p2align 4,,15
.Lpmove_\Suffix\()_2x:
    \MoveOp     (%rsi), %xmm0
    \MoveOp     16(%rsi), %xmm1
    \MoveOp     %xmm0, (%rdi)
    \MoveOp     %xmm1, 16(%rdi)
    ret

    .p2align 4,,15
.Lpmove_\Suffix\()_3x:
    \MoveOp     (%rsi), %xmm0
    \MoveOp     16(%rsi), %xmm1
    \MoveOp     32(%rsi), %xmm2
    \MoveOp     %xmm0, (%rdi)
    \MoveOp     %xmm1, 16(%rdi)
    \MoveOp     %xmm2, 32(%rdi)
    ret

MVFUNC_END

.endm

#-------------------------------------------------------------------------------

.if HWCAP_AVX
PMOVE128 avx, vmovdqu
.endif

#-------------------------------------------------------------------------------

.if HWCAP_SSE2
PMOVE128 sse2, movdqu
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pmove

    push        %rax
    push        %rdx
    push        %rsi
    push        %rdi
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rdi
    pop         %rsi
    mov         %eax, %ecx
    pop         %rdx
    pop         %rax

.if HWCAP_AVX
    test        $hebi_hwcap_avx, %ecx
    jz          1f
    lea         hebi_pmove_avx__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_SSE2
    test        $hebi_hwcap_sse2, %ecx
    BREAKZ
    lea         hebi_pmove_sse2__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
