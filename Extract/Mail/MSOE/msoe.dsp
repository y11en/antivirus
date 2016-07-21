# Microsoft Developer Studio Project File - Name="msoe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=msoe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "msoe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msoe.mak" CFG="msoe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msoe - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msoe - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/MSOE", HKBDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "msoe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/MSOE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/MSOE"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include\iface" /I "..\include" /I ".\MSOEAPI" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /D "C_STYLE_PROP" /D "_USE_VTBL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib mapi32.lib ole32.lib user32.lib advapi32.lib /nologo /base:"0x65700000" /dll /debug /machine:I386 /out:"../../out/Release/msoe.ppl" /libpath:"..\..\CommonFiles\ReleaseDll" /IGNORE:4089 /opt:ref
# SUBTRACT LINK32 /pdb:none /map /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\msoe.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/MSOE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/MSOE"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include\iface" /I "..\include" /I ".\MSOEAPI" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /D "C_STYLE_PROP" /D "_USE_VTBL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib mapi32.lib ole32.lib user32.lib advapi32.lib /nologo /dll /map:"../../out/Debug/msoe.map" /debug /machine:I386 /out:"../../out/Debug/msoe.ppl" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "msoe - Win32 Release"
# Name "msoe - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\hook.cpp
# End Source File
# Begin Source File

SOURCE=.\msoe.cpp
# End Source File
# Begin Source File

SOURCE=.\msoe_base.cpp
# End Source File
# Begin Source File

SOURCE=.\msoe_io.cpp
# End Source File
# Begin Source File

SOURCE=.\msoe_objptr.cpp
# End Source File
# Begin Source File

SOURCE=.\msoe_os.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_msoe.cpp
# End Source File
# Begin Source File

SOURCE=.\stream.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\buff_prm.h
# End Source File
# Begin Source File

SOURCE=.\hook.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_mailmsg.h
# End Source File
# Begin Source File

SOURCE=.\msoe.h
# End Source File
# Begin Source File

SOURCE=.\msoe_base.h
# End Source File
# Begin Source File

SOURCE=.\msoeAPI.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_msoe.h
# End Source File
# Begin Source File

SOURCE=.\plugin_msoe.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\msoe.imp
# End Source File
# Begin Source File

SOURCE=.\msoe.meta
# End Source File
# Begin Source File

SOURCE=.\msoe.rc

!IF  "$(CFG)" == "msoe - Win32 Release"

!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"

# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419

!ENDIF 

# End Source File
# End Group
# Begin Group "MSOEAPI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSOEAPI\FolderInfo.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\FolderTableSchema.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\IDatabase.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\IDatabaseExtension.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\IDatabaseSession.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\IMimeMessageW.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\IVirtualStream.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\MessageInfo.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\MessageTableSchema.h
# End Source File
# Begin Source File

SOURCE=.\MSOEAPI\PropSym.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\msoe.dep
# End Source File
# Begin Source File

SOURCE=.\msoe.mak
# End Source File
# End Target
# End Project
