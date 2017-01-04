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

# file paths
PREFIX = /usr/local

# toolchain
CC = cc
AR = ar
RANLIB = ranlib
STRIP = strip
AS = as

# toolchain flags
CFLAGS = -O3 -std=c99 -pedantic -pthread -Wall -Wextra -Waggregate-return \
	 -Wconversion -Wmissing-prototypes -Wshadow
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
ASFLAGS =
LDFLAGS = -s
LDLIBS = -lc -lm
ARFLAGS = -rc

# additional flags used when building shared library
ASFLAGS_shared = --defsym USE_PIC=1
CPPFLAGS_shared = -DHEBI_EXPORT_SYMBOLS
CFLAGS_shared = -fpic -fvisibility=hidden
LDFLAGS_shared = -shared

# asflags for different FNMV modes
ASFLAGS_dispatch_dynamic = --defsym USE_MULTI_VERSIONING=1
ASFLAGS_dispatch_static =
