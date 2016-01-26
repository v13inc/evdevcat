X86_GCC=gcc
MIPS_GCC=/opt/entware/toolchain-entware/bin/mipsel-uclibc-gcc -std=c99

.PHONY: build clean
build: evdevcat evdevcat.mips

clean:
	rm evdevcat evdevcat.mips

evdevcat: evdevcat.c evdevcat.h
	$(X86_GCC) $^ -o $@

evdevcat.mips: evdevcat.c evdevcat.h
	$(MIPS_GCC) $^ -o $@

