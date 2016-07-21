CC = gcc
CFLAGS = -O -g -Wall -I../.. -I ../../../common-release-3
LIBS = -L  ../../../common-release-3/gnu/md5a -lmd5a

all:  mksigdb testsigdb dumpsigdb checksigdb findsig libsigs.a

libsigs.a: libsigs.c
	rm -f libsigs.a
	$(CC) $(CFLAGS) -c libsigs.c
	ar cr libsigs.a libsigs.o
	ranlib libsigs.a

findsig: findsig.c libsigs.c
	$(CC) $(CFLAGS) -o findsig findsig.c libsigs.c $(LIBS)

mksigdb: mksigdb.c libsigs.c
	$(CC) $(CFLAGS) -o mksigdb mksigdb.c libsigs.c $(LIBS)

testsigdb: testsigdb.c libsigs.c
	$(CC) $(CFLAGS) -o testsigdb testsigdb.c libsigs.c $(LIBS)

dumpsigdb: dumpsigdb.c libsigs.c
	$(CC) $(CFLAGS) -o dumpsigdb dumpsigdb.c libsigs.c $(LIBS)

checksigdb: checksigdb.c libsigs.c
	$(CC) $(CFLAGS) -o checksigdb checksigdb.c libsigs.c $(LIBS)

clean:
	rm -f mksigdb testsigdb dumpsigdb checksigdb *.o *~ findsig
