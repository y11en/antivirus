# Microsoft Developer Studio Project File - Name="Util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Util.mak" CFG="Util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/Util", YJEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Util - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/Util"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/Util"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /base:"0x69100000" /entry:"DllMain" /dll /debug /machine:I386 /out:"../../out/Release/PrUtil.ppl" /pdbtype:sept /opt:ref
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\Release\PrUtil.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Util - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/Util"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/Util"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W4 /Gm /GX /Zi /Od /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /incremental:no /debug /machine:I386 /out:"../../out/Debug/PrUtil.ppl" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Util - Win32 Release"
# Name "Util - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\list.c
# End Source File
# Begin Source File

SOURCE=.\lru.c
# End Source File
# Begin Source File

SOURCE=.\ltree.c
# End Source File
# Begin Source File

SOURCE=.\stack.c
# End Source File
# Begin Source File

SOURCE=.\Tree.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\IFace\i_compar.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_list.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_lru.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_lrustg.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_ltree.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\I_ROOT.H
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_stack.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_tree.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\lru.h
# End Source File
# Begin Source File

SOURCE=.\ltree.h
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

SOURCE=..\INCLUDE\Pr_obj.h
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

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stack.h
# End Source File
# Begin Source File

SOURCE=.\TREE.H
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\PrUtil.imp
# End Source File
# Begin Source File

SOURCE=.\PrUtil.meta
# End Source File
# Begin Source File

SOURCE=.\Util.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Util.dep
# End Source File
# Begin Source File

SOURCE=.\Util.mak
# End Source File
# End Target
# End Project
