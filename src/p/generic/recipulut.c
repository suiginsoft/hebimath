/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "generic.h"

#ifdef USE_INT128
#include "../recipu64lut.c"
#else
#include "../recipu32lut.c"
#endif
