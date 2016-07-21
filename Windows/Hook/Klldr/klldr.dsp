# Microsoft Developer Studio Project File - Name="klldr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klldr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klldr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klldr.mak" CFG="klldr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klldr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klldr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Personal Firewall/Drivers/klldr", EGPBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klldr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Ox /Ob2 /Gy /D "WIN32" /c
# ADD CPP /nologo /Gz /W3 /O2 /Ob0 /I "$(BASEDIR2K)\src\network\inc" /I "$(BASEDIR2K)\inc\ddk" /I "..\\" /I "$(basedir)\src\network\inc" /I "$(basedir)\inc" /I "..\KLSDK" /FI"$(BASEDIR2K)\inc\warning.h" /D "_NDEBUG" /D _WIN32_WINNT=0x0400 /D DBG=0 /D "_UNICODE" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D "NDIS40" /D "NDIS40_MINIPORT" /D BINARY_COMPATIBLE=0 /D "__CPP" /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(DDK2KROOT)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"Release\klldr.sys" /libpath:"$(BASEDIR2K)\Libfre\i386" /libpath:"..\..\\" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4006,4088,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /map /debug

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

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
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Z7 /Oi /Gy /I "$(basedir)\src\network\inc" /I "$(basedir)\inc" /I "..\KLSDK" /FI"$(BASEDIR2K)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D "_UNICODE" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D "NDIS40" /D "NDIS40_MINIPORT" /D BINARY_COMPATIBLE=0 /D "__CPP" /FR /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DDK2KROOT)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /pdb:"../../../../out/debug/klldr.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"../../../../out/debug/klldr.sys" /libpath:"$(BASEDIR2K)\libchk\i386" /libpath:"..\..\\" /driver /debug:notmapped,FULL /IGNORE:4088,4006,4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "klldr - Win32 Release"
# Name "klldr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc"
# End Group
# Begin Group "KLSDK src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\KLSDK\g_registry.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\g_thread.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_alloc.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_buffer.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_debug.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_file.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_hook.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_list.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_lock.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_log.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_object.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\KLSDK\kl_string.cpp

!IF  "$(CFG)" == "klldr - Win32 Release"

!ELSEIF  "$(CFG)" == "klldr - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "KLSDK inc"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
