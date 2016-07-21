INCS = -I.
LIBDIR = /usr/lib
CC = gcc
CFLAGS = -fno-builtin -g -c -W -Wreturn-type -Wcomment

OBJS = dev2gif.o egif_lib.o dgif_lib.o gifalloc.o gif_font.o \
	gif_err.o quantize.o qprintf.o getarg.o

.c.o:
	$(CC) $(INCS) $(DEVS) $(CFLAGS) $<

all: static gif2rgb

static: libgif.a

gif2rgb: gif2rgb.c
	${CC} -o gif2rgb gif2rgb.c -L. -lgif

libgif.a: $(OBJS)
	rm -f libgif.a
	ar rcv libgif.a $(OBJS)
	-ranlib libgif.a

dev2gif.o: gif_lib.h
egif_lib.o: gif_lib.h
dgif_lib.o: gif_lib.h
gif_hash.o: gif_lib.h
gif_err.o: gif_lib.h
gifalloc.o: gif_lib.h
gif_font.o: gif_lib.h
quantize.o: gif_lib.h
qprintf.o: gif_lib.h
getarg.o: getarg.h

clean:
	rm -f *.[oa] *~ gif2rgb
