
.EXPORT_ALL_VARIABLES:

AS = as
CPP = cpp
CC = gcc
AR = ar
SED = sed
RM = rm
CD = cd
MAKE = make

CFLAGS = -Werror -Wimplicit-function-declaration -O2 -fomit-frame-pointer -march=i386

all: dep
	($(CD) lib; $(MAKE))
	($(CD) kernel; $(MAKE))

.PHONY:	dep
dep:
	($(CD) lib; $(MAKE) dep)
	($(CD) kernel; $(MAKE) dep)

.PHONY:	clean
clean:
	($(CD) lib; $(MAKE) clean)
	($(CD) kernel; $(MAKE) clean)
	$(RM) -f *~
