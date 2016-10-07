# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_addp(hebi_packet *r, const hebi_packet *a, const hebi_packet *b,
#           size_t m, size_t n);

.include "src/p/x86_64/x86_64.inc"

.macro ADCSBB Src, Dst
adc \Src, \Dst
.endm

FUNC_BEGIN padd
.include "src/p/x86_64/paddsub.inc"
FUNC_END
