# Microsoft Developer Studio Project File - Name="Mail Checker2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Mail Checker2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MC2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MC2.mak" CFG="Mail Checker2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mail Checker2 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Mail Checker2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Mail Checker2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/MC2", QVBLAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mail Checker2 - Win32 ReleaseWithTrace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseWithTrace"
# PROP BASE Intermediate_Dir "ReleaseWithTrace"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseWithTrace"
# PROP Intermediate_Dir "ReleaseWithTrace"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\prague\include" /I "..\..\..\..\windows\hook" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 prremote.lib, fsdrvlib.lib ws2_32.lib Rpcrt4.lib fssync.lib prremote.lib /nologo /entry:"DllMain" /subsystem:windows /dll /pdb:"../ReleaseWithTrace/sc.pdb" /debug /machine:I386 /out:"../ReleaseWithTrace/mc.ppl" /implib:"" /pdbtype:sept /libpath:"..\..\prague\release"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetPath=\PPP\MailCommon\ReleaseWithTrace\mc.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Mail Checker2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/ppp/mc"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\..\..\\" /I "..\..\include" /I "..\..\..\prague\include" /I "..\..\..\..\..\windows\hook" /I "..\..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 advapi32.lib user32.lib kernel32.lib prremote.lib fsdrvlib.lib ws2_32.lib Rpcrt4.lib fssync.lib /nologo /base:"0x64e00000" /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Release/mc.ppl" /libpath:"..\..\..\out\release" /libpath:"..\..\..\commonfiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\mc.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Mail Checker2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/ppp/mc"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\\" /I "..\..\include" /I "..\..\..\prague\include" /I "..\..\..\..\..\windows\hook" /I "..\..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib user32.lib kernel32.lib prremote.lib fsdrvlib.lib ws2_32.lib Rpcrt4.lib fssync.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/mc.ppl" /implib:"" /pdbtype:sept /libpath:"..\..\..\out\debug" /libpath:"..\..\..\commonfiles\debugdll"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Mail Checker2 - Win32 ReleaseWithTrace"
# Name "Mail Checker2 - Win32 Release"
# Name "Mail Checker2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MailTask\mailtask.cpp
# End Source File
# Begin Source File

SOURCE=.\MC2.rc
# End Source File
# Begin Source File

SOURCE=.\MailTask\plugin_mctask.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MailTask\p_mctask.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "ThreadImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ThreadImp.cpp
# End Source File
# Begin Source File

SOURCE=..\ThreadImp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MCTask.meta
# End Source File
# Begin Source File

SOURCE=ReadMe.txt
# End Source File
# End Target
# End Project
