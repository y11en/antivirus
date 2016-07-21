# Microsoft Developer Studio Project File - Name="klavcorelib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=klavcorelib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klavcorelib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klavcorelib.mak" CFG="klavcorelib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klavcorelib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "klavcorelib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/KLAVA/klava/kernel/klavcorelib", MAIJAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klavcorelib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\klava\kernel\klavcorelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /I "..\include" /I "..\..\..\CommonFiles" /I "..\..\..\external\lzmalib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "klavcorelib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\klava\kernel\klavcorelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Zi /Od /I "..\include" /I "..\..\..\CommonFiles" /I "..\..\..\external\lzmalib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /GZ /c
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

# Name "klavcorelib - Win32 Release"
# Name "klavcorelib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\action_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\buffer_io.cpp
# End Source File
# Begin Source File

SOURCE=.\codeloader.cpp
# End Source File
# Begin Source File

SOURCE=.\compldr.cpp
# End Source File
# Begin Source File

SOURCE=.\context_data_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\dso_proc_resolver.cpp
# End Source File
# Begin Source File

SOURCE=.\file_loader.cpp
# End Source File
# Begin Source File

SOURCE=.\fnutils.cpp
# End Source File
# Begin Source File

SOURCE=.\formatting.cpp
# End Source File
# Begin Source File

SOURCE=.\io_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\katran_support.cpp
# End Source File
# Begin Source File

SOURCE=.\kdb_utils.c
# End Source File
# Begin Source File

SOURCE=.\kdu_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\kfb_dbloader.cpp
# End Source File
# Begin Source File

SOURCE=.\klav_component.cpp
# End Source File
# Begin Source File

SOURCE=.\klav_objstack.cpp
# End Source File
# Begin Source File

SOURCE=.\klav_propbag.cpp
# End Source File
# Begin Source File

SOURCE=.\klav_props.cpp
# End Source File
# Begin Source File

SOURCE=.\klav_string.cpp
# End Source File
# Begin Source File

SOURCE=.\klaveng_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\klavtree.cpp
# End Source File
# Begin Source File

SOURCE=.\lb_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\logical_block.cpp
# End Source File
# Begin Source File

SOURCE=.\mft_reader.cpp
# End Source File
# Begin Source File

SOURCE=.\mft_writer.cpp
# End Source File
# Begin Source File

SOURCE=.\obj_queue.cpp
# End Source File
# Begin Source File

SOURCE=.\object_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\objstack_component.cpp
# End Source File
# Begin Source File

SOURCE=.\perf_monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\pool_alloc.cpp
# End Source File
# Begin Source File

SOURCE=.\rgn_checker.cpp
# End Source File
# Begin Source File

SOURCE=.\rgn_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\rgn_scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\rgnmgr_component.cpp
# End Source File
# Begin Source File

SOURCE=.\sig_checker.cpp
# End Source File
# Begin Source File

SOURCE=.\sig_data.cpp
# End Source File
# Begin Source File

SOURCE=.\sig_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\std_dbloader.cpp
# End Source File
# Begin Source File

SOURCE=.\string_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\strm_vdmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\tree_searcher.cpp
# End Source File
# Begin Source File

SOURCE=.\verdict_info.cpp
# End Source File
# Begin Source File

SOURCE=.\verdict_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\virtual_io.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "klavstl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\klavstl\algorithm.h
# End Source File
# Begin Source File

SOURCE=..\include\klavstl\allocator.h
# End Source File
# Begin Source File

SOURCE=..\include\klavstl\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\klavstl\function.h
# End Source File
# Begin Source File

SOURCE=..\include\klavstl\string.h
# End Source File
# Begin Source File

SOURCE=..\include\klavstl\vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\include\binstream.h
# End Source File
# Begin Source File

SOURCE=..\include\crc32.h
# End Source File
# Begin Source File

SOURCE=..\include\formatting.h
# End Source File
# Begin Source File

SOURCE=..\include\kdb_ids.h
# End Source File
# Begin Source File

SOURCE=..\include\kdb_utils.h
# End Source File
# Begin Source File

SOURCE=..\include\kdu.h
# End Source File
# Begin Source File

SOURCE=..\include\kdu_utils.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_dyn_obj.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_holders.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_perf.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_platform.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_stdint.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_tchar.h
# End Source File
# Begin Source File

SOURCE=..\include\kl_types.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_components.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_dbg.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_if_decl.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_io.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_objstack.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_props.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_rgnmgr.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_scanner.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_sig.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_stream.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_string.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_utils.h
# End Source File
# Begin Source File

SOURCE=..\include\klavcore.h
# End Source File
# Begin Source File

SOURCE=..\include\klavctx.h
# End Source File
# Begin Source File

SOURCE=..\include\klavdb.h
# End Source File
# Begin Source File

SOURCE=..\include\klavdef.h
# End Source File
# Begin Source File

SOURCE=..\include\klaveng.h
# End Source File
# Begin Source File

SOURCE=..\include\klaverr.h
# End Source File
# Begin Source File

SOURCE=..\include\klavimpl.h
# End Source File
# Begin Source File

SOURCE=..\include\klavprocid.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys_os.h
# End Source File
# Begin Source File

SOURCE=..\include\klavtree.h
# End Source File
# Begin Source File

SOURCE=..\include\lbdata.h
# End Source File
# Begin Source File

SOURCE=..\include\lbelts.h
# End Source File
# Begin Source File

SOURCE=..\include\obj_code.h
# End Source File
# Begin Source File

SOURCE=..\include\obj_queue.h
# End Source File
# End Group
# End Target
# End Project
