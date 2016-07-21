#////////////////////////////////////////////////////////////////////
#//
#//  OBJ.MAK
#//  AVP 3.0 Database internal O16 O32 makefile
#//  Project AVP
#//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
#//  Copyright (c) Kaspersky Labs.
#//
#////////////////////////////////////////////////////////////////////

!IFNDEF TargetName
!ERROR "TargetName should be defined (without extension)"
!ENDIF

!IFNDEF CommonFiles
!ERROR "CommonFiles should be defined"
!ENDIF

!IFNDEF VC32
VC32=D:\MVS\VC98
!ENDIF
!IFNDEF VC16
VC16=D:\MSVC
!ENDIF

!IFNDEF CFG
CFG=RELEASE
!ENDIF

CFLAGS = /nologo /c /Zl /Ox /I$(CommonFiles)\Bases /D$(CFG)

all: $(CFG)\$(TargetName).o32 $(CFG)\$(TargetName).o16

$(CFG)\$(TargetName).o32: $(DEP)
    @$(VC32)\bin\cl $(CFLAGS) /Fo$(CFG)\$(TargetName).o32 /Fa$(CFG)\$(TargetName)32.asm $(TargetName).c

$(CFG)\$(TargetName).o16: $(DEP)
    @$(VC16)\bin\cl $(CFLAGS) /Fo$(CFG)\$(TargetName).o16 /Fa$(CFG)\$(TargetName).asm $(TargetName).c  
    @$(VC16)\bin\ml /nologo /c /Cp /Zm      /Fo$(CFG)\$(TargetName).o16 $(CFG)\$(TargetName).asm


