# hebimath - arbitrary precision arithmetic library
# See LICENSE file for copyright and license details

CONFIG := config.mk
include $(CONFIG)

DRIVER_generic := generic
DRIVER_x86_64 := x86_64

DRIVER_auto != sh -c 'uname -m 2>/dev/null || echo generic | \
	sed -e ''s/i.86$$/i386/'''

DRIVER_selected != sh -c 'test -n "$(DRIVER_$(DRIVER))" -a \
	-d src/p/$(DRIVER_$(DRIVER)) && \
	echo $(DRIVER_$(DRIVER)) || echo generic'

DISPATCH_generic := static
DISPATCH_x86_64 := $(DISPATCH)
DISPATCH_driver := $(DISPATCH_$(DRIVER_selected))

CPPFLAGS_generic := -DUSE_DRIVER_GENERIC
CPPFLAGS_x86_64 := -DUSE_DRIVER_X86_64
CPPFLAGS_driver := $(CPPFLAGS_$(DRIVER_selected))

PDRIVERFUNCS := \
	padd \
	paddu \
	pneg \
	psub \
	psubu \
	recipulut

PDRIVERMVFUNCS := \
	pand \
	pclz \
	pcmp \
	pcopy \
	pctz \
	pdivremr \
	pdivremu \
	pdivremru2x1 \
	pdivremru3x2 \
	pmove \
	pmul \
	pmulu \
	pnorm \
	pnot \
	por \
	pshl \
	pshr \
	psqr \
	pxor \
	pzero \
	recipu2x1 \
	recipu3x2

PDRIVERMVFUNCS_dynamic := $(PDRIVERMVFUNCS:%=dynamic/%)

PFUNCTIONS := \
	palloc \
	palloc_cb \
	pfree \
	pfree_cb \
	pmul_karatsuba \
	pmul_karatsuba_space \
	pdivrem \
	prand_kiss \
	psetu \
	psetu2 \
	psetzero \
	psqr_karatsuba \
	psqr_karatsuba_space \
	$(PDRIVERFUNCS:%=$(DRIVER_selected)/%) \
	$(PDRIVERMVFUNCS:%=$(DRIVER_selected)/%) \
	$(PDRIVERMVFUNCS_$(DISPATCH_driver))

ZFUNCTIONS := \
	zinit \
	zinits \
	zinitn \
	zinit_allocator \
	zinit_buffer \
	zinit_reserve \
	zinit_copy \
	zinit_copy_buffer \
	zinit_copy_reserve \
	zinit_move \
	zdestroy \
	zdestroys \
	zdestroyn \
	zrealloc \
	zrealloczero \
	zreserve \
	zshrink \
	zswap \
	zallocator \
	zcapacity \
	zused \
	zsign \
	zzero \
	zeven \
	zodd \
	zbits \
	zcmpmag \
	zcmp \
	zset \
	zset_copy \
	zset_move \
	zseti \
	zsetu \
	zsetzero \
	zsetstr \
	zgeti \
	zgetu \
	zgetsi \
	zgetsu \
	zgetstr \
	zabs \
	zneg \
	zadd \
	zsub \
	zmul \
	zsqr \
	zdiv \
	zrem \
	zdivrem \
	zaddi \
	zaddu \
	zsubi \
	zsubu \
	zmuli \
	zmulu \
	zdivi \
	zdivu \
	zremi \
	zremu \
	zdivremi \
	zdivremu \
	zshl \
	zshr \
	zrand_kiss

FUNCTIONS := \
	$(PFUNCTIONS:%=p/%) \
	$(ZFUNCTIONS:%=z/%) \
	error_assert \
	error_handler \
	error_jmp \
	error_save \
	error_restore \
	error_last \
	error_raise \
	alloc \
	alloc_cb \
	free \
	free_cb

MODULES_dynamic := hwcaps

MODULES := \
	alloc_get \
	alloc_set \
	alloc_table \
	context \
	$(MODULES_$(DISPATCH_driver))

SRC := $(FUNCTIONS) $(MODULES)

CFG_TARGETS_generic := config.h
CFG_TARGETS_x86_64 := config.h config.inc

DEPS_generic := src/p/generic/generic.h
DEPS_x86_64 := config.inc src/p/x86_64/x86_64.inc
DEPS := $(CONFIG) hebimath.h config.h internal.h src/p/pcommon.h $(DEPS_$(DRIVER_selected))

OBJ_shared := $(SRC:%=src/%.po)
OBJ_static := $(SRC:%=src/%.o)

LIBS_both := libhebimath.so libhebimath.a
LIBS_shared := libhebimath.so
LIBS_static := libhebimath.a
LIBS := $(LIBS_$(LINKAGE))

TEST_LDLIBS_both := libhebimath.a
TEST_LDLIBS_shared := -lhebimath
TEST_LDLIBS_static := libhebimath.a
TEST_LDLIBS := $(TEST_LDLIBS_$(LINKAGE))

BENCH_P := \
	padd \
	paddu \
	pclz \
	pcmp \
	pcopy \
	pdivrem \
	pdivremu \
	pmove \
	pmul \
	pmul_karatsuba \
	pmulu \
	pnorm \
	pshl \
	pshr \
	psqr \
	psqr_karatsuba \
	psub \
	psubu \
	pzero

BENCH_P_BIN := $(BENCH_P:%=bench/p/%)
BENCH_BIN := $(BENCH_P_BIN)
BENCH_OBJ := bench/bench.o
BENCH_DEPS := $(DEPS) bench/bench.h

CHECK_P := \
	pcmp \
	pcopy \
	pnorm \
	pzero

CHECK_Z := \
	zgetset \
	zadd \
	zaddi \
	zaddu \
	zsub \
	zsubi \
	zsubu \
	zmul \
	zmuli \
	zmulu \
	zdiv \
	zdivi \
	zdivu \
	zrem \
	zremi \
	zremu \
	zshl \
	zshr

CHECK_SRC := \
	check \
	bc \
	zcheckbinop \
	zcheckbinopi64 \
	zcheckbinopu64 \
	zdirty \
	zpermutation

CHECK_P_BIN := $(CHECK_P:%=check/p/%)
CHECK_Z_BIN := $(CHECK_Z:%=check/z/%)
CHECK_BIN := $(CHECK_P_BIN) $(CHECK_Z_BIN)
CHECK_OBJ := $(CHECK_SRC:%=check/%.o)
CHECK_DEPS := $(DEPS) check/check.h

Q_ := @
Q_0 := @
Q := $(Q_$(V))

.SUFFIXES:
.SUFFIXES: .c .s .o .po

.PHONY: all config check check/p check/z \
	clean scrub dist install uninstall options

all: $(LIBS)

config: $(CFG_TARGETS_$(DRIVER_selected))

config.h:
	@echo copying $@ from config.def.h
	$(Q)cp config.def.h $@

config.inc:
	@echo copying $@ from config.def.$(DRIVER_selected).inc
	$(Q)cp config.def.$(DRIVER_selected).inc $@

hebimath.h: hebimath.h.in
	@echo generating $@ from hebimath.h.in
	$(Q)awk -v p='dispatch_$(DISPATCH_driver)|simd_$(SIMD)' \
	    '$$1==">>>"{x=$$2!~p;next;} \
	     $$1=="<<<"{x=0;next;} \
	     !x{print $$0}' \
	     <hebimath.h.in >$@

libhebimath.so: $(DEPS) $(OBJ_shared)
	@echo LD $@
	$(Q)$(CC) $(LDFLAGS_shared) -o $@ $(LDFLAGS) $(OBJ_shared) $(LDLIBS)

libhebimath.a: $(DEPS) $(OBJ_static)
	@echo AR $@
	$(Q)$(AR) $(ARFLAGS) $@ $(OBJ_static)
	$(Q)$(RANLIB) $@

bench: $(BENCH_BIN)
	@sh runtests.sh "bench" $(BENCH_BIN)

bench/p: $(BENCH_BIN)
	@sh runtests.sh "bench/p" $(BENCH_P_BIN)

$(BENCH_BIN): $(BENCH_DEPS) $(LIBS) bench/libbench.a
	@echo CC $@.c
	$(Q)$(CC) -o $@ $(CFLAGS_static) $(CFLAGS) \
		$(CPPFLAGS_static) $(CPPFLAGS) $(CPPFLAGS_driver) $@.c \
		$(LDFLAGS) -L. bench/libbench.a $(TEST_LDLIBS) $(LDLIBS)

bench/libbench.a: $(BENCH_DEPS) $(BENCH_OBJ)
	@echo AR $@
	$(Q)$(AR) $(ARFLAGS) $@ $(BENCH_OBJ)
	$(Q)$(RANLIB) $@

check: $(CHECK_BIN)
	@sh runtests.sh "check" $(CHECK_BIN)

check/p: $(CHECK_P_BIN)
	@sh runtests.sh "check/p" $(CHECK_P_BIN)

check/z: $(CHECK_Z_BIN)
	@sh runtests.sh "check/z" $(CHECK_Z_BIN)

$(CHECK_BIN): $(CHECK_DEPS) $(LIBS) check/libcheck.a
	@echo CC $@.c
	$(Q)$(CC) -o $@ $(CFLAGS_static) $(CFLAGS) \
		$(CPPFLAGS_static) $(CPPFLAGS) $(CPPFLAGS_driver) $@.c \
		$(LDFLAGS) -L. check/libcheck.a $(TEST_LDLIBS) $(LDLIBS)

check/libcheck.a: $(CHECK_DEPS) $(CHECK_OBJ)
	@echo AR $@
	$(Q)$(AR) $(ARFLAGS) $@ $(CHECK_OBJ)
	$(Q)$(RANLIB) $@

.c.po: $(DEPS)
	@echo CC $<
	$(Q)$(CC) -o $@ -c $(CFLAGS_shared) $(CFLAGS) \
		$(CPPFLAGS_shared) $(CPPFLAGS) $(CPPFLAGS_driver) $<

.c.o: $(DEPS)
	@echo CC $<
	$(Q)$(CC) -o $@ -c $(CFLAGS_static) $(CFLAGS) \
		$(CPPFLAGS_static) $(CPPFLAGS) $(CPPFLAGS_driver) $<

.s.po: $(DEPS)
	@echo AS $<
	$(Q)$(AS) -o $@ $(ASFLAGS_dispatch_$(DISPATCH_driver)) \
		$(ASFLAGS_shared) $(ASFLAGS) $<
	$(Q)$(STRIP) -o $@ -x $@

.s.o: $(DEPS)
	@echo AS $<
	$(Q)$(AS) -o $@ $(ASFLAGS_dispatch_$(DISPATCH_driver)) \
		$(ASFLAGS_static) $(ASFLAGS) $<
	$(Q)$(STRIP) -o $@ -x $@

clean:
	@echo cleaning
	$(Q)rm -f hebimath.h $(LIBS_both) $(OBJ_shared) $(OBJ_static) \
		bench/libbench.a check/libcheck.a $(CHECK_BIN) $(CHECK_OBJ)

scrub: clean
	@echo scrubbing
	$(Q)rm -f config.h config.inc *.tar.xz
	$(Q)find bench -type f -name "*.o" -exec rm -f {} \;
	$(Q)find bench -type f -name "*.po" -exec rm -f {} \;
	$(Q)find bench -type f -name "*.lst" -exec rm -f {} \;
	$(Q)find bench -type f -perm -0100 -exec rm -f {} \;
	$(Q)find check -type f -name "*.o" -exec rm -f {} \;
	$(Q)find check -type f -name "*.po" -exec rm -f {} \;
	$(Q)find check -type f -name "*.lst" -exec rm -f {} \;
	$(Q)find check -type f -perm -0100 -exec rm -f {} \;
	$(Q)find src -type f -name "*.o" -exec rm -f {} \;
	$(Q)find src -type f -name "*.po" -exec rm -f {} \;
	$(Q)find src -type f -name "*.lst" -exec rm -f {} \;

dist: scrub
	@echo creating dist tarball
	$(Q)mkdir -p hebimath-$(VERSION)
	$(Q)cp -R -t hebimath-$(VERSION) LICENSE README Makefile \
		config.def.h config.def.x86_x64.inc config.mk \
		hebimath.h.in internal.h bench check src
	$(Q)tar -cJf hebimath-$(VERSION).tar.xz hebimath-$(VERSION)
	$(Q)rm -rf hebimath-$(VERSION)

install: all
	@echo installing header files to $(DESTDIR)$(PREFIX)/include
	$(Q)mkdir -p $(DESTDIR)$(PREFIX)/include
	$(Q)cp -f hebimath.h $(DESTDIR)$(PREFIX)/include
	$(Q)chmod 644 $(DESTDIR)$(PREFIX)/include/hebimath.h
	@echo installing library files to $(DESTDIR)$(PREFIX)/lib
	$(Q)mkdir -p $(DESTDIR)$(PREFIX)/lib
	$(Q)cp -f libhebimath.a $(DESTDIR)$(PREFIX)/lib
	$(Q)chmod 644 $(DESTDIR)$(PREFIX)/lib/libhebimath.a
	$(Q)cp -f libhebimath.so $(DESTDIR)$(PREFIX)/lib
	$(Q)chmod 755 $(DESTDIR)$(PREFIX)/lib/libhebimath.so

uninstall:
	@echo removing header files from $(DESTDIR)$(PREFIX)/lib
	$(Q)rm -f $(DESTDIR)$(PREFIX)/include/hebimath.h
	@echo removing library files from $(DESTDIR)$(PREFIX)/lib
	$(Q)rm -f $(DESTDIR)$(PREFIX)/lib/libhebimath.a
	$(Q)rm -f $(DESTDIR)$(PREFIX)/lib/libhebimath.so

options:
	@echo libhebimath build options:
	@echo "LINKAGE           = $(LINKAGE)"
	@echo "DRIVER            = $(DRIVER_selected)"
	@echo "DISPATCH          = $(DISPATCH_driver)"
	@echo "SIMD              = $(SIMD)"
	@echo "CPPFLAGS          = $(CPPFLAGS)"
	@echo "CFLAGS            = $(CFLAGS)"
	@echo "ASFLAGS           = $(ASFLAGS)"
	@echo "ARFLAGS           = $(ARFLAGS)"
	@echo "LDFLAGS           = $(LDFLAGS)"
	@echo "CC                = $(CC)"
	@echo "AS                = $(AS)"
	@echo "AR                = $(AR)"
	@echo "STRIP             = $(STRIP)"
