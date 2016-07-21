# Microsoft Developer Studio Project File - Name="basegui_u" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=basegui_u - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "basegui_u.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basegui_u.mak" CFG="basegui_u - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basegui_u - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basegui_u - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/basegui", ZPBABAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basegui_u - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/basegui_u"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\.\CommonFiles" /I "../../prague/include" /I "../include" /I "../../prague" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\include" /i "..\..\.\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /base:"0x62600000" /dll /debug /machine:I386 /out:"../../out/Release/basegui.ppl" /pdbtype:sept /libpath:"..\..\out\Release" /libpath:"$(QTDIR)/lib" /opt:ref
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "basegui_u - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/basegui_u"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../prague/include" /I "../include" /I "../../prague" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_MBCS" /D "_USRDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\include" /i "..\..\.\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"../../out/Debug/basegui.ppl" /libpath:"../../out/Debug" /libpath:"$(QTDIR)/lib"
# SUBTRACT LINK32 /pdb:none /map
# Begin Custom Build - Performing registration
OutDir=.\../../out/Debug
TargetPath=\out\Debug\basegui.ppl
InputPath=\out\Debug\basegui.ppl
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "basegui_u - Win32 Release"
# Name "basegui_u - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\basegui.cpp
# End Source File
# Begin Source File

SOURCE=.\binding.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrls.cpp
# End Source File
# Begin Source File

SOURCE=.\Formats.cpp
# End Source File
# Begin Source File

SOURCE=.\Graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\prague\IniLib\IniRead.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemProps.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemRoot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\AVP32\pagememorymanager\pagememorymanager.cpp
# End Source File
# Begin Source File

SOURCE=.\VListCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\basegui.h
# End Source File
# Begin Source File

SOURCE=.\binding.h
# End Source File
# Begin Source File

SOURCE=.\Ctrls.h
# End Source File
# Begin Source File

SOURCE=.\DataSource.h
# End Source File
# Begin Source File

SOURCE=.\Formats.h
# End Source File
# Begin Source File

SOURCE=.\Graphics.h
# End Source File
# Begin Source File

SOURCE=.\GuiLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\IniLib\IniRead.h
# End Source File
# Begin Source File

SOURCE=.\ItemBase.h
# End Source File
# Begin Source File

SOURCE=.\ItemProps.h
# End Source File
# Begin Source File

SOURCE=.\ItemRoot.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\wingui\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\wingui\linkhand.cur
# End Source File
# Begin Source File

SOURCE=.\wingui\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\wingui\resource.h
# End Source File
# Begin Source File

SOURCE=.\wingui\sortdown.bmp
# End Source File
# Begin Source File

SOURCE=.\wingui\sortpdown.bmp
# End Source File
# Begin Source File

SOURCE=.\wingui\sortpup.bmp
# End Source File
# Begin Source File

SOURCE=.\wingui\sortup.bmp
# End Source File
# Begin Source File

SOURCE=.\WinGUI\wingui.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\basegui_u.dep
# End Source File
# Begin Source File

SOURCE=.\basegui_u.mak
# End Source File
# End Target
# End Project
