# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# hebi_word hebi_subp_u(hebi_packet *r, const hebi_packet *a, hebi_word b, size_t n);

.include "src/p/x86_64/x86_64.inc"

.macro MOVSUB src, dst
sub \src, \dst
.endm

.macro ADCSBB src, dst
sbb \src, \dst
.endm

#-------------------------------------------------------------------------------

MVFUNC_BEGIN psubu, _x86_64, 16

.include "src/p/x86_64/paddsubu.inc"

MVFUNC_END psubu, _x86_64

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING

MVFUNC_PTR psubu, _x86_64

.endif
