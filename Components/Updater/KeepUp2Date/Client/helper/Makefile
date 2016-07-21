# $Id: Makefile,v 1.34 2003/08/07 09:31:37 mike Exp $

BINDIR ?= ../bin
LIBDIR ?= ../lib
OBJDIR ?= ../obj
DIRS = $(OBJDIR) $(LIBDIR) $(BINDIR)
ADMKIT ?= ../../Admin\ Kit/DEVELOPMENT
EXPAT  ?= ../../expat-1.95.2/lib
SO_EXT ?= so

AKSONAME = libadmkit.$(SO_EXT)

SONAME = libkavupdhlp.$(SO_EXT)
STATIC_NAME = libkavupdhlp.a

SYSARCH=linux
SYSARCHDEF=__LINUX_

LD_TO_BUILD=$(shell uname|grep "HP-UX">/dev/null && \
	echo "/usr/ccs/bin/ld" || echo "g++") 
SHARED=$(shell uname|grep "HP-UX">/dev/null && \
	echo "-b" || echo "-shared") 
SONAME_TO_LD=$(shell uname|grep "HP-UX">/dev/null && \
	echo "+h $(SONAME)" || echo "-Wl,-soname,$(SONAME)") 
AKSONAME_TO_LD=$(shell uname|grep "HP-UX">/dev/null && \
	echo "+h $(AKSONAME)" || echo "-Wl,-soname,$(AKSONAME)") 

C_SOURCES = 
CPP_SOURCES = strptime.cpp message.cpp sites_info.cpp \
              string.cpp log_iface.cpp \
              file_transaction_common.cpp local_file_common.cpp \
              license_common.cpp licensing.cpp \
              update_transaction.cpp map_wrapper.cpp

C_SOURCES_SYSARCH = 
CPP_SOURCES_SYSARCH = stdMutex.cpp file_transaction.cpp local_file.cpp \
			  dummy.cpp sign.cpp

C_OBJS := $(addprefix $(OBJDIR)/,$(C_SOURCES:.c=.o))
CPP_OBJS := $(addprefix $(OBJDIR)/,$(CPP_SOURCES:.cpp=.o))
C_OBJS_SYSARCH := $(addprefix $(OBJDIR)/,$(C_SOURCES_SYSARCH:.c=.o))
CPP_OBJS_SYSARCH := $(addprefix $(OBJDIR)/,$(CPP_SOURCES_SYSARCH:.cpp=.o))
OBJS = $(C_OBJS) $(CPP_OBJS) $(C_OBJS_SYSARCH) $(CPP_OBJS_SYSARCH)

CFLAGS_MT = $(shell uname|grep FreeBSD>/dev/null && echo "-DNO_MT")
CFLAGS += -D$(SYSARCHDEF)
CFLAGS += $(CFLAGS_MT)
DEBUG_FLAGS = -g -Wall
OPTIMIZATION_FLAGS = 

INCLUDE_DIRS = -I../include -I../core -I../helper/${SYSARCH} -I${SYSARCH} -I. -I$(ADMKIT) -I$(ADMKIT)/include -I$(EXPAT)

CC = g++ $(INCLUDE_DIRS)

all: $(DIRS) $(LIBDIR)/$(SONAME) $(LIBDIR)/$(STATIC_NAME) \
$(LIBDIR)/$(AKSONAME)

$(DIRS):
	@mkdir -p $@

$(C_OBJS): $(OBJDIR)/%.o: %.c
	$(CC) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS) $(CFLAGS) -c -o $@ $<

$(CPP_OBJS): $(OBJDIR)/%.o: %.cpp
	$(CC) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS) $(CFLAGS) -c -o $@ $<

$(C_OBJS_SYSARCH): $(OBJDIR)/%.o: $(SYSARCH)/%.c
	$(CC) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS) $(CFLAGS) -c -o $@ $<

$(CPP_OBJS_SYSARCH): $(OBJDIR)/%.o: $(SYSARCH)/%.cpp
	$(CC) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS) $(CFLAGS) -c -o $@ $<

$(LIBDIR)/$(SONAME): $(OBJS)
	$(LD_TO_BUILD) $(SHARED) $(SONAME_TO_LD) -o $@ $(OBJS)

$(LIBDIR)/$(STATIC_NAME): $(OBJS)
	ar r $@ $(OBJS)

$(LIBDIR)/$(AKSONAME): $(OBJS)
	$(LD_TO_BUILD) $(SHARED) $(AKSONAME_TO_LD) -o $@ $(OBJDIR)/dummy.o \
	$(OBJDIR)/string.o 

clean:
	rm -f $(LIBDIR)/$(SONAME) $(LIBDIR)/$(STATIC_NAME) $(OBJS)
	rm -f $(LIBDIR)/$(AKSONAME)


