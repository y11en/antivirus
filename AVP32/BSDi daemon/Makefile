#Makefile for Avp for Lunix

PROG = AvpBSDiDaemon
SINSTALL   = \
	LibDisk/Blocks.c LibDisk/create_chunk.c \
	LibDisk/Chunk.c LibDisk/Change.c LibDisk/Disk.c LibDisk/Rules.c \
	LibUtil/property.c \
	SysInstall/cdrom.c SysInstall/Disks.c SysInstall/floppy.c \
	SysInstall/dos.c SysInstall/devices.c \
	SysInstall/media.c SysInstall/msg.c \
	SysInstall/variable.c SysInstall/misc.c \
	SysInstall/System.c SysInstall/tape.c
#SysInstall/config.c SysInstall/network.c 
SINSTALLOBJ= \
	LibDisk/Blocks.o LibDisk/create_chunk.o \
	LibDisk/Chunk.o LibDisk/Change.o LibDisk/Disk.o LibDisk/Rules.o \
	LibUtil/property.o \
	SysInstall/cdrom.o SysInstall/Disks.o SysInstall/floppy.o \
	SysInstall/dos.o SysInstall/devices.o \
	SysInstall/media.o SysInstall/msg.o \
	SysInstall/variable.o SysInstall/misc.o \
	SysInstall/System.o SysInstall/tape.o 
#SysInstall/config.o SysInstall/network.o 
#SRCS = fdisk/llseek.cpp mount/fstab.cpp mount/mntent.cpp mount/sundries.cpp avp.cpp disk.cpp dword.cpp kernel.cpp contain.cpp profile.cpp redir.cpp report.cpp start.cpp 
#OBJS = fdisk/llseek.o   mount/fstab.o   mount/mntent.o   mount/sundries.o   avp.o   disk.o   dword.o   kernel.o   contain.o   profile.o   redir.o   report.o   start.o   
SRCS = avp.cpp disk.cpp contain.cpp dword.cpp kernel.cpp profile.cpp redir.cpp report.cpp 
#start.cpp 
OBJS = avp.o   disk.o   contain.o   dword.o   kernel.o   profile.o   redir.o   report.o
# start.o   

GRAF    = ../BaseWork/Avpmem.cpp ../BaseWork/BaseAPI.cpp \
	../BaseWork/Basework.cpp ../../CommonFiles/Stuff/_carray.cpp \
	../BaseWork/Clink.cpp ../../CommonFiles/Stuff/Sq_u.cpp \
	../../CommonFiles/Stuff/CalcSum.cpp ../BaseWork/_printf.cpp \
	../BaseWork/ImgSect.c
GRAFOBJ = ../BaseWork/Avpmem.o   ../BaseWork/BaseAPI.o   \
	../BaseWork/Basework.o   ../../CommonFiles/Stuff/_carray.o   \
	../BaseWork/Clink.o   ../../CommonFiles/Stuff/Sq_u.o   \
	../../CommonFiles/Stuff/CalcSum.o   ../BaseWork/_printf.o   \
	../BaseWork/ImgSect.o

KLDT    = loadkeys.cpp \
	../../CommonFiles/Serialize/Deser.c \
	../../CommonFiles/Serialize/DeserHdr.c \
	../../CommonFiles/Serialize/DeserR.c \
	../../CommonFiles/Serialize/SeriaInt.c
KLDTOBJ = loadkeys.o \
	../../CommonFiles/Serialize/Deser.o \
	../../CommonFiles/Serialize/DeserHdr.o \
	../../CommonFiles/Serialize/DeserR.o \
	../../CommonFiles/Serialize/SeriaInt.o

SWM	= ../../CommonFiles/Serialize/DeserSWM.c \
	../../CommonFiles/SWManager/Checker.c \
	../../CommonFiles/SWManager/CheckerR.c \
	../../CommonFiles/SWManager/Encrypt.c \
	../../CommonFiles/SWManager/EncryptR.c \
	../../CommonFiles/SWManager/Manager.c \
	../../CommonFiles/SWManager/ManagerR.c \
	../../CommonFiles/SWManager/Packer.c \
	../../CommonFiles/SWManager/PackerR.c \
	../../CommonFiles/SWManager/RawData.c \
	../../CommonFiles/SWManager/RawDataR.c 
SWMOBJ	= ../../CommonFiles/Serialize/DeserSWM.o \
	../../CommonFiles/SWManager/Checker.o \
	../../CommonFiles/SWManager/CheckerR.o \
	../../CommonFiles/SWManager/Encrypt.o \
	../../CommonFiles/SWManager/EncryptR.o \
	../../CommonFiles/SWManager/Manager.o \
	../../CommonFiles/SWManager/ManagerR.o \
	../../CommonFiles/SWManager/Packer.o \
	../../CommonFiles/SWManager/PackerR.o \
	../../CommonFiles/SWManager/RawData.o \
	../../CommonFiles/SWManager/RawDataR.o 

PROP    = ../../CommonFiles/Property/CRC.c \
	../../CommonFiles/Property/d_add.c \
	../../CommonFiles/Property/d_add_pr.c \
	../../CommonFiles/Property/d_attach.c \
	../../CommonFiles/Property/d_deser.c \
	../../CommonFiles/Property/d_detach.c \
	../../CommonFiles/Property/d_find.c \
	../../CommonFiles/Property/d_find_p.c \
	../../CommonFiles/Property/d_free.c \
	../../CommonFiles/Property/d_get.c \
	../../CommonFiles/Property/d_getval.c \
	../../CommonFiles/Property/d_init.c \
	../../CommonFiles/Property/d_remove.c \
	../../CommonFiles/Property/d_setval.c \
	../../CommonFiles/Property/sec.c \
	../../CommonFiles/Property/p_arr.c \
	../../CommonFiles/Property/p_catch.c \
	../../CommonFiles/Property/p_init.c \
	../../CommonFiles/Property/p_predct.c \
	../../CommonFiles/Property/p_get.c \
	../../CommonFiles/Property/p_free.c \
	../../CommonFiles/Property/pa_gitms.c \
	../../CommonFiles/Property/pa_insrt.c
PROPOBJ = ../../CommonFiles/Property/CRC.o \
	../../CommonFiles/Property/d_add.o \
	../../CommonFiles/Property/d_add_pr.o \
	../../CommonFiles/Property/d_attach.o \
	../../CommonFiles/Property/d_deser.o \
	../../CommonFiles/Property/d_detach.o \
	../../CommonFiles/Property/d_find.o \
	../../CommonFiles/Property/d_find_p.o \
	../../CommonFiles/Property/d_free.o \
	../../CommonFiles/Property/d_get.o \
	../../CommonFiles/Property/d_getval.o \
	../../CommonFiles/Property/d_init.o \
	../../CommonFiles/Property/d_remove.o \
	../../CommonFiles/Property/d_setval.o \
	../../CommonFiles/Property/sec.o \
	../../CommonFiles/Property/p_arr.o \
	../../CommonFiles/Property/p_catch.o \
	../../CommonFiles/Property/p_init.o \
	../../CommonFiles/Property/p_predct.o \
	../../CommonFiles/Property/p_get.o \
	../../CommonFiles/Property/p_free.o \
	../../CommonFiles/Property/pa_gitms.o\
	../../CommonFiles/Property/pa_insrt.o

SIGN    = ../../CommonFiles/Sign/signDumm.cpp
SIGNOBJ = ../../CommonFiles/Sign/signDumm.o

LIB  = #-L/usr/local/lib/gcc-lib/i386-pc-bsdi3.1/2.95.1
#/usr/lib/libncurses.so
INCLUDE =
DEFINES =-DNOAFX -D__LINUX__
#GCC    = shlicc   #gcc
#GCPP   = shlicc++ #g++
GCC    = egcs #/usr/local/bin/gcc
GCPP   = egcs++ ##/usr/local/bin/g++
CFLAGS = -Wall -Wstrict-prototypes -O2 #-g -N -s -g 
CXXFLAGS = -Wall -Wstrict-prototypes -O2 #-g -N -s -g -fno-implicit-templates bootstrap -fomit-frame-pointer
GFLAGS = 
#LINKFLAG= -static 

#install-strip:
#	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install
#installdirs: installdirs-recursive

# Installation path
INSTPATH=/usr/local/bin

.SUFFIXES: .c .cpp .o

.c.o:
	$(GCC) $(CFLAGS) $(DEFINES) $(EXTRA_CFLAGS) -c -o $@ $<

.cpp.o:
	$(GCPP) $(CXXFLAGS) $(DEFINES) $(EXTRA_CFLAGS) -c -o $@ $<

all:    $(PROG)

$(PROG): $(OBJS) $(GRAFOBJ) $(KLDTOBJ) $(SIGNOBJ) $(PROPOBJ) #$(SINSTALLOBJ) 
#	mv $(PROG) o$(PROG)
	rm -f *.core
#	$(GCPP) $(CXXFLAGS) $(DEFINES) $(OBJS) $(SINSTALLOBJ) $(GRAFOBJ) $(KLDTOBJ) $(SIGNOBJ) $(PROPOBJ) -o $(PROG) $(LINKFLAG) $(LIB)
	$(GCPP) $(CXXFLAGS) $(DEFINES) $(OBJS) $(GRAFOBJ) $(KLDTOBJ) $(SIGNOBJ) $(PROPOBJ) -o $(PROG) $(LINKFLAG) $(LIB)
#	strip -d -o $(PROG)
#	strip -d $(PROG)
	strip $(PROG)

#avp.o kernel.o profile.o redir.o report.o start.o : avp_os2.h
#avp.o disk.o profile.o contain.o kernel.o redir.o report.o start.o : disk.h
#avp.o kernel.o profile.o redir.o report.o : kernel.h
#avp.o contain.o kernel.o profile.o loadkeys.o start.o : loadkeys.h
#../kldt/KLDataTreeDeserialize.o start.o : ../kldt/AvpCommonIDs.h

clean:
	rm -f $(PROG)
	rm -f *.core
	rm -f *.o
	rm -f SysInstall/*.o
	rm -f ../*.o
	rm -f ../AvpBase/*.o
	rm -f ../BaseWork/*.o
	rm -f ../../CommonFiles/Property/*.o
	rm -f ../../CommonFiles/Serialize/*.o
	rm -f ../../CommonFiles/SWManager/*.o
	rm -f ../../CommonFiles/Sign/*.o
	rm -f ../../CommonFiles/Stuff/*.o
	rm -f ../kldt/*.o
	rm -f ../sign/*.o

#$(PROG): first.o
#       mv $(PROG) o$(PROG)
#       $(LINK) -o  
#     $(CC) $(OBJS) -o $@ $(LIB)
#       c++ first.o -o first

#first.o: first.c
#     $(CC) -c $(INCLUDE) $(DEFINES) first.c
 
#install: all symlinks.8
#	install -m 755 -o root -g root symlinks /usr/local/bin
#	install -m 644 -o root -g root symlink
#OCOAs.8 /usr/local/man/man8

#For example, here is a way to print the names of all of the unstripped
#binaries in the @file{/usr/local} directory tree.  Builtin tests avoid
#running @code{file} on files that are not regular files or are not
#executable.

#@example
#find /usr/local -type f -perm +a=x | xargs file | 
#  grep 'not stripped' | cut -d: -f1
#@end example

#Here is a shell script called @code{unstripped} that checks whether its
#argument is an unstripped binary file:
#
#@example
##!/bin/sh
#file $1 | grep 'not stripped' > /dev/null
#@end example

