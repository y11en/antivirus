# Microsoft Developer Studio Project File - Name="CodeGen_C" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CodeGen_C - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CodeGen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CodeGen.mak" CFG="CodeGen_C - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CodeGen_C - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CodeGen_C - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen_C", IHYAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CodeGen_C - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/C"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/C"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\CommonFiles" /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 property.lib kldtser.lib swm.lib dtutils.lib DTDropper.lib Win32utils.lib winavpio.lib IFaceInfo.lib hashutil.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Debug/CodeGen_C.dll" /pdbtype:sept /libpath:"../../../out/debug" /libpath:"../../../CommonFiles/debugdll"
# Begin Custom Build - Invoking Version Control
IntDir=.\../../../temp/Debug/prague/CodeGen/C
ProjDir=.
TargetPath=\Prague\Debug\CodeGen_C.dll
InputPath=\Prague\Debug\CodeGen_C.dll
SOURCE="$(InputPath)"

"$(IntDir)\verctrl.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	VERSAVE $(TargetPath) $(ProjDir) 
	echo This is Version Control helper file. It is save to delete it. >$(IntDir)\verctrl.tmp 
	
# End Custom Build
# Begin Special Build Tool
TargetPath=\Prague\Debug\CodeGen_C.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=regsvr32 /s "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CodeGen_C - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/C"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/C"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GR /GX /Zi /O2 /I "..\..\..\CommonFiles" /I "..\..\include" /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "NO_ATL_MIN_CRT" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 property.lib kldtser.lib swm.lib dtutils.lib IFaceInfo.lib hashutil.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../Release/CodeGen_C.dll" /libpath:"../../../out/release"
# Begin Custom Build - Invoking Version Control
IntDir=.\../../../temp/Release/prague/CodeGen/C
ProjDir=.
TargetPath=\Prague\Release\CodeGen_C.dll
InputPath=\Prague\Release\CodeGen_C.dll
SOURCE="$(InputPath)"

"$(IntDir)\verctrl.tmp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	VERSAVE $(TargetPath) $(ProjDir) 
	echo This is Version Control helper file. It is save to delete it. >$(IntDir)\verctrl.tmp 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "CodeGen_C - Win32 Debug"
# Name "CodeGen_C - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowseFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\c_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\CGTools.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGen.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGen.def
# End Source File
# Begin Source File

SOURCE=.\CodeGen.idl

!IF  "$(CFG)" == "CodeGen_C - Win32 Debug"

# ADD MTL /tlb "CodeGen.tlb" /h "..\PCGInterface.h" /iid "CodeGen_i.c" /Oicf

!ELSEIF  "$(CFG)" == "CodeGen_C - Win32 Release"

# ADD MTL /tlb ".\CodeGen.tlb" /h "..\PCGInterface.h" /iid "CodeGen_i.c" /Oicf

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CodeGen2.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGenApp.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGenCPP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\COMUtils.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\PGIServer\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\extract.cpp
# End Source File
# Begin Source File

SOURCE=.\GenFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\h_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\i_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\mergers.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\CommonFiles\_AVPIO.H
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\AVPComID.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\AVPPrpID.h
# End Source File
# Begin Source File

SOURCE=..\AVPPveID.h
# End Source File
# Begin Source File

SOURCE=.\CodeGen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\COMUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\cpointer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\DTDropper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\DTUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_ifinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_impex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\iface\i_root.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\IFaceInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\KLDTDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\KLDTSer.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\OutputFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\Parray.h
# End Source File
# Begin Source File

SOURCE=..\PCGError.h
# End Source File
# Begin Source File

SOURCE=..\PCGInterface.h
# End Source File
# Begin Source File

SOURCE=..\PGIServerInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\PR_ERR.H
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\PR_IID.H
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_int.h
# End Source File
# Begin Source File

SOURCE=..\..\include\pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\PR_PID.H
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_types.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\PR_VID.H
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\Pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\..\INCLUDE\PRAGUE.H
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Property.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\Sarray.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\StdDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Swm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Version\ver_itoa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Version\ver_kl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\CodeDescription.dsc
# End Source File
# Begin Source File

SOURCE=.\CodeGen.rc
# End Source File
# Begin Source File

SOURCE=.\CodeGen.rgs
# End Source File
# Begin Source File

SOURCE=.\CodeGen2.rgs
# End Source File
# Begin Source File

SOURCE=.\CodeGenCPP.rgs
# End Source File
# Begin Source File

SOURCE=.\Options.klp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CodeGen.tlb
# End Source File
# Begin Source File

SOURCE=.\CodeGen_i.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\PGIServer\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
