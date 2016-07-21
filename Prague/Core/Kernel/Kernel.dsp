# Microsoft Developer Studio Project File - Name="Kernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Kernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Kernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Kernel.mak" CFG="Kernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Kernel - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Kernel - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/Kernel", SFABAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Kernel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/Kernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/Kernel"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /D "ITS_KERNEL" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 m_utils.lib hashutil.lib /nologo /base:"0x64a00000" /entry:"DllMain" /dll /debug /machine:I386 /out:"../../out/Release/PrKernel.ppl" /libpath:"../../out/release" /IGNORE:4086 /opt:ref
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetPath=\out\Release\PrKernel.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/Kernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/Kernel"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\include" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_PRAGUE_TRACE_" /D "ITS_KERNEL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 m_utils.lib hashutil.lib /nologo /entry:"DllMain" /dll /map:"../../out/Debug/PrKernel.map" /debug /machine:I386 /out:"../../out/Debug/PrKernel.ppl" /pdbtype:sept /libpath:"../../out/debug" /ignore:4086 /IGNORE:6004
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Kernel - Win32 Release"
# Name "Kernel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\custprop.c
# End Source File
# Begin Source File

SOURCE=.\time\dt.c
# End Source File
# Begin Source File

SOURCE=.\handle.c
# End Source File
# Begin Source File

SOURCE=.\iface.c
# End Source File
# Begin Source File

SOURCE=.\impex.c
# End Source File
# Begin Source File

SOURCE=.\k_ifenum.c
# End Source File
# Begin Source File

SOURCE=.\k_root.c
# End Source File
# Begin Source File

SOURCE=.\k_trace.c
# End Source File
# Begin Source File

SOURCE=.\kernel.c

!IF  "$(CFG)" == "Kernel - Win32 Release"

!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

# ADD CPP /nologo

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memory.c
# End Source File
# Begin Source File

SOURCE=.\mh_array.c
# End Source File
# Begin Source File

SOURCE=.\not_sink.c
# End Source File
# Begin Source File

SOURCE=.\output.c
# End Source File
# Begin Source File

SOURCE=.\prop.c
# End Source File
# Begin Source File

SOURCE=.\prop_str.c
# End Source File
# Begin Source File

SOURCE=.\receiver.c
# End Source File
# Begin Source File

SOURCE=.\ser_reg.cpp
# End Source File
# Begin Source File

SOURCE=.\ser_stream.cpp
# End Source File
# Begin Source File

SOURCE=.\serializer.cpp
# End Source File
# Begin Source File

SOURCE=.\synchro.c
# End Source File
# Begin Source File

SOURCE=.\sys_hand.c
# End Source File
# Begin Source File

SOURCE=.\sys_mem.c
# End Source File
# Begin Source File

SOURCE=.\sys_msg.c
# End Source File
# Begin Source File

SOURCE=.\sys_obj.c
# End Source File
# Begin Source File

SOURCE=.\sys_prop.c
# End Source File
# Begin Source File

SOURCE=.\sys_sync.c
# End Source File
# Begin Source File

SOURCE=.\sys_trc.c
# End Source File
# Begin Source File

SOURCE=.\system.c
# End Source File
# Begin Source File

SOURCE=.\wrappers.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\AVPPort.h
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

SOURCE=..\Include\IFace\e_string.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\hashutil.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\hashutil\hashutil.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_csect.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_event.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_impex.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_io.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_loader.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_mutex.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\i_reg.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_root.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_semaph.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_seqio.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_serializer.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_string.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_tracer.h
# End Source File
# Begin Source File

SOURCE=.\impex.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\impexhlp.h
# End Source File
# Begin Source File

SOURCE=.\k_ifenum.h
# End Source File
# Begin Source File

SOURCE=.\k_root.h
# End Source File
# Begin Source File

SOURCE=.\k_trace.h
# End Source File
# Begin Source File

SOURCE=.\kernel.h
# End Source File
# Begin Source File

SOURCE=..\Include\IFace\m_kernel.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\m_utils.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_string.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_cpp.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_err.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_iid.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_oid.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_pg.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_pid.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_pm.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_pmc.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_d_vid.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_err.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_iid.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_int.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_loadr.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_pid.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_serdescriptor.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_serializable.h
# End Source File
# Begin Source File

SOURCE=..\Include\pr_stream.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_types.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_vid.h
# End Source File
# Begin Source File

SOURCE=..\include\pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\include\prague.h
# End Source File
# Begin Source File

SOURCE=.\receiver.h
# End Source File
# Begin Source File

SOURCE=.\ser.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_product.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\kernel.imp
# End Source File
# Begin Source File

SOURCE=.\kernel.meta
# End Source File
# Begin Source File

SOURCE=.\kernel.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Kernel.dep
# End Source File
# Begin Source File

SOURCE=.\Kernel.mak
# End Source File
# Begin Source File

SOURCE=..\Release\PrConvert.cmd
# End Source File
# End Target
# End Project
