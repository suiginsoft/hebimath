# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_addp(hebi_packet *r, const hebi_packet *a, const hebi_packet *b, size_t an, size_t bn);

.include "src/p/x86_64/x86_64.inc"

.macro ADCSBB src, dst
adc \src, \dst
.endm

#-------------------------------------------------------------------------------

MVFUNC_BEGIN padd, _x86_64, 16

.include "src/p/x86_64/paddsub.inc"

MVFUNC_END padd, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR padd, _x86_64

.endif
