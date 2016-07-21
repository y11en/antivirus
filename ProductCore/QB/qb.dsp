# Microsoft Developer Studio Project File - Name="qb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qb.mak" CFG="qb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/qb", NNSGAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/qb"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O1 /I "." /I "../include" /I "../../prague/include" /I "../../prague" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ods_EXPORTS" /D "C_STYLE_PROP" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libc.lib /nologo /base:"0x66a00000" /entry:"DllMain" /dll /debug /machine:I386 /nodefaultlib /out:"../../out/Release/qb.ppl" /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\qb.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "qb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/qb"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"DllMain" /dll /debug /machine:I386 /out:"../../out/Debug/qb.ppl" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "qb - Win32 Release"
# Name "qb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\plugin_qb.cpp
# End Source File
# Begin Source File

SOURCE=.\posio_sp.cpp
# End Source File
# Begin Source File

SOURCE=.\qb.cpp
# End Source File
# Begin Source File

SOURCE=.\qbrestorer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_engine.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_impex.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_io.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_list.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_objptr.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_os.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_posio_sp.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_qb.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_reg.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_report.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_root.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_seqio.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_string.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_qb.h
# End Source File
# Begin Source File

SOURCE=.\plugin_qb.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_err.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_iid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_int.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_pid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_types.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_vid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\prague.h
# End Source File
# Begin Source File

SOURCE=.\qbrestorer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\qb.meta
# End Source File
# Begin Source File

SOURCE=.\qb.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\qb.dep
# End Source File
# Begin Source File

SOURCE=.\qb.imp
# End Source File
# Begin Source File

SOURCE=.\qb.mak
# End Source File
# Begin Source File

SOURCE=..\Include\prt\qb.prt
# End Source File
# End Target
# End Project
