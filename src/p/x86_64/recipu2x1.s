# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_recipu64_2x1__(uint64_t d);

.include "src/p/x86_64/x86_64.inc"

# NOTE: It's important that hebi_recipu64_2x1__ doesn't use registers r8
# through r11, as callees will use these registers to save state instead
# of using the stack.

#-------------------------------------------------------------------------------

.extern hebi_recipu64_v0lut__
.hidden hebi_recipu64_v0lut__

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE
MVFUNC_BEGIN recipu64_2x1, x86_64, @private, @explicit

    mov         %rdi, %rax
    mov         %rdi, %rsi
    shr         $55, %rax
.ifdef HAS_PIC
    mov         hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
    movzbl      %al, %eax
    shr         $24, %rsi
.ifdef HAS_PIC
    movzwl      (%rdx,%rax,2), %eax
.else
    movzwl      hebi_recipu64_v0lut__(%rax,%rax), %eax
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
MVFUNC_DISPATCH_PTR recipu64_2x1, x86_64
.endif
