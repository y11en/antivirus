#////////////////////////////////////////////////////////////////////
#//
#//  ENGINEDT.MAK
#//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 2003
#//  Copyright (c) Kaspersky Labs.
#//
#////////////////////////////////////////////////////////////////////
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

#//////////////////////////////////////////////////
#// Main target
#//

TARGET=config\engine.dt

$(TARGET): config\engine.reg
	\prague\debug\dt_test.exe $(TARGET)
	tsigner  $(TARGET)

CLEAN :
	-@erase $(TARGET)

