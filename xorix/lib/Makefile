
TOPDIR = ..
INCDIR = $(TOPDIR)/include

all:
	($(CD) libgcc; $(MAKE))
	($(CD) libxorix; $(MAKE))
	($(CD) libc; $(MAKE))

.PHONY:	dep
dep:
	($(CD) libgcc; $(MAKE) dep)
	($(CD) libxorix; $(MAKE) dep)
	($(CD) libc; $(MAKE) dep)

.PHONY:	clean
clean:
	($(CD) libgcc; $(MAKE) clean)
	($(CD) libxorix; $(MAKE) clean)
	($(CD) libc; $(MAKE) clean)
	$(RM) -f *.o *.a *~ ../include/*~
