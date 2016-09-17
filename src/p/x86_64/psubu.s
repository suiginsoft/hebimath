# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t hebi_subp_u(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

#-------------------------------------------------------------------------------

.macro MOVSUB src, dst
sub \src, \dst
.endm

.macro ADCSBB src, dst
sbb \src, \dst
.endm

MVFUNC_BEGIN psubu, x86_64
.include "src/p/x86_64/paddsubu.inc"
MVFUNC_END

#-------------------------------------------------------------------------------

.ifdef HAS_MULTI_VERSIONING
MVFUNC_DISPATCH_PTR psubu, x86_64
.endif
