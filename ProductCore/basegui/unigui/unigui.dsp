# Microsoft Developer Studio Project File - Name="unigui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=unigui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "unigui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "unigui.mak" CFG="unigui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "unigui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unigui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "unigui"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "unigui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/ppp/unigui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../prague/include" /I "../../include" /I "$(QTDIR)/Include" /I "$(QTDIR)/Include/QtGui" /I "$(QTDIR)/Include/QtCore" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "unigui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/ppp/unigui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../prague/include" /I "../../include" /I "$(QTDIR)/Include" /I "$(QTDIR)/Include/QtGui" /I "$(QTDIR)/Include/QtCore" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "unigui - Win32 Release"
# Name "unigui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SysSink.cpp
# End Source File
# Begin Source File

SOURCE=.\UniCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\UniFormats.cpp
# End Source File
# Begin Source File

SOURCE=.\UniGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\UniRoot.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SysSink.h
# End Source File
# Begin Source File

SOURCE=.\UniCtrls.h

!IF  "$(CFG)" == "unigui - Win32 Release"

# Begin Custom Build - Running MOC on $(InputPath)
InputPath=.\UniCtrls.h

"moc_ctrls.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)/bin/moc.exe"  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_EDITION=QT_EDITION_DESKTOP -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"../../../temp/Debug/ppp/unigui/moc" -I"." -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ctrls.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "unigui - Win32 Debug"

# Begin Custom Build - Running MOC on $(InputPath)
InputPath=.\UniCtrls.h

"moc_ctrls.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)/bin/moc.exe"  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_EDITION=QT_EDITION_DESKTOP -DQT_DLL -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"../../../temp/Debug/ppp/unigui/moc" -I"." -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ctrls.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UniRoot.h
# End Source File
# End Group
# End Target
# End Project
