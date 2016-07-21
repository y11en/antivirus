# -*- Makefile -*- Time-stamp: <04/03/25 23:18:30 ptr>
# $Id: Makefile.inc,v 1.2 2004/03/29 08:16:24 ptr Exp $

LIBNAME = klcsrt
MAJOR = 5
MINOR = 0
PATCH = 0
SRC_CPP = ../dll_main.cpp \
          ../fstream.cpp \
          ../strstream.cpp \
          ../sstream.cpp \
          ../ios.cpp \
          ../stdio_streambuf.cpp \
          ../istream.cpp \
          ../ostream.cpp \
          ../iostream.cpp \
          ../codecvt.cpp \
          ../collate.cpp \
          ../ctype.cpp \
          ../monetary.cpp \
          ../num_get.cpp \
          ../num_put.cpp \
          ../num_get_float.cpp \
          ../num_put_float.cpp \
          ../numpunct.cpp \
          ../time_facets.cpp \
          ../messages.cpp \
          ../locale_impl.cpp \
          ../locale.cpp \
          ../locale_catalog.cpp \
          ../facets_byname.cpp \
          ../c_locale_stub.cpp \
          ../complex.cpp \
          ../complex_exp.cpp \
          ../complex_io.cpp \
          ../complex_trig.cpp \
          ../complex_io_w.cpp \
          ../string_w.cpp \
	  ../streambuf.cpp

SRC_C = c_locale.c

SRC_RC = ../stlport.rc
