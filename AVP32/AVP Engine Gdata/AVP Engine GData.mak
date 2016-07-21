#////////////////////////////////////////////////////////////////////
#//
#//  ENGINE.MAK
#//  AVP 3.0 Engine makefile
#//  Project AVP
#//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
#//  Copyright (c) Kaspersky Labs.
#//
#////////////////////////////////////////////////////////////////////
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 


OUTDIR_OBJ=\
..\..\out\Release\avp_io.vxd \
..\..\out\Release\avpbase.dll \
..\..\out\Release\avp_io32.dll \
..\..\out\Release\avp_ioNT.dll \
"..\AVP Engine\engine.rtf" \
files.lst \



#//////////////////////////////////////////////////
#// Main target
#//

TARGET=..\Release\gd_e.zip

$(TARGET): $(OUTDIR_OBJ)
	@WZZIP  -uP -ybc $(TARGET) @files.lst

CLEAN :
	-@erase $(TARGET)

