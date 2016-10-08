# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_pand(hebi_packet *r, const hebi_packet *a,
#       const hebi_packet *b, size_t n);

.include "src/p/x86_64/pbitwise.inc"

BITWISE_LOGIC pand, pand, vpand
