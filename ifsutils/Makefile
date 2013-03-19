BASE = ..
BASE2 = ../..

include $(BASE)/Makefile.incl

PROGS = mntisofs.c umntiso.c

LIB = $(BASE)/misc/getopt.a 

all: $(PROGS:.c=$(EXE))

$(PROGS:.c=$(EXE)): $(LIB)

%$(EXE): %.o 
	$(CC) $(CFLAGS) $(LDFLAGS) $< $(LIB) $(SYSLIB) -o $@

clean-extra:
	$(RM) $(PROGS:.c=$(EXE))

install: all
	$(CP) $(PROGS:.c=$(EXE)) $(INSTALLDIR)

ifneq ($(MAKECMDGOALS),clean)
include $(PROGS:.c=.d)
endif
