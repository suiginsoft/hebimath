# see config.mk for documentation
VERSION = 0.6
LINKAGE = static
DRIVER = generic
DISPATCH = static

# file paths
PREFIX = /usr/local

# toolchain
RANLIB = ranlib
STRIP = strip

# toolchain flags
CFLAGS = -O3 -std=c99 -pedantic -pthread -Wall -Wextra -Waggregate-return \
	 -Wconversion -Wshadow
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L -DUSE_ASSERTIONS
ASFLAGS =
LDFLAGS = -s
LDLIBS = -lc -lm
ARFLAGS = -rc

# additional flags used when building shared library
ASFLAGS_shared = --defsym USE_PIC=1
CPPFLAGS_shared = -DHEBI_EXPORT_SYMBOLS
CFLAGS_shared = -fpic -fvisibility=hidden
LDFLAGS_shared = -shared

# asflags for different DISPATCH modes
ASFLAGS_dispatch_dynamic = --defsym USE_MULTI_VERSIONING=1
ASFLAGS_dispatch_static =
