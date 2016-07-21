# Microsoft Developer Studio Project File - Name="TreeSearchLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TreeSearchLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TreeSearchLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TreeSearchLib.mak" CFG="TreeSearchLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TreeSearchLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeSearchLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/KLAVA/klava/kernel/TreeSearchLib", NDVGAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TreeSearchLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\klava\kernel\TreeSearchLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /Gr /MD /W3 /Zi /O2 /I "..\include" /I "..\..\..\Prague\include" /I "..\..\..\CommonFiles" /I "..\..\.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D _FORCEINLINE=__forceinline /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Out\Release\TreeSearch.lib"

!ELSEIF  "$(CFG)" == "TreeSearchLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\klava\kernel\TreeSearchLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /Gr /MDd /W3 /Zi /Od /Gf /Gy /I "..\include" /I "..\..\..\Prague\include" /I "..\..\..\CommonFiles" /I "..\..\.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Out\Debug\TreeSearch.lib"

!ENDIF 

# Begin Target

# Name "TreeSearchLib - Win32 Release"
# Name "TreeSearchLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "AKTree"

# PROP Default_Filter "c,h"
# Begin Source File

SOURCE=.\AKTree\_AVL.H
# End Source File
# Begin Source File

SOURCE=.\AKTree\AKTYPES.H
# End Source File
# Begin Source File

SOURCE=.\aktree\avl.c
# End Source File
# Begin Source File

SOURCE=.\AKTree\AVL.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\dynamic.c
# End Source File
# Begin Source File

SOURCE=.\dynarray.c
# End Source File
# Begin Source File

SOURCE=.\dynmodif.c
# End Source File
# Begin Source File

SOURCE=.\enum.c
# End Source File
# Begin Source File

SOURCE=.\hex2bin.c
# End Source File
# Begin Source File

SOURCE=.\loaddyn.c

!IF  "$(CFG)" == "TreeSearchLib - Win32 Release"

# ADD CPP /Zi /O2

!ELSEIF  "$(CFG)" == "TreeSearchLib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\loadstat.c
# End Source File
# Begin Source File

SOURCE=.\rtnodes.c
# End Source File
# Begin Source File

SOURCE=.\savedyn.c
# End Source File
# Begin Source File

SOURCE=.\savestat2.c
# End Source File
# Begin Source File

SOURCE=.\search.c
# End Source File
# Begin Source File

SOURCE=.\static.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\treedbg.c
# End Source File
# Begin Source File

SOURCE=.\treemain.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\crc32.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\internal.h
# End Source File
# Begin Source File

SOURCE=.\savedyn.h
# End Source File
# Begin Source File

SOURCE=.\tree.h
# End Source File
# Begin Source File

SOURCE=.\tree_old.h
# End Source File
# Begin Source File

SOURCE=.\treedefs.h
# End Source File
# End Group
# End Target
# End Project
