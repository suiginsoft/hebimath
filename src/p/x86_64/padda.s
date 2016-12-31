# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_padda(hebi_packet *r, const hebi_packet *a, size_t m, size_t n);

.include "src/p/x86_64/paddsub.inc"

PADDSUBA padda, paddac, adc
