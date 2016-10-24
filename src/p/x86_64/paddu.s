# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

# uint64_t
# hebi_paddu(hebi_packet *r, const hebi_packet *a, uint64_t b, size_t n);

.include "src/p/x86_64/paddsub.inc"

PADDSUBU paddu, paddc, add, adc
PADDSUBC paddc, adc
