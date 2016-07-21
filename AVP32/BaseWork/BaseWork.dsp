# Microsoft Developer Studio Project File - Name="BaseWork" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=BaseWork - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BaseWork.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BaseWork.mak" CFG="BaseWork - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BaseWork - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseWork - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseWork - Win32 Release Static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/BaseWork", RDDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BaseWork - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "..\..\temp\Release\AVP32\BaseWork"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\Debug"
# PROP Intermediate_Dir "..\..\temp\Debug\AVP32\BaseWork"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseWork___Win32_Release_Static"
# PROP BASE Intermediate_Dir "BaseWork___Win32_Release_Static"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release_static"
# PROP Intermediate_Dir "..\..\temp\release_static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Yu"stdafx.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "BaseWork - Win32 Release"
# Name "BaseWork - Win32 Debug"
# Name "BaseWork - Win32 Release Static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.cpp
# End Source File
# Begin Source File

SOURCE=.\_printf.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\Basework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Clink.cpp
# End Source File
# Begin Source File

SOURCE=.\DupInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\DupMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\IOCache\FileIO.cpp

!IF  "$(CFG)" == "BaseWork - Win32 Release"

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImgSect.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\pagememorymanager\pagememorymanager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SpcAlloc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\suballoc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.h
# End Source File
# Begin Source File

SOURCE=.\_printf.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Avp_dll.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=..\Bases\Format\Base.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\BaseAPI.h
# End Source File
# Begin Source File

SOURCE=.\Basework.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\byteorder.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.h
# End Source File
# Begin Source File

SOURCE=.\Clink.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\FileFormat\Coff.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\defines.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\DupMem\DupMem.h
# End Source File
# Begin Source File

SOURCE=..\bases\Method.h
# End Source File
# Begin Source File

SOURCE=..\pagememorymanager\pagememorymanager.h
# End Source File
# Begin Source File

SOURCE=..\Bases\Format\Records.h
# End Source File
# Begin Source File

SOURCE=..\bases\Retcode3.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\RetFlags.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Scanobj.h
# End Source File
# Begin Source File

SOURCE=..\bases\Sizes.h
# End Source File
# Begin Source File

SOURCE=.\SpcAlloc.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\IOCache\TFileData.h
# End Source File
# Begin Source File

SOURCE=..\bases\Format\Typedef.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Types.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# Begin Source File

SOURCE=.\Workarea.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseWork.dep
# End Source File
# Begin Source File

SOURCE=.\BaseWork.mak
# End Source File
# End Target
# End Project
