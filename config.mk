# customize below to fit your system

# VERSION sets the hebimath release version
VERSION = 0.6

# LINKAGE controls the type of libraries to build
#   both    - builds both shared and static libraries
#   shared  - only build the shared library
#   static  - only build the static library
LINKAGE = both

# KERN controls the maths kernels backend to use and can be set
# to one of the following options:
#   auto, generic, x86_64
KERN = auto

# KERNMV controls function multi-versioning for the maths kernels
# backend and can be set to either dynamic or static. Can be
# set to one of the following options:
#   dynamic - will enable dynamic runtime dispatching based
#             on CPUID, devtmpfs/syfs/procfs file systems,
#             and/or the environment. flags in 'config.inc'
#             are used to enable/disable which kernel
#             versions are included in the library
#   fixed   - will determine which kernel version to use at
#             build time, using the best version matching
#             flags in 'config.inc'
KERNMV = dynamic

# SIMD controls whether to generate support for using GNU C style
# vector extensions (supported by both GCC and Clang). When this
# feature is turned on, emits support for vector extensions
# into the public hebimath.h header that is conditionally enabled
# if the compiler is detected to support this extension. Enabling
# this feature significantly improves performance of certain
# operations when running on integer SIMD hardware. This flag has
# no effect on assembly language kernels, only on C language code
# that takes advantage of this feature. Can be set to one of the
# following options:
#   simd    - enables compiler vector extensions
#   nosimd  - disables compiler vector extensions
SIMD = simd

# file paths
PREFIX = /usr/local

# toolchain
CC = c99
AR = ar
RANLIB = ranlib
AS = as
STRIP = strip

# toolchain flags
ASFLAGS =
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS = -O3 -flto -ftree-vectorize -pedantic -pthread -Wall
ARFLAGS = -rc
LDFLAGS = -s
LDLIBS = -lc -lm

# additional flags used when building shared library
ASFLAGS_shared = -mshared --defsym HAS_PIC=1
CPPFLAGS_shared = -DHEBI_EXPORT_SYMBOLS
CFLAGS_shared = -fpic -fvisibility=hidden
LDFLAGS_shared = -shared

# asflags for different KERNMV modes
ASFLAGS_dynamic = --defsym HAS_MULTI_VERSIONING=1
ASFLAGS_fixed =
