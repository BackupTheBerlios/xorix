
TOPDIR = ..
INCDIR = $(TOPDIR)/include
LIBDIR = $(TOPDIR)/lib

LIBS = $(LIBDIR)/libc/libc.a $(LIBDIR)/libxorix/libxorix.a \
       $(LIBDIR)/libgcc/libgcc.a

SRCS = multiboot.S console.c cpu.c gdt.c idt.c \
       memory.c setup.c shutdown.c page.c page_pae.c \
       entry.S apm.c bootmem.c debug.c apic.c \
       smp.c smpboot.S sleep.c kmalloc.c spinlock.c \
       cmos.c task.c

OBJS = $(subst .c,.o,$(subst .S,.o,$(SRCS)))

all:	xorix.elf

xorix.elf:	$(OBJS) $(LIBS)
	$(LD) multiboot.o $(OBJS:multiboot.o=) $(LIBS) -e _start -nostdlib --oformat elf32-i386 -Ttext 0x100000 -o xorix.elf

.S.o:
	$(CPP) -nostdinc -I$(INCDIR) $< | $(AS) -o $@

.c.o:
	$(CC) -nostdinc -I$(INCDIR) $(CFLAGS) -c $<

.PHONY:	dep
dep:
	( \
	for i in $(SRCS); \
	do \
		echo; \
		$(CPP) -M $$i -nostdinc -I$(INCDIR) | $(SED) '1s/:/: Makefile/'; \
	done \
	) > depend.txt

.PHONY:	clean
clean:
	$(RM) -f *.o xorix.elf *~ depend.txt

ifeq (depend.txt, $(wildcard depend.txt))
include depend.txt
endif
