# Microsoft Developer Studio Project File - Name="IFaceInfo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IFaceInfo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IFaceInfo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IFaceInfo.mak" CFG="IFaceInfo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IFaceInfo - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "IFaceInfo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen/IFaceInfo", RGHBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IFaceInfo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/IFaceInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/IFaceInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /I ".." /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "IFaceInfo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/IFaceInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/IFaceInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /I ".." /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "IFaceInfo - Win32 Release"
# Name "IFaceInfo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DataStructInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\IFaceInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MethodInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\msg.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PropInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdTypeInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\tree.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\CommonFiles\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\AVPComID.h
# End Source File
# Begin Source File

SOURCE=..\AVPPveID.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\cpointer.h
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

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
