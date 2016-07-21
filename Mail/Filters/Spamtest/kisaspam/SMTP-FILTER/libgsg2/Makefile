
all: libgsg.a file2gsg

CFLAGS= -DNODUMP -g -O3 -Wall -I../.. -I/usr/local/include

libgsg.a: binary_handler.o binary_handler1.o image.o image-gif.o gsg.h image-jpeg.o Makefile
	rm -f $@
	ar cr $@ binary_handler.o binary_handler1.o  image.o image-gif.o image-jpeg.o
	ranlib $@

.c.o:	$< gsg.h  Makefile
	gcc ${CFLAGS} -c  $<


file2gsg: file2gsg.o libgsg.a Makefile
	gcc -static ${CFLAGS} -o $@ file2gsg.o -L/usr/local/lib -L../libsigs -L. -lgsg -lsigs -L../libdccwrapper -ldccwrapper -lmd -ljpeg -L../ungif -lgif -lm -ldcc

clean:
	rm -f *.o libgsg.a file2gsg *.core *gmon *~
