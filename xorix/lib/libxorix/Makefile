
TOPDIR = ../..
INCDIR = $(TOPDIR)/include

SRCS = xorix.c
OBJS = $(subst .c,.o,$(subst .S,.o,$(SRCS)))

all:	libxorix.a

libxorix.a:	$(OBJS)
	$(AR) rcs libxorix.a $(OBJS)

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
	$(RM) -f *.o *.a *~ depend.txt

ifeq (depend.txt, $(wildcard depend.txt))
include depend.txt
endif
