# Configuration section

AR := ar
CC := gcc
CFLAGS := -g -O2 -I../../api/include
MKDIR := mkdir -p

APPS :=  programmer stream_dyn console stream_stat
FRAMEWORKS :=  framework_dyn framework_stat

BUILDDIR := build

# Configuration of the individual products

framework_dyn_SRC_FILES := core.c flash.c fw_version.c rtc.c stream.c \
                       io/cdserial_linux.c io/serial.c \
                       dynamic/depr_stream.c dynamic/dynamic.c
framework_dyn_SRC_PATH  := ../../api/src
framework_dyn_BUILDDIR  := $(BUILDDIR)/framework/dynamic
framework_dyn_FILENAME  := libgestic.so
framework_dyn_CFLAGS    := -fpic -DGESTIC_API_EXPORT -DGESTIC_API_DYNAMIC
framework_dyn_LDFLAGS   := -shared

framework_stat_SRC_FILES := core.c flash.c fw_version.c rtc.c stream.c \
                        io/cdserial_linux.c io/serial.c
framework_stat_SRC_PATH  := ../../api/src
framework_stat_BUILDDIR  := $(BUILDDIR)/framework/static
framework_stat_FILENAME  := libgestic.a

console_SRC_FILES := gestic.c main.c menu.c
console_SRC_PATH  := console
console_BUILDDIR  := $(BUILDDIR)/console
console_FILENAME  := console
console_CFLAGS    := -DGESTIC_API_DYNAMIC
console_LDFLAGS   := -L$(BUILDDIR)/bin -lgestic -Wl,-rpath,\$$ORIGIN -lcurses

stream_dyn_SRC_FILES := stream-dynamic.c
stream_dyn_SRC_PATH  := stream-dynamic
stream_dyn_BUILDDIR  := $(BUILDDIR)/stream-dynamic
stream_dyn_FILENAME  := stream-dynamic
stream_dyn_CFLAGS    := -DGESTIC_API_DYNAMIC
stream_dyn_LDFLAGS   := -L$(BUILDDIR)/bin -lgestic -Wl,-rpath,\$$ORIGIN

stream_stat_SRC_FILES := stream-static.c
stream_stat_SRC_PATH  := stream-static
stream_stat_BUILDDIR  := $(BUILDDIR)/stream-static
stream_stat_FILENAME  := stream-static
stream_stat_LDFLAGS   := -static -L$(BUILDDIR)/bin -lgestic

programmer_SRC_FILES := Library.c Loader.c main.c
programmer_SRC_PATH  := programmer
programmer_BUILDDIR  := $(BUILDDIR)/programmer
programmer_FILENAME  := programmer
programmer_CFLAGS    := -Wno-missing-braces -DGESTIC_API_DYNAMIC
programmer_LDFLAGS   := -L$(BUILDDIR)/bin -lgestic -Wl,-rpath,\$$ORIGIN

.PHONY: all framework apps clean

all: framework apps

framework: $(FRAMEWORKS)

apps: $(APPS)

# Macro for creating object files and their dependency information
define make-object
$3: $2
	$(MKDIR) $$(dir $$@)
	$(CC) $(CFLAGS) $($1_CFLAGS) -MM -MF $$(patsubst %.o,%.d,$$@) $$<
	$(CC) $(CFLAGS) $($1_CFLAGS) -c -o $$@ $$<
endef

# Macro for creating whole products
define make-product
$1_SOURCES := $$(patsubst %,$$($1_SRC_PATH)/%,$$($1_SRC_FILES))
$1_OBJECTS := $$(patsubst %.c,$$($1_BUILDDIR)/%.o,$$($1_SRC_FILES))
$1_DEPENDS := $$(patsubst %.c,$$($1_BUILDDIR)/%.d,$$($1_SRC_FILES))
CLEAN += $$($1_DEPENDS) $$($1_OBJECTS) $$($1_BUILDDIR)/$$($1_FILENAME)
DEPENDS += $$($1_DEPENDS)
$$(foreach src,$$($1_SRC_FILES),$$(eval $$(call make-object,$1,$$(patsubst %,$$($1_SRC_PATH)/%,$$(src)),$$(patsubst %.c,$$($1_BUILDDIR)/%.o,$$(src)))))
$1: $(BUILDDIR)/bin/$$($1_FILENAME)
ifeq ($$(suffix $$($1_FILENAME)),.a)
$(BUILDDIR)/bin/$$($1_FILENAME): $$($1_OBJECTS)
	$(MKDIR) $(BUILDDIR)/bin
	$(AR) rcs $$@ $$^
else
$(BUILDDIR)/bin/$$($1_FILENAME): $$($1_OBJECTS)
	$(MKDIR) $(BUILDDIR)/bin
	$(CC) -o $$@ $$^ $$(LDFLAGS) $$($1_LDFLAGS)
endif
endef

$(foreach framework,$(FRAMEWORKS),$(eval $(call make-product,$(framework))))
$(foreach app,$(APPS),$(eval $(call make-product,$(app))))

-include $(DEPENDS)

clean:
	rm -f $(CLEAN)
