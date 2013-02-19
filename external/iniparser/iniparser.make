#
# iniparser Makefile
#

# Compiler settings
CC      = g++
CFLAGS  = -O2 -fPIC -Wall -ansi -pedantic

# Ar settings to build the library
AR	    = ar
ARFLAGS = -ruc

SHLD = ${CC} ${CFLAGS}
LDSHFLAGS = -shared -Wl,-Bsymbolic  -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib
LDFLAGS = -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib

RM = rm -f


# Implicit rules

SUFFIXES = .o .c .h .a .so .sl

COMPILE.c=$(CC) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


SRCS = iniparser.c \
	    dictionary.c

OBJS = $(SRCS:.c=.o)


all:	libiniparser.a

libiniparser.a:	$(OBJS)
	@($(AR) $(ARFLAGS) libiniparser.a $(OBJS))

clean:
	@($(RM) $(OBJS))
	@($(RM) libiniparser.a)
