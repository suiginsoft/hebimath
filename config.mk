# customize below to fit your system

# VERSION sets the hebimath release version
VERSION = 0.6

# LINKAGE controls the type of libraries to build
#   both    - builds both shared and static libraries
#   shared  - only build the shared library
#   static  - only build the static library
LINKAGE = both

# DRIVER controls which low-level functions backend to compile
# and use and can be set to one of the following options:
#   auto    - auto-detect best backend for host system
#   generic - generic portable backend
#   x86_64  - optimized for x86-64 processors
DRIVER = auto

# DISPATCH controls function multi-versioning for the backend
# and can be set to either dynamic or static:
#   dynamic - will enable dynamic runtime dispatching based
#             on CPUID, devtmpfs/syfs/procfs file systems,
#             and/or the environment. flags in 'config.inc'
#             are used to enable/disable which kernel
#             versions are included in the library
#   static  - will determine which kernel version to use at
#             build time, using the best version matching
#             flags in 'config.inc'
DISPATCH = dynamic

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
#   on      - enables compiler vector extensions
#   off     - disables compiler vector extensions
SIMD = on

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
CFLAGS = -O3 -pedantic -pthread -Wall
LDFLAGS = -s
LDLIBS = -lc -lm
ARFLAGS = -rc

# debug toolchain flags
#CFLAGS = -g -O0 -pedantic -pthread -Wall -DUSE_ASSERTIONS
#LDFLAGS =

# additional flags used when building shared library
ASFLAGS_shared = -mshared --defsym USE_PIC=1
CPPFLAGS_shared = -DHEBI_EXPORT_SYMBOLS
CFLAGS_shared = -fpic -fvisibility=hidden
LDFLAGS_shared = -shared

# asflags for different FNMV modes
ASFLAGS_dispatch_dynamic = --defsym USE_MULTI_VERSIONING=1
ASFLAGS_dispatch_static =
