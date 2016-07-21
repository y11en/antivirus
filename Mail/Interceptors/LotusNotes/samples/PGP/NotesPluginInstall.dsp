# Microsoft Developer Studio Project File - Name="NotesPluginInstall" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NotesPluginInstall - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NotesPluginInstall.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NotesPluginInstall.mak" CFG="NotesPluginInstall - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NotesPluginInstall - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NotesPluginInstall - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NotesPluginInstall - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\build\win32\release"
# PROP Intermediate_Dir "releaseInstaller"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "W32" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib notes.lib /nologo /version:0.0 /subsystem:windows /dll /pdb:none /machine:I386 /out:"..\..\..\build\win32\release\PGPinstl.dll"

!ELSEIF  "$(CFG)" == "NotesPluginInstall - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\win32\debug"
# PROP Intermediate_Dir "debugInstaller"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "W32" /D "_WINDOWS" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"debugInstaller\PGPinstl.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib notes.lib /nologo /version:0.0 /stack:0x400,0x100 /subsystem:windows /dll /pdb:"debugInstaller/PGPinstl.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\build\win32\debug\PGPinstl.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "NotesPluginInstall - Win32 Release"
# Name "NotesPluginInstall - Win32 Debug"
# Begin Source File

SOURCE=.\DbAndNoteHandling.c
# End Source File
# Begin Source File

SOURCE=.\DbAndNoteHandling.h
# End Source File
# Begin Source File

SOURCE=.\Installer.c

!IF  "$(CFG)" == "NotesPluginInstall - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "NotesPluginInstall - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Installer.h
# End Source File
# Begin Source File

SOURCE=.\InstallerResource\InstallerResource.rc
# End Source File
# Begin Source File

SOURCE=.\ItemHandling.c
# End Source File
# Begin Source File

SOURCE=.\ItemHandling.h
# End Source File
# Begin Source File

SOURCE=.\LibDbAndNote.h
# End Source File
# Begin Source File

SOURCE=.\LibItem.h
# End Source File
# Begin Source File

SOURCE=.\LibRichText.h
# End Source File
# Begin Source File

SOURCE=.\LibString.h
# End Source File
# Begin Source File

SOURCE=.\LinkListHandling.c
# End Source File
# Begin Source File

SOURCE=.\NotesApiGlobals.c
# End Source File
# Begin Source File

SOURCE=.\NotesApiGlobals.h
# End Source File
# Begin Source File

SOURCE=.\PGPinstl.def
# End Source File
# Begin Source File

SOURCE=.\PstGlobals.c
# End Source File
# Begin Source File

SOURCE=.\PstGlobals.h
# End Source File
# Begin Source File

SOURCE=.\RichTextHandling.c
# End Source File
# Begin Source File

SOURCE=.\RichTextHandling.h
# End Source File
# Begin Source File

SOURCE=.\StringHandling.c
# End Source File
# End Target
# End Project
