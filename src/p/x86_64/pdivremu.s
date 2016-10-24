# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_pdivremu(hebi_packet *q, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

# NOTE: Our implementation of hebi_recipu64_2x1_x86_64__ doesn't touch registers
# r8 through r11, so use these registers instead of the stack to save state.

.extern hebi_recipu64_2x1_x86_64__
.hidden hebi_recipu64_2x1_x86_64__

#-------------------------------------------------------------------------------

.if HWCAP_LZCNT
MVFUNC_BEGIN pdivremu, lzcnt

    mov         %rcx, %r9
    lzcnt       %rdx, %rcx
    mov         %rdx, %r8
    mov         %rdi, %r10
    mov         %rsi, %r11
    shl         %cl, %r8
    push        %rcx
    mov         %r8, %rdi
    call        hebi_recipu64_2x1_x86_64__
    mov         %r10, %rdi
    mov         %r11, %rsi
    lea         0(,%r9,2), %rdx
    pop         %rcx
    mov         %rax, %r9
    jmp         hebi_pdivremru64_2x1_x86_64__

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HWCAP_X86_64
MVFUNC_BEGIN pdivremu, x86_64

    mov         %rcx, %r9
    bsr         %rdx, %rcx
    mov         %rdx, %r8
    mov         %rdi, %r10
    mov         %rsi, %r11
    xor         $63, %rcx
    shl         %cl, %r8
    push        %rcx
    mov         %r8, %rdi
    call        hebi_recipu64_2x1_x86_64__
    mov         %r10, %rdi
    mov         %r11, %rsi
    lea         0(,%r9,2), %rdx
    pop         %rcx
    mov         %rax, %r9
    jmp         hebi_pdivremru64_2x1_x86_64__

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if USE_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN pdivremu

    push        %rdi
    push        %rsi
    push        %rdx
    push        %rcx
    sub         $8, %rsp
    call        hebi_hwcaps__
    add         $8, %rsp
    xor         %r10, %r10
    pop         %rcx
    pop         %rdx
    pop         %rsi
    pop         %rdi

.if HWCAP_LZCNT
    test        $hebi_hwcap_lzcnt, %eax
    jz          1f
    lea         hebi_pdivremu_lzcnt__(%rip), %r10
    BREAK
.endif

1:
.if HWCAP_X86_64
    lea         hebi_pdivremu_x86_64__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
