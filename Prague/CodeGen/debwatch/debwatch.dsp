# Microsoft Developer Studio Project File - Name="debwatch" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=debwatch - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "debwatch.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "debwatch.mak" CFG="debwatch - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "debwatch - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "debwatch - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen/debwatch", ZBICAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "debwatch - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/debwatch"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/debwatch"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../ppp/include" /I "../../include/iface" /I "../../include" /I "../../../updater60/Include" /I "../../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib /nologo /dll /machine:I386 /out:"../../Release/debwatch.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/debwatch"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/debwatch"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include/iface" /I "../../include" /I "../../../ppp/include" /I "../../updater60/Include" /I "../../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib /nologo /dll /debug /machine:I386 /out:"E:\ds\common\MSDev98\Bin\debwatch.dll " /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "debwatch - Win32 Release"
# Name "debwatch - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cp_format.c
# End Source File
# Begin Source File

SOURCE=.\debwatch.def
# End Source File
# Begin Source File

SOURCE=..\..\kernel\time\dt.c
# End Source File
# Begin Source File

SOURCE=.\dt_format.c
# End Source File
# Begin Source File

SOURCE=.\err_format.c
# End Source File
# Begin Source File

SOURCE=.\find.c
# End Source File
# Begin Source File

SOURCE=.\handle.c
# End Source File
# Begin Source File

SOURCE=.\heap.c
# End Source File
# Begin Source File

SOURCE=.\hOBJECT.c
# End Source File
# Begin Source File

SOURCE=.\hr_format.c
# End Source File
# Begin Source File

SOURCE=.\hString.c
# End Source File
# Begin Source File

SOURCE=.\iid_format.c
# End Source File
# Begin Source File

SOURCE=.\nfio.c
# End Source File
# Begin Source File

SOURCE=.\pid_format.c
# End Source File
# Begin Source File

SOURCE=.\plugin.c
# End Source File
# Begin Source File

SOURCE=.\prop_format.c
# End Source File
# Begin Source File

SOURCE=.\rec_format.c
# End Source File
# Begin Source File

SOURCE=.\serializableId.cpp
# End Source File
# Begin Source File

SOURCE=.\str_range_format.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\custview.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\e_clsid.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\e_ktrace.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_csect.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_event.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_impex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_io.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_root.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_semaph.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_seqio.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_string.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\IFace\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\..\Kernel\kernel.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_err.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_iid.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_int.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_pid.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_types.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_vid.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\prague.h
# End Source File
# Begin Source File

SOURCE=..\..\String\pstring.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=E:\ds\Common\MSDev98\Bin\AUTOEXP.DAT
# End Source File
# Begin Source File

SOURCE=..\..\Release\debwath.readme.txt

!IF  "$(CFG)" == "debwatch - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
