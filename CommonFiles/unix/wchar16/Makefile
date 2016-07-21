ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=wchar16

INCLUDES= \
	$(ROOT)/external

SOURCES= \
	mbsrtowcs.c \
	mbstowcs.c \
	towlower.c \
	towupper.c \
	wcscat.c \
	wcschr.c \
	wcscmp.c \
	wcscpy.c \
	wcscspn.c \
	wcslen.c \
	wcsncat.c \
	wcsncmp.c \
	wcsrchr.c \
	wcsrtombs.c \
	wcstombs.c

DEPS= \
	$(ROOT)/external/libutf

include $(ROOT)/Makefiles/Make.Archive
