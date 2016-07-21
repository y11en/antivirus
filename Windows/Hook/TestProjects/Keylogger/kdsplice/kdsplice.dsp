# Microsoft Developer Studio Project File - Name="kdsplice" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kdsplice - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kdsplice.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kdsplice.mak" CFG="kdsplice - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kdsplice - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kdsplice - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "kdsplice"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kdsplice - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KDSPLICE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KDSPLICE_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "kdsplice - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KDSPLICE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /Gz /W3 /WX /Z7 /Oi /Gy /I "i386\\" /I "." /I "$(NTMAKEENV)\..\inc\mfc42" /I "objchk_w2K_x86\i386" /I "$(NTMAKEENV)\..\inc\w2K" /I "$(NTMAKEENV)\..\inc\ddk\w2K" /I "$(NTMAKEENV)\..\inc\ifs\w2K" /I "$(NTMAKEENV)\..\inc\crt" /FI"$(NTMAKEENV)\..\inc\w2K\warning.h" /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WINVER=0x0500 /D _WIN32_IE=0x0501 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D __BUILDMACHINE__=WinDDK /D FPO=0 /D "NDEBUG" /D _DLL=1 /Zl -cbstring /GF /GS /QIfdiv- /hotpatch /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(NTMAKEENV)\..\lib\w2K\i386\BufferOverflowK.lib $(NTMAKEENV)\..\lib\w2K\i386\ntoskrnl.lib $(NTMAKEENV)\..\lib\w2K\i386\hal.lib $(NTMAKEENV)\..\lib\w2K\i386\wmilib.lib $(NTMAKEENV)\..\lib\w2K\i386\sehupd.lib /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"GsDriverEntry@8" /incremental:no /pdb:"objchk_w2K_x86\i386\kdsplice.pdb" /debug /machine:IX86 /nodefaultlib /out:"objchk_w2K_x86\i386\kdsplice.sys" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -IGNORE:4198,4010,4037,4039,4065,4070,4078,4087,4089,4221 -FULLBUILD /release /WX -osversion:5.0 /functionpadmin:5 /pdbcompress -driver -align:0x80 /stub:$(NTMAKEENV)\..\lib\w2K\stub512.com -subsystem:native,5.00 /SECTION:.exdata,ERW
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild.cmd
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "kdsplice - Win32 Release"
# Name "kdsplice - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\kdprint\kernel\kdprint.c
DEP_CPP_KDPRI=\
	"..\kdprint\kdshared.h"\
	"..\kdprint\kernel\kdprint.h"\
	"..\kdprint\kernel\kdworkqueue.h"\
	"C:\WINDDK\3790~1.141\inc\crt\specstrings.h"\
	"C:\WINDDK\3790~1.141\inc\ddk\w2K\ntnls.h"\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\ntifs.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\basetsd.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\bugcodes.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\guiddef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ia64reg.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntdef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntiologc.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntstatus.h"\
	
NODEP_CPP_KDPRI=\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\alpharef.h"\
	
# End Source File
# Begin Source File

SOURCE=.\kdsplice.c
DEP_CPP_KDSPL=\
	"..\kdprint\kdctl.h"\
	"..\kdprint\kdshared.h"\
	"..\kdprint\kernel\kdprint.h"\
	"C:\WINDDK\3790~1.141\inc\crt\specstrings.h"\
	"C:\WINDDK\3790~1.141\inc\ddk\w2K\ntnls.h"\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\ntifs.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\basetsd.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\bugcodes.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\guiddef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ia64reg.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntddkbd.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntdef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntiologc.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntstatus.h"\
	
NODEP_CPP_KDSPL=\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\alpharef.h"\
	
# End Source File
# Begin Source File

SOURCE=..\kdprint\kernel\kdworkqueue.c
DEP_CPP_KDWOR=\
	"..\kdprint\kernel\kdworkqueue.h"\
	"C:\WINDDK\3790~1.141\inc\crt\specstrings.h"\
	"C:\WINDDK\3790~1.141\inc\ddk\w2K\ntnls.h"\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\ntifs.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\basetsd.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\bugcodes.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\guiddef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ia64reg.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntdef.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntiologc.h"\
	"C:\WINDDK\3790~1.141\inc\w2K\ntstatus.h"\
	
NODEP_CPP_KDWOR=\
	"C:\WINDDK\3790~1.141\inc\ifs\w2K\alpharef.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
