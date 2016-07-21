#////////////////////////////////////////////////////////////////////
#//
#//  ENGINE.MAK
#//  KLE 3.0 Engine makefile
#//  Project KLE
#//  Alexey de Mont de Rique [graf@kaspersky.com] 
#//  Copyright (c) Kaspersky Labs. 2001
#//
#////////////////////////////////////////////////////////////////////
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 


OUTDIR_OBJ=\
..\Release\kle_app.exe \
..\Release\klh_sys.sys \
..\Release\kle_sys.sys \
..\Release\klh_vxd.vxd \
..\Release\kle_vxd.vxd \
..\Debug\kle_app.exe \
..\Debug\klh_sys.sys \
..\Debug\kle_sys.sys \
..\Debug\klh_vxd.vxd \
..\Debug\kle_vxd.vxd \
engine.mak \
files.lst \
readme.txt



#//////////////////////////////////////////////////
#// Main target
#//

TARGET=KLE_E.zip

$(TARGET): $(OUTDIR_OBJ)
	@WZZIP  -uP -ybc -sTerrence $(TARGET) @files.lst

CLEAN :
	-@erase $(TARGET)

