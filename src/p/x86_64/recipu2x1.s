# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_recipu64_2x1__(uint64_t d);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.extern hebi_recipu64_v0lut__
.hidden hebi_recipu64_v0lut__

#-------------------------------------------------------------------------------

.if HAS_HWCAP_BMI2
MVFUNC_BEGIN recipu64_2x1, bmi2, @private, @explicit

#
# TODO: optimize how we compute d63 and mask values
#

.ifdef HAS_PIC
    mov         hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
    mov         %rdi, %rax
    mov         %rdi, %rsi
    mov         %rdi, %rcx
    shr         $55, %rax
    shr         $24, %rsi
    and         $1, %ecx
    movzb       %al, %eax
    inc         %rsi
    mov         %rcx, %r9
.ifdef HAS_PIC
    movzw       (%rdx,%rax,2), %eax
.else
    movzw       hebi_recipu64_v0lut__(%rax,%rax), %eax
.endif
    neg         %r9
    mov         %eax, %edx
    imul        %eax, %eax
    shl         $11, %edx
    dec         %rdx
    imul        %rsi, %rax
    shr         $40, %rax
    sub         %rax, %rdx
    movabs      $1152921504606846976, %rax
    imul        %rdx, %rsi
    sub         %rsi, %rax
    imul        %rdx, %rax
    shl         $13, %rdx
    shr         $47, %rax
    add         %rdx, %rax
    mov         %rdi, %rdx
    shr         %rdx
    and         %rax, %r9
    add         %rdx, %rcx
    shr         %r9
    imul        %rax, %rcx
    sub         %rcx, %r9
    mov         %r9, %rdx
    mulx        %rax, %r9, %r10
    shl         $31, %rax
    mov         %r10, %rdx
    shr         %rdx
    add         %rdx, %rax
    mov         %rax, %rdx
    sub         %rdi, %rax
    mulx        %rdi, %r9, %r10
    add         %r9, %rdi
    sbb         %r10, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE
MVFUNC_BEGIN recipu64_2x1, x86_64, @private, @explicit

    mov         %rdi, %rax
    mov         %rdi, %rsi
    shr         $55, %rax
.ifdef HAS_PIC
    mov         hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
    movzb       %al, %eax
    shr         $24, %rsi
.ifdef HAS_PIC
    movzw       (%rdx,%rax,2), %eax
.else
    movzw       hebi_recipu64_v0lut__(%rax,%rax), %eax
.endif
    inc         %rsi
    mov         %eax, %ecx
    imul        %eax, %eax
    shl         $11, %ecx
    dec         %rcx
    imul        %rsi, %rax
    shr         $40, %rax
    sub         %rax, %rcx
    movabs      $1152921504606846976, %rax
    imul        %rcx, %rsi
    sub         %rsi, %rax
    mov         %rdi, %rsi
    imul        %rcx, %rax
    shl         $13, %rcx
    shr         $47, %rax
    add         %rax, %rcx
    shr         %rsi
    sbb         %rax, %rax
    sub         %rax, %rsi
    imul        %rcx, %rsi
    and         %rcx, %rax
    shr         %rax
    sub         %rsi, %rax
    mul         %rcx
    shl         $31, %rcx
    shr         %rdx
    add         %rdx, %rcx
    mov         %rdi, %rax
    mul         %rcx
    add         %rdi, %rax
    mov         %rcx, %rax
    adc         %rdi, %rdx
    sub         %rdx, %rax
    ret

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN recipu64_2x1

    push        %rdi
    call        hebi_hwcaps__
    xor         %r10, %r10
    pop         %rdi

.if HAS_HWCAP_BMI2
    test        $hebi_hwcap_bmi2, %eax
    jz          1f
    lea         hebi_recipu64_2x1_bmi2__(%rip), %r10
    BREAK
.endif

1:
.if HAS_HWCAP_X86_64
    lea         hebi_recipu64_2x1_x86_64__(%rip), %r10
.endif

MVFUNC_DISPATCH_END
.endif
