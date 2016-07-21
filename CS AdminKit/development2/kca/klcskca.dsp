# Microsoft Developer Studio Project File - Name="klcskca" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klcskca - Win32 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klcskca.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klcskca.mak" CFG="klcskca - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klcskca - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcskca - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcskca - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcskca - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CS AdminKit/development2/kca", SUKFBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klcskca - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\dll"
# PROP Intermediate_Dir "..\bin\release\kca"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "WIN32" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "NDEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x67a90000" /dll /map /debug /machine:I386 /implib:"../lib/release/klcskca.lib" /libpath:"../../../CommonFiles/ReleaseDll" /libpath:"../lib/release" /OPT:REF /OPT:ICF /OPT:NOWIN98 /DELAYLOAD:ADVAPI32.DLL /DELAYLOAD:USER32.DLL
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcskca - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\dlld"
# PROP Intermediate_Dir "..\bin\debug\kca"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../openssl/inc32" /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "_DEBUG" /D "WIN32" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "_DEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /implib:"..\lib\debug/klcskca.lib" /libpath:"../../../CommonFiles/DebugDll" /libpath:"../lib/debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcskca - Win32 Debug MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "klcskca___Win32_Debug_MBCS"
# PROP BASE Intermediate_Dir "klcskca___Win32_Debug_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\nudlld"
# PROP Intermediate_Dir "..\bin\debug_mbcs\kca"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "_DEBUG" /D "WIN32" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../openssl/inc32" /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "_DEBUG" /D "WIN32" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "_DEBUG" /d "_DLL"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "_DEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"..\bin\dlld/klcskca.dll" /implib:"..\lib\debug/klcskca.lib" /libpath:"O:\CommonFiles\DebugDll\\" /libpath:"../lib/debug"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /implib:"..\lib\debug_mbcs/klcskca.lib" /libpath:"../../../CommonFiles/DebugDll" /libpath:"../lib/debug_mbcs"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcskca - Win32 Release MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "klcskca___Win32_Release_MBCS"
# PROP BASE Intermediate_Dir "klcskca___Win32_Release_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\nudll"
# PROP Intermediate_Dir "..\bin\release_mbcs\kca"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "WIN32" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSKCA_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "USE_BOOST_THREADS" /D "BOOST_ALL_NO_LIB" /D "WIN32" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "NDEBUG" /d "_DLL"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "NDEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x678B0000" /dll /map /debug /machine:I386 /out:"..\bin\dll/klcskca.dll" /implib:"../lib/release/klcskca.lib" /libpath:"O:\CommonFiles\ReleaseDll\\" /libpath:"../lib/release" /OPT:REF /OPT:ICF /OPT:NOWIN98 /DELAYLOAD:ADVAPI32.DLL /DELAYLOAD:USER32.DLL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x678B0000" /dll /map /debug /machine:I386 /implib:"../lib/release_mbcs/klcskca.lib" /libpath:"../../../CommonFiles/ReleaseDll" /libpath:"../lib/release_mbcs" /OPT:REF /OPT:ICF /DELAYLOAD:ADVAPI32.DLL /DELAYLOAD:USER32.DLL
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "klcskca - Win32 Release"
# Name "klcskca - Win32 Debug"
# Name "klcskca - Win32 Debug MBCS"
# Name "klcskca - Win32 Release MBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "prcp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prcp\componentproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\prcp\proxybase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\wintermprefix.cpp
# End Source File
# End Group
# Begin Group "prci"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prci\cmp_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\componentidimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\componentinstancebaseimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\componentinstancebaseimpl.h
# End Source File
# Begin Source File

SOURCE=.\prci\componentinstanceimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\componentinstanceimpl.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\errlocids.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\errors.h
# End Source File
# Begin Source File

SOURCE=.\prci\prci_auth.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\prci_inprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\sink.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\soapapi.cpp
# End Source File
# Begin Source File

SOURCE=.\prci\taskdata.cpp
# End Source File
# End Group
# Begin Group "pres"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pres\eventsstorage.cpp
# End Source File
# Begin Source File

SOURCE=.\pres\eventsstorageserver.cpp
# End Source File
# Begin Source File

SOURCE=.\pres\localeventsstorage.h
# End Source File
# Begin Source File

SOURCE=..\common\policy.cpp
# End Source File
# Begin Source File

SOURCE=.\pres\presdefs.h
# End Source File
# Begin Source File

SOURCE=.\pres\prespersistenteventlist.cpp
# End Source File
# Begin Source File

SOURCE=.\pres\prespersistenteventlist.h
# End Source File
# Begin Source File

SOURCE=.\pres\pressoapapi.h
# End Source File
# Begin Source File

SOURCE=.\pres\pressubscriptionlist.cpp
# End Source File
# Begin Source File

SOURCE=.\pres\pressubscriptionlist.h
# End Source File
# End Group
# Begin Group "prss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prss\cxreplace.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\distr_pol.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\nameconvertor.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\nameconvertor.h
# End Source File
# Begin Source File

SOURCE=.\prss\ops_pol.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\paths.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\profiles.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\settingsstorage.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ss_bulk_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ss_bulkmodeimp.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ss_bulkmodeimp.h
# End Source File
# Begin Source File

SOURCE=.\prss\ss_modifyss.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ss_props.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ss_smartwrite.h
# End Source File
# Begin Source File

SOURCE=.\prss\ss_sync.cpp
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\ss_sync.h
# End Source File
# Begin Source File

SOURCE=.\prss\store.cpp
# End Source File
# Begin Source File

SOURCE=.\prss\ts_smart_rw.cpp
# End Source File
# End Group
# Begin Group "prts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prts\prts_task_ser.cpp
# End Source File
# Begin Source File

SOURCE=.\prts\prtssoapapi.h
# End Source File
# Begin Source File

SOURCE=.\prts\taskinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\prts\tasksstorage.cpp
# End Source File
# Begin Source File

SOURCE=.\prts\TasksStorageLocalProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\prts\taskstorageserver.cpp
# End Source File
# Begin Source File

SOURCE=.\prts\tsk_list.cpp
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\tsk_list.h
# End Source File
# End Group
# Begin Group "jrnl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\jrnl\common.cpp
# End Source File
# Begin Source File

SOURCE=.\jrnl\common.h
# End Source File
# Begin Source File

SOURCE=.\jrnl\errors.h
# End Source File
# Begin Source File

SOURCE=.\jrnl\journal.h
# End Source File
# Begin Source File

SOURCE=.\jrnl\journalimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\jrnl\journalimpl.h
# End Source File
# Begin Source File

SOURCE=.\jrnl\journaltextimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\jrnl\journaltextimpl.h
# End Source File
# Begin Source File

SOURCE=.\jrnl\osdep.h
# End Source File
# End Group
# Begin Group "gsoap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gsoap\std\modulestub\ftbridgesoapstub.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\modulestub\prcisoapstub.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\modulestub\pressoapstub.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\modulestub\prtssoapstub.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\modulestub\ss_srvsoapstub.h
# End Source File
# End Group
# Begin Group "ss_srv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ss_srv\ss_srv.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_srv\sssrv_proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_srv\sssrv_serverimp.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_srv\sssrv_serverimp.h
# End Source File
# Begin Source File

SOURCE=.\ss_srv\sssrv_soapapi.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_srv\sssrv_soapapi.h
# End Source File
# End Group
# Begin Group "prts2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prts2\ts_server2.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\error2params.cpp
# End Source File
# Begin Source File

SOURCE=..\common\normaddress.cpp
# End Source File
# Begin Source File

SOURCE=..\include\common\normaddress.h
# End Source File
# Begin Source File

SOURCE=..\common\trace_username.cpp
# End Source File
# Begin Source File

SOURCE=..\include\common\trace_username.h
# End Source File
# End Group
# Begin Group "init"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\init\init_kca.cpp
# End Source File
# End Group
# Begin Group "contie"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\contie\connectiontiedobjectshelperimp.cpp
# End Source File
# Begin Source File

SOURCE=.\contie\connectiontiedobjectshelperimp.h
# End Source File
# End Group
# Begin Group "ft"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\nagent\naginst\cert_load.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\file.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\file.h
# End Source File
# Begin Source File

SOURCE=.\ft\filereceiverbridge.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\filereceiverbridge.h
# End Source File
# Begin Source File

SOURCE=.\ft\ftbridge\filereceiverbridgesoapapi.h
# End Source File
# Begin Source File

SOURCE=.\ft\filesender.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\fileslist.h
# End Source File
# Begin Source File

SOURCE=.\ft\filetransferimp.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\filetransferimp.h
# End Source File
# Begin Source File

SOURCE=.\ft\filetransfersoapapi.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\filetransfersoapapi.h
# End Source File
# Begin Source File

SOURCE=.\ft\fileuploader.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\fileuploader.h
# End Source File
# Begin Source File

SOURCE=.\ft\foldersync.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\multicastsender.cpp
# End Source File
# Begin Source File

SOURCE=.\ft\multicastsender.h
# End Source File
# End Group
# Begin Group "loc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\fmtmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\include\common\fmtmsg.h
# End Source File
# Begin Source File

SOURCE=..\common\locevents.cpp
# End Source File
# Begin Source File

SOURCE=..\include\common\locevents.h
# End Source File
# End Group
# Begin Group "aud"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aud\auditsource.cpp
# End Source File
# End Group
# Begin Group "srvhrch"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\srvhrch\srvhrch_common_impl.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "prci headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\prci\cmp_helpers.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\componentid.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\componentinstance.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\componentinstance2.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\prci_actions.h
# End Source File
# Begin Source File

SOURCE=.\prci\prci_actions.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\prci_auth.h
# End Source File
# Begin Source File

SOURCE=.\prci\prci_auth.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\prci_const.h
# End Source File
# Begin Source File

SOURCE=.\prci\prci_inprocess.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\prci_tasks.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\productsettings.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\productstatistics.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\productstatistics2.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\producttaskscontrol.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\producttaskscontrol2.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\runtimeinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prci\simple_params.h
# End Source File
# Begin Source File

SOURCE=.\prci\sink.h
# End Source File
# Begin Source File

SOURCE=.\prci\soapapi.h
# End Source File
# Begin Source File

SOURCE=.\prci\taskdata.h
# End Source File
# End Group
# Begin Group "prcp headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\prcp\agentproxy.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\componentproxy.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\errlocids.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\internal.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\prcp_const.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\proxy_attrib.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prcp\proxybase.h
# End Source File
# End Group
# Begin Group "pres headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\pres\eventsstorage.h
# End Source File
# End Group
# Begin Group "prts headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\prts\prts_task_ser.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\prtsdefs.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\prxsmacros.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\taskinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\tasksstorage.h
# End Source File
# Begin Source File

SOURCE=.\prts\TasksStorageLocalProxy.h
# End Source File
# Begin Source File

SOURCE=.\prts\TasksStorageProxyImpl.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prts\taskstorageserver.h
# End Source File
# End Group
# Begin Group "prss headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\prss\errlocids.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\helpersi.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\prssconst.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\settingsstorage.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\ss_bulkmode.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\prss\store.h
# End Source File
# End Group
# Begin Group "init headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\init\init_kca.h
# End Source File
# End Group
# Begin Group "contie headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\contie\connectiontiedobjectshelper.h
# End Source File
# End Group
# Begin Group "ft headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\ft\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\filedescription.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\filesender.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\filetransfer.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\filetransferbridgebase.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\filetransferconst.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\foldersync.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ft\updateagentinfo.h
# End Source File
# End Group
# Begin Group "ss_srv headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\ss_srv\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ss_srv\ss_srv.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ss_srv\sss_actions.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ss_srv\sssrv_proxy.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\ss_srv\sssrv_server.h
# End Source File
# Begin Source File

SOURCE=..\include\common\win32_unimpersonate.h
# End Source File
# End Group
# Begin Group "aud headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\aud\audit_consts.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\aud\auditsource.h
# End Source File
# End Group
# Begin Group "srvhrch headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\srvhrch\srvhrch_common.h
# End Source File
# End Group
# Begin Group "bl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\kca\bl\agentbusinesslogic.h
# End Source File
# Begin Source File

SOURCE=..\include\kca\bl\businesslogic.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\klcskca.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# End Target
# End Project
