# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

.include "config.inc"

.ifdef HAS_MULTI_VERSIONING

.extern hebi_hwcaps__
.hidden hebi_hwcaps__
.extern hebi_hwcaps_fatal__
.hidden hebi_hwcaps_fatal__

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

.macro MVFUNC_BEGIN Name:req, Version:req, Scope=@public, Access=@opaque
.macro MVFUNC_END_IMPL__
.size hebi_\Name\()_\Version\()__, .-hebi_\Name\()_\Version\()__
.endm
.text
.p2align 4,,15
.ifc \Access,@explicit
.global hebi_\Name\()_\Version\()__
.hidden hebi_\Name\()_\Version\()__
.endif
.type hebi_\Name\()_\Version\()__, @function
hebi_\Name\()_\Version\()__:
.endm

.macro MVFUNC_END
MVFUNC_END_IMPL__
.purgem MVFUNC_END_IMPL__
.endm

.ifdef HAS_PIC

.macro MVFUNC_DISPATCH_PTR Name:req, DefaultVersion:req
.text
.section .data.rel.local,"aw",@progbits
.p2align 3,,7
.global hebi_\Name\()_ptr__
.type hebi_\Name\()_ptr__, @object
.size hebi_\Name\()_ptr__, 8
hebi_\Name\()_ptr__:
.quad hebi_\Name\()_\DefaultVersion\()__
.endm

.macro MVFUNC_DISPATCH_FINALIZE_IMPL__ Name:req
.p2align 4,,7
.L\Name\()_dispatch_finalize__:
    mov     hebi_\Name\()_ptr__@GOTPCREL(%rip), %r11
    test    %r10, %r10
    jz      .L\Name\()_dispatch_fatal__
    mov     %r10, (%r11)
    jmp     *%r10
.L\Name\()_dispatch_fatal__:
    jmp     hebi_hwcaps_fatal__
.endm

.macro MVFUNC_CALL Name:req, Dst:req
    mov     hebi_\Name\()_ptr__@GOTPCREL(%rip), \Dst
    mov     (\Dst), \Dst
    call    *\Dst
.endm

.macro MVFUNC_JMP Name:req, Dst:req
    mov     hebi_\Name\()_ptr__@GOTPCREL(%rip), \Dst
    mov     (\Dst), \Dst
    jmp     *\Dst
.endm

.else # HAS_PIC

.macro MVFUNC_DISPATCH_PTR Name:req, DefaultVersion:req
.data
.p2align 3,,7
.global hebi_\Name\()_ptr__
.type hebi_\Name\()_ptr__, @object
.size hebi_\Name\()_ptr__, 8
hebi_\Name\()_ptr__:
.quad hebi_\Name\()_\DefaultVersion\()__
.endm

.macro MVFUNC_DISPATCH_FINALIZE_IMPL__ Name:req
.p2align 4,,7
.L\Name\()_dispatch_finalize__:
    test    %r10, %r10
    jz      .L\Name\()_dispatch_fatal__
    mov     %r10, hebi_\Name\()_ptr__(%rip)
    jmp     *%r10
.L\Name\()_dispatch_fatal__:
    jmp     hebi_hwcaps_fatal__
.endm

.macro MVFUNC_CALL Name:req, Dst:req
    call    *hebi_\Name\()_ptr__(%rip)
.endm

.macro MVFUNC_JMP Name:req, Dst:req
    jmp     *hebi_\Name\()_ptr__(%rip)
.endm

.endif # HAS_PIC

.macro MVFUNC_DISPATCH_BEGIN Name:req
.macro BREAK
    jmp     .L\Name\()_dispatch_finalize__
.endm
.macro BREAKZ
    jz      .L\Name\()_dispatch_finalize__
.endm
.macro MVFUNC_DISPATCH_END_IMPL__
MVFUNC_DISPATCH_FINALIZE_IMPL__ \Name
.size hebi_\Name\()_dispatch__, .-hebi_\Name\()_dispatch__
MVFUNC_DISPATCH_PTR \Name, dispatch
.endm
.text
.p2align 4,,15
.type hebi_\Name\()_dispatch__, @function
hebi_\Name\()_dispatch__:
.endm

.macro MVFUNC_DISPATCH_END
MVFUNC_DISPATCH_END_IMPL__
.purgem BREAK
.purgem BREAKZ
.purgem MVFUNC_DISPATCH_END_IMPL__
.endm

.macro MVFUNC_CALL_EXPLICIT Name:req, Version:req
    call    hebi_\Name\()_\Version\()__
.endm

.macro MVFUNC_JMP_EXPLICIT Name:req, Version:req
    jmp     hebi_\Name\()_\Version\()__
.endm

.else # HAS_MULTI_VERSIONING

.macro MVFUNC_BEGIN_OPAQUE__ Name:req, Scope:req
.macro MVFUNC_END_IMPL__
.size \Name, .-\Name
.endm
.text
.p2align 4,,15
.global \Name
.ifc \Scope,@private
.hidden \Name
.endif
.type \Name, @function
\Name\():
.endm

.macro MVFUNC_BEGIN_EXPLICIT__ Name:req, ExplicitName:req, Scope:req
.macro MVFUNC_END_IMPL__
.size \Name, .-\Name
.size \ExplicitName, .-\ExplicitName
.endm
.text
.p2align 4,,15
.global \Name
.ifc \Scope,@private
.hidden \Name
.endif
.global \ExplicitName
.hidden \ExplicitName
.type \Name, @function
.type \ExplicitName, @function
\Name\():
\ExplicitName\():
.endm

.macro MVFUNC_BEGIN Name:req, Version:req, Scope=@public, Access=@opaque
.ifc \Access,@opaque
.ifc \Scope,@public
MVFUNC_BEGIN_OPAQUE__ hebi_\Name, @public
.endif
.ifc \Scope,@private
MVFUNC_BEGIN_OPAQUE__ hebi_\Name\()__, @private
.endif
.endif
.ifc \Access,@explicit
.ifc \Scope,@public
MVFUNC_BEGIN_EXPLICIT__ hebi_\Name, hebi_\Name\()_\Version\()__, @public
.endif
.ifc \Scope,@private
MVFUNC_BEGIN_EXPLICIT__ hebi_\Name\()__, hebi_\Name\()_\Version\()__, @private
.endif
.endif
.endm

.macro MVFUNC_END
MVFUNC_END_IMPL__
.purgem MVFUNC_END_IMPL__
.equ HAS_HWCAP_X86_64,      0
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

.ifdef HAS_PIC

.macro MVFUNC_CALL Name, Dest
    call    hebi_\Name\()@PLT
.endm

.macro MVFUNC_JMP Name, Dest
    jmp     hebi_\Name\()@PLT
.endm

.else

.macro MVFUNC_CALL Name, Dest
    call    hebi_\Name
.endm

.macro MVFUNC_JMP Name, Dest
    jmp     hebi_\Name
.endm

.endif

.macro MVFUNC_CALL_EXPLICIT Name:req, Version:req
    call    hebi_\Name\()_\Version\()__
.endm

.macro MVFUNC_JMP_EXPLICIT Name:req, Version:req
    jmp     hebi_\Name\()_\Version\()__
.endm

.endif # HAS_MULTI_VERSIONING
