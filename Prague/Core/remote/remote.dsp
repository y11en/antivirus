# Microsoft Developer Studio Project File - Name="pr_remote" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pr_remote - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "remote.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "remote.mak" CFG="pr_remote - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_remote - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/remote", MDSDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pr_remote - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/remote"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/remote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_remote_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "idl" /I "../include" /I "../include/iface" /I "../../CommonFiles" /D "NDEBUG" /D "WIN32" /D "_PRAGUE_TRACE_" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 advapi32.lib Rpcrt4.lib fsdrvlib.lib kernel32.lib user32.lib hashutil.lib /nologo /base:"0x66600000" /dll /debug /machine:I386 /out:"../../out/Release/prremote.dll" /libpath:"../../out/Release" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\prremote.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/remote"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/remote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_remote_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "idl" /I "../include" /I "../include/iface" /I "../../CommonFiles" /D "_DEBUG" /D "WIN32" /D "_PRAGUE_TRACE_" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib Rpcrt4.lib fsdrvlib.lib kernel32.lib user32.lib hashutil.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/prremote.dll" /pdbtype:sept /libpath:"../../out/Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pr_remote - Win32 Release"
# Name "pr_remote - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pr_internal.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_remote.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_remote.rc
# End Source File
# Begin Source File

SOURCE=.\pr_root_c.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_system\pr_system_c.cpp
# End Source File
# Begin Source File

SOURCE=.\rpc_connect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Service\sa.cpp
# End Source File
# Begin Source File

SOURCE=.\SharedTable.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\pr_common.h
# End Source File
# Begin Source File

SOURCE=.\pr_manager.h
# End Source File
# Begin Source File

SOURCE=.\pr_process.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_remote.h
# End Source File
# Begin Source File

SOURCE=.\rpc_connect.h
# End Source File
# Begin Source File

SOURCE=.\SharedTable.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pr_remote.dep
# End Source File
# Begin Source File

SOURCE=.\pr_remote.mak
# End Source File
# End Target
# End Project
