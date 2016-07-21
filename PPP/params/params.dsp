# Microsoft Developer Studio Project File - Name="params" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=params - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "params.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "params.mak" CFG="params - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "params - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "params - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/params", VQIHAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "params - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/params"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O1 /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /I "../../updater60/Include" /I "../../CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /D "C_STYLE_PROP" /FD /c
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
# ADD LINK32 rpcrt4.lib /nologo /base:"0x66200000" /entry:"_DllMain" /dll /debug /machine:I386 /out:"../../out/Release/params.ppl" /libpath:"../../CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Sources6\out\Release\params.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=call prconvert "$(TargetPath)"	call tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "params - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/params"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /I "../../updater60/Include" /I "../../CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /D "C_STYLE_PROP" /FR /FD /GZ /c
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
# ADD LINK32 rpcrt4.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/params.ppl" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/debug"
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "params - Win32 Release"
# Name "params - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\plugin_params.cpp
# End Source File
# Begin Source File

SOURCE=.\startup.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\e_clsid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\e_ktime.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\e_ktrace.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\e_loader.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\error_mac.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_antispamfilter.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_avp3info.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_avs.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_avssession.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_avstreats.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_heap.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_iface.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_ifenum.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_io.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_ipresolver.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_licensing.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_mailinterceptorloader.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_mailtask.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_methodinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\iface\i_netsession.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\iface\i_posio.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_posio_sp.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_productlogic.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_qb.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_qbrestorer.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\IFace\i_receiv.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\i_reg.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_root.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_scheduler.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_string.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_task.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_taskex.h
# End Source File
# Begin Source File

SOURCE=..\include\iface\i_taskmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\iface\i_tracer.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\iface\i_updatecategory.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\iface\i_updateinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\iface\i_updateobjectinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\IFace\impexhlp.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\m_utils.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_ahfw.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_ahstm.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_antispamtask.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\plugin\p_avp3info.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_avs.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_bl.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_gui.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_idstask.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_licensing60.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_mctask.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\plugin\p_netsession.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_oas.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_ods.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_offguard.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_outlookplugintask.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_params.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_pdm.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_qb.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\plugin\p_resip.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_scheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_tm.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin\p_trafficmonitor.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\plugin\p_updatecategory.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\plugin\p_updateinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\plugin\p_updateobjectinfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_updater2005.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_compl.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_cpp.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_err.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_iid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_int.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_obj.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_oid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_pid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_prop.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_reg.h
# End Source File
# Begin Source File

SOURCE=..\..\Prague\Include\pr_remote.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_serdescriptor.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_serializable.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_stream.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_sys.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_types.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\pr_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_vid.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\include\pr_vtbl.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\INCLUDE\PRAGUE.H
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ah.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ahfw.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ahstm.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_antispam.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_as.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\structs\s_avp3info.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_avs.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_bb.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_bl.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\structs\s_fileinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_gui.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ids.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\structs\s_ipresolver.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_licensing.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc_oe.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc_trafficmonitor.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mcou_antispam.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_offguard.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_pdm.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\structs\s_propinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_qb.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_scaner.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_scheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_settings.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_taskmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\structs\s_updatecategory.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\structs\s_updateinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Updater60\Include\structs\s_updateobjectinfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_updater2005.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_itoa.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_kl.h
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

SOURCE=.\params.imp
# End Source File
# Begin Source File

SOURCE=.\params.meta
# End Source File
# Begin Source File

SOURCE=.\params.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\params.dep
# End Source File
# Begin Source File

SOURCE=.\params.mak
# End Source File
# End Target
# End Project
