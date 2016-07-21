
!IFNDEF MSVC16
!ERROR Environment variable MSVC16 should be defined. for ex:  MSVC16=D:\MSVC
!ENDIF


CFLAGS = /batch /nologo /c /Zl /D$(CFG) /I$(MSVC16)\Include /IO:\Commonfiles

TargetName=avp_stub

VER=..\..\CommonFiles\Version

$(CFG)\$(TargetName).exe: $(TargetName).c $(TargetName).mak $(VER)\ver_kl.h $(VER)\ver_avp.h $(VER)\ver_kl.ver
    @$(MSVC16)\bin\CL   $(CFLAGS) /Fo$(CFG)\$(TargetName).obj $(TargetName).c
    @$(MSVC16)\bin\LINK /batch /KNOWEAS $(CFG)\$(TargetName).obj, $(CFG)\$(TargetName).exe,,$(MSVC16)\lib\slibce.lib $(MSVC16)\lib\oldnames.lib,,
