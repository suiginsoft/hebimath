# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "config.inc"

.ifdef HAS_MULTI_VERSIONING

.extern hebi_hwcaps__
.extern hebi_hwcaps_fatal__

.ifdef HAS_PIC
.hidden hebi_hwcaps__
.hidden hebi_hwcaps_fatal__
.endif

.equ hebi_hwcap_sse,        0x00000001
.equ hebi_hwcap_sse2,       0x00000002
.equ hebi_hwcap_sse3,       0x00000004
.equ hebi_hwcap_ssse3,      0x00000008
.equ hebi_hwcap_sse41,      0x00000010
.equ hebi_hwcap_sse42,      0x00000020
.equ hebi_hwcap_aesni,      0x00000040
.equ hebi_hwcap_clmul,      0x00000080
.equ hebi_hwcap_popcnt,     0x00000100
.equ hebi_hwcap_lzcnt,      0x00000200
.equ hebi_hwcap_f16c,       0x00000400
.equ hebi_hwcap_fma,        0x00000800
.equ hebi_hwcap_avx,        0x00001000
.equ hebi_hwcap_avx2,       0x00002000
.equ hebi_hwcap_bmi,        0x00004000
.equ hebi_hwcap_bmi2,       0x00008000
.equ hebi_hwcap_ermsb,      0x00010000
.equ hebi_hwcap_adx,        0x00020000
.equ hebi_hwcap_sha,        0x00040000
.equ hebi_hwcap_avx512f,    0x00080000
.equ hebi_hwcap_avx512bw,   0x00100000
.equ hebi_hwcap_avx512cd,   0x00200000
.equ hebi_hwcap_avx512dq,   0x00400000
.equ hebi_hwcap_avx512er,   0x00800000
.equ hebi_hwcap_avx512pf,   0x01000000
.equ hebi_hwcap_avx512vl,   0x02000000
.equ hebi_hwcap_avx512ifma, 0x04000000
.equ hebi_hwcap_avx512vbmi, 0x08000000

.macro MVFUNC_BEGIN Name, Suffix, Alignment
	.text
	.align \Alignment, 0x90
	.type \Name\Suffix, @function
\Name\Suffix:
.endm

.macro MVFUNC_END Name, Suffix
	.size \Name\Suffix, .-\Name\Suffix
.endm

.ifdef HAS_PIC

.macro MVFUNC_PTR Name, DefaultSuffix
	.text
	.global hebi_\Name\()_ptr__
	.section .data.rel.local,"aw",@progbits
	.align 8
	.type hebi_\Name\()_ptr__, @object
	.size hebi_\Name\()_ptr__, 8
hebi_\Name\()_ptr__:
	.quad \Name\DefaultSuffix
.endm

.macro MVFUNC_CALL Name, Dest
	movq	hebi_\Name\()_ptr__@GOTPCREL(%rip), \Dest
	movq	(\Dest), \Dest
	call	*\Dest
.endm

.macro MVFUNC_JMP Name, Dest
	movq	hebi_\Name\()_ptr__@GOTPCREL(%rip), \Dest
	movq	(\Dest), \Dest
	jmpq	*\Dest
.endm

.macro MVFUNC_USE Name, Src, Temp
	movq	hebi_\Name\()_ptr__@GOTPCREL(%rip), \Temp
	movq	\Src, (\Temp)
	jmpq	*\Src
.endm

.else # HAS_PIC

.macro MVFUNC_PTR Name, DefaultSuffix
	.data
	.global hebi_\Name\()_ptr__
	.align 8
	.type hebi_\Name\()_ptr__, @object
	.size hebi_\Name\()_ptr__, 8
hebi_\Name\()_ptr__:
	.quad \Name\DefaultSuffix
.endm

.macro MVFUNC_CALL Name, Dest
	call	*hebi_\Name\()_ptr__(%rip)
.endm

.macro MVFUNC_JMP Name, Dest
	jmp		*hebi_\Name\()_ptr__(%rip)
.endm

.macro MVFUNC_USE Name, Src, Temp
	movq	\Src, hebi_\Name\()_ptr__(%rip)
	jmpq	*\Src
.endm

.endif # HAS_PIC

.else # HAS_MULTI_VERSIONING

.macro MVFUNC_BEGIN Name, Suffix, Alignment
	.text
	.align \Alignment, 0x90
	.global hebi_\Name
	.type hebi_\Name, @function
hebi_\Name:
.endm

.macro MVFUNC_END Name, Suffix
	.size hebi_\Name, .-hebi_\Name
	.equ HAS_HWCAP_SSE,         0
	.equ HAS_HWCAP_SSE2,        0
	.equ HAS_HWCAP_SSE3,        0
	.equ HAS_HWCAP_SSE41,       0
	.equ HAS_HWCAP_SSE42,       0
	.equ HAS_HWCAP_AESNI,       0
	.equ HAS_HWCAP_CLMUL,       0
	.equ HAS_HWCAP_POPCNT,      0
	.equ HAS_HWCAP_LZCNT,       0
	.equ HAS_HWCAP_F16C,        0
	.equ HAS_HWCAP_FMA,         0
	.equ HAS_HWCAP_AVX,         0
	.equ HAS_HWCAP_AVX2,        0
	.equ HAS_HWCAP_BMI,         0
	.equ HAS_HWCAP_BMI2,        0
	.equ HAS_HWCAP_ERMSB,       0
	.equ HAS_HWCAP_ADX,         0
	.equ HAS_HWCAP_SHA,         0
	.equ HAS_HWCAP_AVX512F,     0
	.equ HAS_HWCAP_AVX512BW,    0
	.equ HAS_HWCAP_AVX512CD,    0
	.equ HAS_HWCAP_AVX512DQ,    0
	.equ HAS_HWCAP_AVX512ER,    0
	.equ HAS_HWCAP_AVX512PF,    0
	.equ HAS_HWCAP_AVX512VL,    0
	.equ HAS_HWCAP_AVX512IFMA,  0
	.equ HAS_HWCAP_AVX512VBMI,  0
.endm

.macro MVFUNC_PTR Name, DefaultSuffix
.endm

.macro MVFUNC_CALL Name, Dest
	call	hebi_\Name
.endm

.macro MVFUNC_JMP Name, Dest
	jmp		hebi_\Name
.endm

.macro MVFUNC_USE Name, Src, Temp
	jmpq	*\Src
.endm

.endif # HAS_MULTI_VERSIONING
