# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_psubu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/x86_64.inc"

.macro ADDSUB src, dst
sub \src, \dst
.endm

.macro ADCSBB src, dst
sbb \src, \dst
.endm

FUNC_BEGIN psubu
.include "src/p/x86_64/paddsubu.inc"
FUNC_END
