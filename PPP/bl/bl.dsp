# Microsoft Developer Studio Project File - Name="bl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=bl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bl.mak" CFG="bl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "bl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/bl", AKZTAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "..\..\temp\Release/ppp/bl"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /I "..\..\updater60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /D "KLOPUM_INPLACE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\..\commonfiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib Kernel32.lib advapi32.lib Iphlpapi.lib /nologo /base:"0x62800000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\out/Release/bl.ppl" /libpath:"..\..\out\release" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\bl.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "bl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\Debug"
# PROP Intermediate_Dir "..\..\temp\Debug/ppp/bl"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /I "..\..\updater60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /D "KLOPUM_INPLACE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "..\..\commonfiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib Kernel32.lib advapi32.lib Iphlpapi.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\out/Debug/bl.ppl" /implib:"" /pdbtype:sept /libpath:"..\..\out\debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "bl - Win32 Release"
# Name "bl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bl.cpp
# End Source File
# Begin Source File

SOURCE=.\blloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Hook\Klop\kloplib\klop_util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Hook\Klop\kloplib\kloplib.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_bl.cpp
# End Source File
# Begin Source File

SOURCE=.\update.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bl.h
# End Source File
# Begin Source File

SOURCE=.\blloc.h
# End Source File
# Begin Source File

SOURCE=.\blmsg.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_productlogic.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_bl.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_bl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bl.imp
# End Source File
# Begin Source File

SOURCE=.\bl.meta
# End Source File
# Begin Source File

SOURCE=.\bl.rc
# End Source File
# Begin Source File

SOURCE=.\blmsg.mc

!IF  "$(CFG)" == "bl - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\blmsg.mc
InputName=blmsg

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rem IncrediBuild_RunFirst 
	mc $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "bl - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\blmsg.mc
InputName=blmsg

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rem IncrediBuild_RunFirst 
	mc $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\bl.dep
# End Source File
# Begin Source File

SOURCE=.\bl.mak
# End Source File
# End Target
# End Project
