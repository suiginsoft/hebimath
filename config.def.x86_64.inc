# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# Configure the hardware capabilities of your x86 or x86-64 CPU below.
# Set each hardware capability (hwcap) symbol to 1 to enable it, or 0
# to disable it. Consult your processor documentation or use a utility
# such as lscpu(1) to determine what features your processor supports.
#
# With dynamic multi-versioning (the default), the hardware capability
# symbols below control what function versions are built into the
# library and are available for selection at runtime.
#
# With static multi-versioning, the symbols control the feature set
# available for the target hardware, and only the best function
# matching the enabled features is linked in. All other functions are
# stripped out of the library.

.equ HAS_HWCAP_SSE,         1
.equ HAS_HWCAP_SSE2,        1
.equ HAS_HWCAP_SSE3,        1
.equ HAS_HWCAP_SSE41,       1
.equ HAS_HWCAP_SSE42,       1
.equ HAS_HWCAP_AESNI,       1
.equ HAS_HWCAP_CLMUL,       1
.equ HAS_HWCAP_POPCNT,      1
.equ HAS_HWCAP_LZCNT,       1
.equ HAS_HWCAP_F16C,        1
.equ HAS_HWCAP_FMA,         1
.equ HAS_HWCAP_AVX,         1
.equ HAS_HWCAP_AVX2,        1
.equ HAS_HWCAP_BMI,         1
.equ HAS_HWCAP_BMI2,        1
.equ HAS_HWCAP_ERMSB,       1
.equ HAS_HWCAP_ADX,         1
.equ HAS_HWCAP_SHA,         1
.equ HAS_HWCAP_AVX512F,     1
.equ HAS_HWCAP_AVX512BW,    1
.equ HAS_HWCAP_AVX512CD,    1
.equ HAS_HWCAP_AVX512DQ,    1
.equ HAS_HWCAP_AVX512ER,    1
.equ HAS_HWCAP_AVX512PF,    1
.equ HAS_HWCAP_AVX512VL,    1
.equ HAS_HWCAP_AVX512IFMA,  1
.equ HAS_HWCAP_AVX512VBMI,  1

# VZEROUPPER is invoked at the end of each function that uses the
# 256-bit AVX ymm* registers to ensure a clean transition back
# to the legacy SSE state. If you're building your programs so that
# you use only VEX-encoded SSE instructions, you can safely comment
# out or remove the vzeroupper instruction in the macro below to
# avoid the extra 5-6 cycle cost of a clean transition.

.macro VZEROPUPPER
vzeroupper
.endm
