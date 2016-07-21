# Microsoft Developer Studio Project File - Name="klsys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=klsys - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klsys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klsys.mak" CFG="klsys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klsys - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "klsys - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "klsys - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "klsys - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/klsys", RPBMAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klsys - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDll"
# PROP BASE Intermediate_Dir "DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\Debug"
# PROP Intermediate_Dir "..\..\Temp\debug\commonfiles\klsys"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\out\Debug\klsys.lib"
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "klsys - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDll"
# PROP BASE Intermediate_Dir "ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "..\..\Temp\release\commonfiles\klsys\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "klsys - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "klsys___Win32_Debug"
# PROP BASE Intermediate_Dir "klsys___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "klsys___Win32_Debug"
# PROP Intermediate_Dir "klsys___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MDd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MTd /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\Debugs\klsys.lib"

!ELSEIF  "$(CFG)" == "klsys - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "klsys___Win32_Release"
# PROP BASE Intermediate_Dir "klsys___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "klsys___Win32_Release"
# PROP Intermediate_Dir "klsys___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MT /W3 /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\Release\klsys.lib"

!ENDIF 

# Begin Target

# Name "klsys - Win32 DebugDll"
# Name "klsys - Win32 ReleaseDll"
# Name "klsys - Win32 Debug"
# Name "klsys - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\direnum\direnum_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\errors\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\filemapping\filemapping_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\memorymanager\memorymanager.cpp
# End Source File
# Begin Source File

SOURCE=.\virtualmemory\virtualmemory_win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\direnum\direnum.h
# End Source File
# Begin Source File

SOURCE=.\guards\dummyguard.h
# End Source File
# Begin Source File

SOURCE=.\errors\errors.h
# End Source File
# Begin Source File

SOURCE=.\filemapping\filemapping.h
# End Source File
# Begin Source File

SOURCE=.\memorymanager\guardedmm.h
# End Source File
# Begin Source File

SOURCE=.\memorymanager\memorymanager.h
# End Source File
# Begin Source File

SOURCE=.\guards\posixguard.h
# End Source File
# Begin Source File

SOURCE=.\virtualmemory\virtualmemory.h
# End Source File
# Begin Source File

SOURCE=.\guards\winguard.h
# End Source File
# End Group
# End Target
# End Project
