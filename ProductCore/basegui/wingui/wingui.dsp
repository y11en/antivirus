# Microsoft Developer Studio Project File - Name="wingui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wingui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wingui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wingui.mak" CFG="wingui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wingui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wingui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/basegui/wingui", QQBABAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wingui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/Wingui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\Prague\Include" /I "..\..\Include" /I "..\..\..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wingui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/ppp/wingui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Prague\Include" /I "..\..\Include" /I "..\..\..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wingui - Win32 Release"
# Name "wingui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SendMail.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\SysSink.cpp
# End Source File
# Begin Source File

SOURCE=.\VListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\WinCred.cpp
# End Source File
# Begin Source File

SOURCE=.\WinCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\WinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WinFileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\WinLang.cpp
# End Source File
# Begin Source File

SOURCE=.\WinRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\WinUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SendMail.h
# End Source File
# Begin Source File

SOURCE=.\ShellCtrls.h
# End Source File
# Begin Source File

SOURCE=.\SimpleThread.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysSink.h
# End Source File
# Begin Source File

SOURCE=.\WinCtrls.h
# End Source File
# Begin Source File

SOURCE=.\WinFileInfo.h
# End Source File
# Begin Source File

SOURCE=.\WinLang.h
# End Source File
# Begin Source File

SOURCE=.\WinRoot.h
# End Source File
# Begin Source File

SOURCE=.\WinUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\linkhand.cur
# End Source File
# Begin Source File

SOURCE=.\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sortdown.bmp
# End Source File
# Begin Source File

SOURCE=.\sortpdown.bmp
# End Source File
# Begin Source File

SOURCE=.\sortpup.bmp
# End Source File
# Begin Source File

SOURCE=.\sortup.bmp
# End Source File
# Begin Source File

SOURCE=.\wingui.rc
# End Source File
# End Group
# End Target
# End Project
