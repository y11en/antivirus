# Microsoft Developer Studio Project File - Name="Loader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Loader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "loader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "loader.mak" CFG="Loader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Loader - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Loader - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/Loader", KCBBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Loader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/Loader"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/Loader"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I ".\\" /I ".\win32" /I "..\include" /I "..\..\windows\hook" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "LOADER_BUILD" /D "ITS_LOADER" /FD /c
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
# ADD LINK32 winavpio.lib swm.lib kldtser.lib win32utils.lib property.lib user32.lib kernel32.lib sign.lib ole32.lib ifaceinfo.lib Advapi32.lib klsys.lib metadata.lib dskm.lib /nologo /base:"0x64d00000" /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"../../out/Release/prloader.dll" /libpath:"..\..\out\release" /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref /IGNORE:4098
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\prloader.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/Loader"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/Loader"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\\" /I ".\win32" /I "..\include" /I "..\..\windows\hook" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "LOADER_BUILD" /D "ITS_LOADER" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 swm.lib kldtser.lib property.lib user32.lib kernel32.lib sign.lib ole32.lib ifaceinfo.lib Advapi32.lib klsys.lib metadata.lib dskm.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"../../out/Debug/prloader.dll" /libpath:"..\..\out\debug" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /verbose /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Loader - Win32 Release"
# Name "Loader - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\win32\csect_2l.c
# End Source File
# Begin Source File

SOURCE=.\win32\csection.c
# End Source File
# Begin Source File

SOURCE=.\win32\cthread.c
# End Source File
# Begin Source File

SOURCE=.\win32\dbgout.c
# End Source File
# Begin Source File

SOURCE=.\win32\enter.asm

!IF  "$(CFG)" == "Loader - Win32 Release"

# Begin Custom Build
OutDir=.\../../out/Release
InputPath=.\win32\enter.asm
InputName=enter

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

# Begin Custom Build
OutDir=.\../../out/Debug
InputPath=.\win32\enter.asm
InputName=enter

"$(OutDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Zi /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\env_strings.cpp
# End Source File
# Begin Source File

SOURCE=.\fm_folder.cpp
# End Source File
# Begin Source File

SOURCE=.\fm_str_plist.cpp
# End Source File
# Begin Source File

SOURCE=.\loader.cpp
# End Source File
# Begin Source File

SOURCE=.\memmgr.c
# End Source File
# Begin Source File

SOURCE=.\mod_load.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\mutex.c
# End Source File
# Begin Source File

SOURCE=..\string\oem_win.cpp
# End Source File
# Begin Source File

SOURCE=.\ploader.c
# End Source File
# Begin Source File

SOURCE=.\plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\pprot.c
# End Source File
# Begin Source File

SOURCE=..\CodeGen\PrDbgInfo\PrDbgInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\prop_util.c
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Service\SA.CPP
# End Source File
# Begin Source File

SOURCE=.\scheck.cpp
# End Source File
# Begin Source File

SOURCE=.\scheck_list.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\semaphore.c
# End Source File
# Begin Source File

SOURCE=.\win32\shareexclusiveresource.cpp
# End Source File
# Begin Source File

SOURCE=.\signchk.c
# End Source File
# Begin Source File

SOURCE=.\str_val.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\syncevent.c
# End Source File
# Begin Source File

SOURCE=.\win32\token.c
# End Source File
# Begin Source File

SOURCE=.\win32\trace.c
# End Source File
# Begin Source File

SOURCE=.\win32\wheap.c
# End Source File
# Begin Source File

SOURCE=.\win32\wheap_check.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_AVPIO.H
# End Source File
# Begin Source File

SOURCE=..\string\ansi.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\AVPComID.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\CodeGen\AVPPveID.h
# End Source File
# Begin Source File

SOURCE=..\string\codec.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\cpointer.h
# End Source File
# Begin Source File

SOURCE=.\csect_2l.h
# End Source File
# Begin Source File

SOURCE=.\csection.h
# End Source File
# Begin Source File

SOURCE=..\CodeGen\PrDbgInfo\dbghelp.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\e_clsid.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\e_ktime.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\e_ktrace.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\e_loader.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Hook\FSSync.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_csect.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_event.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_iface.h
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

SOURCE=..\Include\IFace\i_list.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_loader.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_mutex.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\I_ROOT.H
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_semaph.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_seqio.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\IFACE\i_string.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\CodeGen\IFaceInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\impexhlp.h
# End Source File
# Begin Source File

SOURCE=..\Kernel\k_root.h
# End Source File
# Begin Source File

SOURCE=.\loader.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\hook\MemManag.h
# End Source File
# Begin Source File

SOURCE=.\memmgr.h
# End Source File
# Begin Source File

SOURCE=.\mod_load.h
# End Source File
# Begin Source File

SOURCE=.\mutex.h
# End Source File
# Begin Source File

SOURCE=..\string\oem.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_win32loader.h
# End Source File
# Begin Source File

SOURCE=.\plugin.h
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

SOURCE=..\Include\pr_loadr.h
# End Source File
# Begin Source File

SOURCE=..\Include\Pr_msg.h
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

SOURCE=..\CodeGen\PrDbgInfo\PrDbgInfo.h
# End Source File
# Begin Source File

SOURCE=.\PrFormat.h
# End Source File
# Begin Source File

SOURCE=.\prop_util.h
# End Source File
# Begin Source File

SOURCE=..\string\pstring.h
# End Source File
# Begin Source File

SOURCE=..\string\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Service\SA.H
# End Source File
# Begin Source File

SOURCE=.\sem.h
# End Source File
# Begin Source File

SOURCE=.\win32\shareexclusiveresource.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Sign\SIGN.H
# End Source File
# Begin Source File

SOURCE=.\syncevent.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\string\ver_mod.h
# End Source File
# Begin Source File

SOURCE=.\win32\wheap.h
# End Source File
# Begin Source File

SOURCE=.\win32\WHeap_check.h
# End Source File
# Begin Source File

SOURCE=..\string\widechar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Loader.meta
# End Source File
# Begin Source File

SOURCE=.\Loader.rc
# End Source File
# End Group
# Begin Group "inc"

# PROP Default_Filter "inc"
# Begin Source File

SOURCE=.\win32\ex_data.inc
# End Source File
# Begin Source File

SOURCE=.\win32\fm_folder_inc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32\loader_inc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32\mod_load_inc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32\ploader_inc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32\plugin_inc.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32\signchk_inc.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "string"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\string\ansi_win.cpp
# End Source File
# Begin Source File

SOURCE=..\string\codec.cpp
# End Source File
# Begin Source File

SOURCE=..\string\compare.c
# End Source File
# Begin Source File

SOURCE=..\string\plugin_string.cpp
# End Source File
# Begin Source File

SOURCE=..\string\pstring.c
# End Source File
# Begin Source File

SOURCE=..\string\widechar_win.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Loader.dep
# End Source File
# Begin Source File

SOURCE=.\Loader.mak
# End Source File
# Begin Source File

SOURCE=..\Release\PrConvert.cmd
# End Source File
# Begin Source File

SOURCE=..\Release\PrConverter.exe
# End Source File
# End Target
# End Project
