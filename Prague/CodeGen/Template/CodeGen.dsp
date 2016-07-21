# Microsoft Developer Studio Project File - Name="CodeGen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CodeGen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CodeGen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CodeGen.mak" CFG="CodeGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CodeGen - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CodeGen - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CodeGen - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/Template"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/Template"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\CommonFiles" /I "..\..\include" /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib property.lib kldtser.lib swm.lib dtutils.lib ifaceinfo.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/CodeGen.dll" /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll;../../Debug"
# Begin Custom Build - Invoking Version Control
IntDir=.\Debug
ProjDir=.
TargetPath=\Prague\Debug\CodeGen.dll
InputPath=\Prague\Debug\CodeGen.dll
SOURCE="$(InputPath)"

"$(IntDir)\verctrl.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	VERSAVE $(TargetPath) $(ProjDir) 
	echo This is Version Control helper file. It is save to delete it. >$(IntDir)\verctrl.tmp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CodeGen - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/Template"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/Template"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GR /O2 /I "..\..\..\CommonFiles" /I "..\..\include" /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "NO_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib property.lib kldtser.lib swm.lib dtutils.lib ifaceinfo.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../../out/Release/CodeGen.dll" /libpath:"..\..\..\CommonFiles\ReleaseDll;../../Release"
# SUBTRACT LINK32 /debug
# Begin Custom Build - Invoking Version Control
IntDir=.\Release
ProjDir=.
TargetPath=\Prague\Release\CodeGen.dll
InputPath=\Prague\Release\CodeGen.dll
SOURCE="$(InputPath)"

"$(IntDir)\verctrl.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	VERSAVE $(TargetPath) $(ProjDir) 
	echo This is Version Control helper file. It is save to delete it. >$(IntDir)\verctrl.tmp 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "CodeGen - Win32 Debug"
# Name "CodeGen - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CodeGen.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGen.def
# End Source File
# Begin Source File

SOURCE=.\CodeGen.idl

!IF  "$(CFG)" == "CodeGen - Win32 Debug"

# ADD MTL /tlb "CodeGen.tlb" /h "..\PCGInterface.h" /iid "CodeGen_i.c" /Oicf

!ELSEIF  "$(CFG)" == "CodeGen - Win32 Release"

# ADD MTL /tlb ".\CodeGen.tlb" /h "..\PCGInterface.h" /iid "CodeGen_i.c" /Oicf

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CodeGen.rc
# End Source File
# Begin Source File

SOURCE=.\CodeGenApp.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AVPPveID.h
# End Source File
# Begin Source File

SOURCE=.\CodeGen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\KLDTSer.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=..\PCGInterface.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Swm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\CodeDescription.dsc
# End Source File
# Begin Source File

SOURCE=.\CodeGen.rgs
# End Source File
# Begin Source File

SOURCE=.\CodeGen.tlb
# End Source File
# Begin Source File

SOURCE=.\Options.klp
# End Source File
# End Target
# End Project
