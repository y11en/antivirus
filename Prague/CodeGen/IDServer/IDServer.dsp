# Microsoft Developer Studio Project File - Name="IDServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IDServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IDServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IDServer.mak" CFG="IDServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IDServer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IDServer - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IDServer - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen/IDServer", VHABAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IDServer - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/IDServer"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/IDServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "../../../commonfiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Property.lib DTUtils.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"o:\prague\debug/IDServer.dll" /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll"
# Begin Custom Build - Performing registration step
OutDir=.\../../../out/Debug
TargetPath=\prague\debug\IDServer.dll
InputPath=\prague\debug\IDServer.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rem regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "IDServer - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release MinSize"
# PROP BASE Intermediate_Dir "../../../temp/Release MinSize/prague/CodeGen/IDServer"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release MinSize"
# PROP Intermediate_Dir "../../../temp/Release MinSize/prague/CodeGen/IDServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../../../commonfiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Property.lib DTUtils.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../../Release/IDServer.dll" /libpath:"..\..\..\CommonFiles\ReleaseDll"
# Begin Custom Build - Performing registration
OutDir=.\../../../out/Release MinSize
TargetPath=\prague\Release\IDServer.dll
InputPath=\prague\Release\IDServer.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "IDServer - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release MinDependency"
# PROP BASE Intermediate_Dir "../../../temp/Release MinDependency/prague/CodeGen/IDServer"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release MinDependency"
# PROP Intermediate_Dir "../../../temp/Release MinDependency/prague/CodeGen/IDServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "../../../commonfiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Property.lib DTUtils.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"..\..\..\CommonFiles\ReleaseDll"
# Begin Custom Build - Performing registration
OutDir=.\../../../out/Release MinDependency
TargetPath=\out\Release MinDependency\IDServer.dll
InputPath=\out\Release MinDependency\IDServer.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rem regsvr32 /s /c "$(TargetPath)" 
	rem echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "IDServer - Win32 Debug"
# Name "IDServer - Win32 Release MinSize"
# Name "IDServer - Win32 Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ApartThread.cpp
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\IDServer.cpp
# End Source File
# Begin Source File

SOURCE=.\IDServer.def
# End Source File
# Begin Source File

SOURCE=.\IDServer.idl

!IF  "$(CFG)" == "IDServer - Win32 Debug"

# ADD MTL /tlb ".\IDServer.tlb" /h "..\IDServerInterface.h" /iid "IDServer_i.c" /Oicf

!ELSEIF  "$(CFG)" == "IDServer - Win32 Release MinSize"

# ADD MTL /tlb ".\IDServer.tlb" /h "IDServer.h" /iid "IDServer_i.c" /Oicf

!ELSEIF  "$(CFG)" == "IDServer - Win32 Release MinDependency"

# ADD MTL /tlb ".\IDServer.tlb" /h "..\IDServerInterface.h" /iid "IDServer_i.c" /Oicf

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDServer.rc
# End Source File
# Begin Source File

SOURCE=.\PragueIDServer.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\RegStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\SerDeser.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ApartThread.h
# End Source File
# Begin Source File

SOURCE=.\AVPPisID.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PragueIDServer.h
# End Source File
# Begin Source File

SOURCE=.\RegisterDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\RegStorage.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SerDeser.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\PragueIDServer.rgs
# End Source File
# End Group
# End Target
# End Project
