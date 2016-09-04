# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_addp_u(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

.macro MOVSUB src, dst
mov \src, \dst
.endm

.macro ADCSBB src, dst
adc \src, \dst
.endm

#-------------------------------------------------------------------------------

MVFUNC_BEGIN paddu, _x86_64, 16

.include "src/p/x86_64/paddsubu.inc"

MVFUNC_END paddu, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR paddu, _x86_64

.endif
