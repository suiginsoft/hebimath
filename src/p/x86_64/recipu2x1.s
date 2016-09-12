# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_recipu64_2x1__(uint64_t d);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.extern hebi_recipu64_v0lut__

.ifdef HAS_PIC
.hidden hebi_recipu64_v0lut__
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_ADX || HAS_HWCAP_BMI2
MVFUNC_BEGIN recipu64_2x1__, _adx_bmi2, 16

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
	retq

MVFUNC_END recipu64_2x1__, _adx_bmi2
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE
MVFUNC_BEGIN recipu64_2x1__, _x86_64, 16

.ifdef HAS_PIC
	mov         hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
	mov         %rdi, %rax
	mov         %rdi, %rsi
	mov         %rdi, %rdx
	shr         $55, %rax
	shr         $24, %rsi
	and         $1, %edx
	movzb       %al, %eax
	inc         %rsi
.ifdef HAS_PIC
	movzw       (%rdx,%rax,2), %eax
.else
	movzw       hebi_recipu64_v0lut__(%rax,%rax), %eax
.endif
		
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
	shr         %rsi
	shl         $13, %rcx
	shr         $47, %rax
	add         %rax, %rcx
	mov         %rdx, %rax
	add         %rsi, %rdx
	imul        %rcx, %rdx
	neg         %rax
	and         %rcx, %rax
	shr         %rax
	sub         %rdx, %rax
	mul         %rcx
	shl         $31, %rcx
	mov         %rdx, %rax
	shr         %rax
	add         %rax, %rcx
	mov         %rcx, %rax
	sub         %rdi, %rcx
	mul         %rdi
	mov         %rax, %rsi
	add         %rsi, %rdi
	sbb         %rdx, %rcx
	mov         %rcx, %rax
	retq

MVFUNC_END recipu64_2x1__, _x86_64
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

.text
.align 16, 0x90
.type recipu64_2x1_select, @function
recipu64_2x1_select:

	pushq       %rdi
	call        hebi_hwcaps__
	popq        %rdi
	lea         rercipu64_2x1___x86_64(%rip), %r10

.if HAS_HWCAP_ADX || HAS_HWCAP_BMI2
	test        $(hebi_hwcap_adx+hebi_hwcap_bmi2), %eax
	jz          1f
	lea         recipu64_2x1___adx_bmi2(%rip), %r10
.endif

1:	MVFUNC_USE  recipu64_2x1, %r10, %rax

.size recipu64_2x1_select, .-recipu64_2x1_select

MVFUNC_PTR recipu64_2x1, _select

.endif
