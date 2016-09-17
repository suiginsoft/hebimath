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

.ifdef HAS_PIC
	movq        hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
	movq        %rdi, %rax
	movq        %rdi, %rsi
	movq        %rdi, %rcx
	shrq        $55, %rax
	shrq        $24, %rsi
	andl        $1, %ecx
	movzbl      %al, %eax
	incq        %rsi
	movq        %rcx, %r9
.ifdef HAS_PIC
	movzwl      (%rdx,%rax,2), %eax
.else
	movzwl      hebi_recipu64_v0lut__(%rax,%rax), %eax
.endif
	negq        %r9
	movl        %eax, %edx
	imull       %eax, %eax
	shll        $11, %edx
	decq        %rdx
	imulq       %rsi, %rax
	shrq        $40, %rax
	subq        %rax, %rdx
	movabsq     $1152921504606846976, %rax
	imulq       %rdx, %rsi
	subq        %rsi, %rax
	imulq       %rdx, %rax
	shlq        $13, %rdx
	shrq        $47, %rax
	addq        %rdx, %rax
	movq        %rdi, %rdx
	shrq        %rdx
	andq        %rax, %r9
	addq        %rdx, %rcx
	shrq        %r9
	imulq       %rax, %rcx
	subq        %rcx, %r9
	movq        %r9, %rdx
	mulxq       %rax, %r9, %r10
	shlq        $31, %rax
	movq        %r10, %rdx
	shrq        %rdx
	addq        %rdx, %rax
	movq        %rax, %rdx
	subq        %rdi, %rax
	mulxq       %rdi, %r9, %r10
	addq        %r9, %rdi
	sbbq        %r10, %rax
	retq

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.if HAS_HWCAP_SSE
MVFUNC_BEGIN recipu64_2x1, x86_64, @private, @explicit

.ifdef HAS_PIC
	movq        hebi_recipu64_v0lut__@GOTPCREL(%rip), %rdx
.endif
	movq        %rdi, %rax
	movq        %rdi, %rsi
	movq        %rdi, %rdx
	shrq        $55, %rax
	shrq        $24, %rsi
	andl        $1, %edx
	movzbl      %al, %eax
	incq        %rsi
.ifdef HAS_PIC
	movzwl      (%rdx,%rax,2), %eax
.else
	movzwl      hebi_recipu64_v0lut__(%rax,%rax), %eax
.endif
	movl        %eax, %ecx
	imull       %eax, %eax
	shll        $11, %ecx
	decq        %rcx
	imulq       %rsi, %rax
	shrq        $40, %rax
	subq        %rax, %rcx
	movabsq     $1152921504606846976, %rax
	imulq       %rcx, %rsi
	subq        %rsi, %rax
	movq        %rdi, %rsi
	imulq       %rcx, %rax
	shrq        %rsi
	shlq        $13, %rcx
	shrq        $47, %rax
	addq        %rax, %rcx
	movq        %rdx, %rax
	addq        %rsi, %rdx
	imulq       %rcx, %rdx
	negq        %rax
	andq        %rcx, %rax
	shrq        %rax
	subq        %rdx, %rax
	mulq        %rcx
	shlq        $31, %rcx
	movq        %rdx, %rax
	shrq        %rax
	addq        %rax, %rcx
	movq        %rcx, %rax
	subq        %rdi, %rcx
	mulq        %rdi
	movq        %rax, %rsi
	addq        %rsi, %rdi
	sbbq        %rdx, %rcx
	movq        %rcx, %rax
	retq

MVFUNC_END
.endif

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_BEGIN recipu64_2x1

	pushq       %rdi
	call        hebi_hwcaps__
	popq        %rdi
	leaq        hebi_recipu64_2x1_x86_64__(%rip), %r10

.if HAS_HWCAP_BMI2
	testl       $(hebi_hwcap_bmi2), %eax
	jz          1f
	leaq        hebi_recipu64_2x1_bmi2__(%rip), %r10
.endif

1:	MVFUNC_USE  %r10

MVFUNC_DISPATCH_END
.endif
