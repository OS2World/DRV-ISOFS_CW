BASE = ..
include $(BASE)/Makefile.incl

SRCS =	nls_base.c nls_cp437.c nls_cp737.c nls_cp775.c nls_cp850.c \
	nls_cp852.c nls_cp855.c nls_cp857.c nls_cp860.c nls_cp861.c \
	nls_cp862.c nls_cp863.c nls_cp864.c nls_cp865.c nls_cp866.c 

SRCS2 =	nls_cp869.c nls_cp874.c nls_iso8859-1.c nls_iso8859-14.c \
	nls_iso8859-15.c nls_iso8859-2.c nls_iso8859-3.c nls_iso8859-4.c \
	nls_iso8859-5.c nls_iso8859-6.c nls_iso8859-7.c nls_iso8859-8.c \
	nls_iso8859-9.c nls_koi8-r.c nls_mac-roman.c

all:	unls.a 

unls.a:	part1 part2

part1:	$(SRCS:.c=.o)
	ar crs unls.a $?

part2:	$(SRCS2:.c=.o)
	ar crs unls.a $?

ifneq ($(MAKECMDGOALS),clean)
include $(SRCS:.c=.d)
endif
