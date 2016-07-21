# Microsoft Developer Studio Project File - Name="nfio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nfio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nfio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nfio.mak" CFG="nfio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nfio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nfio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague2/nfio", GGBBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nfio - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/nfio"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/nfio"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /FD /c
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
# ADD LINK32 user32.lib Advapi32.lib /nologo /base:"0x65b00000" /dll /debug /machine:I386 /out:"../../out/Release/nfio.ppl" /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\nfio.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/nfio"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/nfio"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /FR /FD /GZ /c
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
# ADD LINK32 user32.lib Advapi32.lib /nologo /base:"0xAD700000" /dll /map:"../../out/Debug/nfio.map" /debug /machine:I386 /out:"../../out/Debug/nfio.ppl" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "nfio - Win32 Release"
# Name "nfio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\plugin_win32_nfio.c
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# Begin Source File

SOURCE=.\win32_io.cpp
# End Source File
# Begin Source File

SOURCE=.\win32_objptr.c
# End Source File
# Begin Source File

SOURCE=.\win32_os.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\IFace\i_buffer.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_impex.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_io.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_objptr.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_os.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_reg.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\I_ROOT.H
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_seqio.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_serializer.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_string.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_win32_nfio.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\PR_ERR.H
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\PR_IID.H
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_int.h
# End Source File
# Begin Source File

SOURCE=..\Include\Pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_oid.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\PR_PID.H
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_serializable.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_types.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\PR_VID.H
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\Pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\PRAGUE.H
# End Source File
# Begin Source File

SOURCE=.\win32_io.h
# End Source File
# Begin Source File

SOURCE=.\win32_objptr.h
# End Source File
# Begin Source File

SOURCE=.\win32_os.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\nfio.imp
# End Source File
# Begin Source File

SOURCE=.\nfio.meta
# End Source File
# Begin Source File

SOURCE=.\nfio.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\nfio.dep
# End Source File
# Begin Source File

SOURCE=.\nfio.mak
# End Source File
# End Target
# End Project
