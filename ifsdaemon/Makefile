BASE = ..
BASE2 = ../..
include $(BASE)/Makefile.incl

OPEN_FILES_LIMIT = 512

CFLAGS += -DOPEN_FILES_LIMIT=$(OPEN_FILES_LIMIT)

SRCS = isofsdmn.c attach.c find.c misc.c dir.c openclose.c \
 fileio.c fileinfo.c fileop.c ea.c utils.c 
#debug.c except.c

LIB =  $(BASE)/misc/getopt.a $(BASE)/corefs/corefs.a  \
 $(BASE)/system/$(SYSTEM)/sysdep.a $(BASE)/libunls/unls.a

all: isofsdmn$(EXE)

isofsdmn$(EXE): $(SRCS:.c=.o) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS:.c=.o) $(LIB) $(SYSLIB) -o $@
	emxbind -a $@ "-h$(OPEN_FILES_LIMIT)"

clean-extra:
	$(RM) isofsdmn$(EXE)

install: all
	$(CP) isofsdmn$(EXE) $(INSTALLDIR)

ifneq ($(MAKECMDGOALS),clean)
include $(SRCS:.c=.d)
endif
