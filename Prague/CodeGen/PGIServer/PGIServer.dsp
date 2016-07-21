# Microsoft Developer Studio Project File - Name="PGIServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PGIServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PGIServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PGIServer.mak" CFG="PGIServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PGIServer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PGIServer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen/PGIServer", JVFBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PGIServer - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/PGIServer"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/PGIServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "../../../commonfiles" /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x417 /i "../../../windows/AVPControls" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klsys.lib DTUtils.lib SWM.lib KLDTSer.lib WinAvpIO.lib Property.lib Win32Utils.lib mpr.lib imagehlp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib metautils.lib ifaceinfo.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Debug/PGIServer.dll" /pdbtype:sept /libpath:"../../../out/debug"
# Begin Custom Build - Performing registration
OutDir=.\../../../out/Debug
TargetPath=\prague\Debug\PGIServer.dll
InputPath=\prague\Debug\PGIServer.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PGIServer - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/PGIServer"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/PGIServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../commonfiles" /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "../../../windows/AVPControls" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 klsys.lib DTUtils.lib SWM.lib KLDTSer.lib WinAvpIO.lib Property.lib Win32Utils.lib mpr.lib imagehlp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib metautils.lib ifaceinfo.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Release/PGIServer.dll" /libpath:"../../../out/release"
# Begin Custom Build - Performing registration
OutDir=.\../../../out/Release
TargetPath=\prague\Release\PGIServer.dll
InputPath=\prague\Release\PGIServer.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "PGIServer - Win32 Debug"
# Name "PGIServer - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowseForFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PGIServer.cpp
# End Source File
# Begin Source File

SOURCE=.\PGIServer.def
# End Source File
# Begin Source File

SOURCE=.\PGIServer.idl

!IF  "$(CFG)" == "PGIServer - Win32 Debug"

# ADD MTL /tlb ".\PGIServer.tlb" /h "..\PGIServerInterface.h" /iid "PGIServer_i.c" /Oicf

!ELSEIF  "$(CFG)" == "PGIServer - Win32 Release"

# ADD MTL /tlb ".\PGIServer.tlb" /h "PGIServer.h" /iid "PGIServer_i.c" /Oicf

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PGIServer.rc
# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419 /i "O:\Windows\AVPControls\\"
# End Source File
# Begin Source File

SOURCE=.\PragueIServer.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueIServerIntegrity.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueIServerLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueIServerSave.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueIServerSaveDB.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueIServerUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\PrepareDBTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SSCheckOut.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BrowseForFolder.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PragueIServer.h
# End Source File
# Begin Source File

SOURCE=.\PrepareDBTree.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\PragueIServer.rgs
# End Source File
# End Group
# Begin Source File

SOURCE=.\SSCheckOut.asp
# End Source File
# End Target
# End Project
