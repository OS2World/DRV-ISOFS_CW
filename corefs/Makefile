BASE = ..
include $(BASE)/Makefile.incl

SRCS =   isoaccess.c

all: corefs.a 

corefs.a: $(SRCS:.c=.o)
	ar crs $@ $?

ifneq ($(MAKECMDGOALS),clean)
include $(SRCS:.c=.d)
endif
