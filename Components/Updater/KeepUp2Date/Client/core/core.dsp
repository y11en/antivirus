# Microsoft Developer Studio Project File - Name="core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=core - Win32 Debug STLPort
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core.mak" CFG="core - Win32 Debug STLPort"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core - Win32 Debug STLPort" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core - Win32 Release STLPort" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "core"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "core - Win32 Debug STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "core___Win32_Debug_STLPort"
# PROP BASE Intermediate_Dir "core___Win32_Debug_STLPort"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_STLPort"
# PROP Intermediate_Dir "Debug_STLPort"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../../CommonFiles" /I "../" /I "../../../../CS AdminKit/development2/include/stlport/stlport" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_USRDLL" /D "CORE_EXPORTS" /D "KAVUPDCORE_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /Fr /YX /FD /GZ /c
# SUBTRACT BASE CPP /Gy
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../../../external/" /I "../../../../../CommonFiles" /I "../../../../../CS AdminKit/development2/include/stlport/stlport" /I "../../../../../CS AdminKit/development2/include" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_USRDLL" /D "CORE_EXPORTS" /D "KAVUPDCORE_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "_USE_32BIT_TIME_T" /D "KLUPD_ENABLE_SIGNATURE_5" /D "DISABLE_BLACK_LIST_CHECK_BY_DATE_FOR_ADMINKIT" /Fr /FD /GZ /c
# SUBTRACT CPP /Gy /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "../../../../CommonFiles" /i "../" /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib advapi32.lib wininet.lib /nologo /dll /pdb:"../../../../out/Debug/upd_core.pdb" /debug /machine:I386 /out:"../../../../out//Debug/upd_core.dll" /implib:"../../../../out/Debug/upd_core.lib" /pdbtype:sept /libpath:"../../../../out/Debug" /libpath:"../../../../CS AdminKit/development2/lib/debug/"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib advapi32.lib wininet.lib iphlpapi.lib diffsAK.lib lzmalib.lib /nologo /dll /pdb:"../../../../../CS AdminKit/development2/bin/dlld/upd_core.pdb" /debug /machine:I386 /out:"../../../../../CS AdminKit/development2/bin/dlld/upd_core.dll" /implib:"../../../../../CS AdminKit/development2/lib/debug/upd_core.lib" /libpath:"../../../../../out/Debug/" /libpath:"../../../../../CS AdminKit/development2/lib/debug/"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Sources\Updater-1.4-BranchFrom-7-Release-KAV_KIS\CS AdminKit\development2\bin\dlld\upd_core.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "core - Win32 Release STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "core___Win32_Release_STLPort"
# PROP BASE Intermediate_Dir "core___Win32_Release_STLPort"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_STLPort"
# PROP Intermediate_Dir "Release_STLPort"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../../CommonFiles" /I "../" /I "../../../../CS AdminKit/development2/include/stlport/stlport" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_USRDLL" /D "CORE_EXPORTS" /D "KAVUPDCORE_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /Fr /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../../../external/" /I "../../../../../CommonFiles" /I "../../../../../CS AdminKit/development2/include/stlport/stlport" /I "../../../../../CS AdminKit/development2/include" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_USRDLL" /D "CORE_EXPORTS" /D "KAVUPDCORE_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_USE_32BIT_TIME_T" /D "KLUPD_ENABLE_SIGNATURE_5" /D "DISABLE_BLACK_LIST_CHECK_BY_DATE_FOR_ADMINKIT" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "../../../../CommonFiles" /i "../" /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib advapi32.lib wininet.lib /nologo /dll /pdb:"../../../../out/Release/upd_core.pdb" /debug /machine:I386 /nodefaultlib:"libcmt" /out:"../../../../out/Release/upd_core.dll" /implib:"../../../../out/Release/upd_core.lib" /pdbtype:sept /libpath:"../../../../out/Release" /libpath:"../../../../CS AdminKit/development2/lib/release/" /OPT:REF /verbose:lib /opt:ref /opt:icf
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib advapi32.lib wininet.lib iphlpapi.lib diffsAK.lib lzmalib.lib /nologo /dll /pdb:"../../../../../CS AdminKit/development2/bin/dll/upd_core.pdb" /debug /machine:I386 /out:"../../../../../CS AdminKit/development2/bin/dll/upd_core.dll" /implib:"../../../../../CS AdminKit/development2/lib/release/upd_core.lib" /libpath:"../../../../../out/Release/" /libpath:"../../../../../CS AdminKit/development2/lib/release/"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Sources\Updater-1.4-BranchFrom-7-Release-KAV_KIS\CS AdminKit\development2\bin\dll\upd_core.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "core - Win32 Debug STLPort"
# Name "core - Win32 Release STLPort"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\helper\base64.cpp
# End Source File
# Begin Source File

SOURCE=..\include\callback_iface.cpp
# End Source File
# Begin Source File

SOURCE=..\include\cfg_updater.cpp
# End Source File
# Begin Source File

SOURCE=.\core.rc
# End Source File
# Begin Source File

SOURCE=..\include\DownloadProgress.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\fileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpAuthorizationDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpProtocolImplementation.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\indexFileXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\licensing.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\local_file.cpp
# End Source File
# Begin Source File

SOURCE=..\include\log_iface.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\map_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\NetFacAK.cpp
# End Source File
# Begin Source File

SOURCE=.\NetFacHTTP.cpp
# End Source File
# Begin Source File

SOURCE=..\net\netFTP.cpp
# End Source File
# Begin Source File

SOURCE=..\net\netSMB_common.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\PrimaryIndexXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\proxyDetector.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\regionDetection.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\sites_info.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\sitesFileXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\sourceDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\net\stdSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\string.cpp
# End Source File
# Begin Source File

SOURCE=..\include\stringParser.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\strptime.cpp
# End Source File
# Begin Source File

SOURCE=.\upd_description.cpp
# End Source File
# Begin Source File

SOURCE=.\upd_download.cpp
# End Source File
# Begin Source File

SOURCE=.\upd_supp.cpp
# End Source File
# Begin Source File

SOURCE=.\update_info.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\update_transaction.cpp
# End Source File
# Begin Source File

SOURCE=.\updater.cpp
# End Source File
# Begin Source File

SOURCE=.\updater_diffs.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\updater_ntlm.cpp
# End Source File
# Begin Source File

SOURCE=.\updater_rollback.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\UpdaterLAXImplementation.cpp
# End Source File
# Begin Source File

SOURCE=..\helper\updaterStaff.cpp
# End Source File
# Begin Source File

SOURCE=..\include\updateSchedule.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\helper\base64.h
# End Source File
# Begin Source File

SOURCE=..\include\callback_iface.h
# End Source File
# Begin Source File

SOURCE=..\include\cfg_updater.h
# End Source File
# Begin Source File

SOURCE=..\include\cfg_updater2.h
# End Source File
# Begin Source File

SOURCE=..\helper\comdefs.h
# End Source File
# Begin Source File

SOURCE=..\include\DownloadProgress.h
# End Source File
# Begin Source File

SOURCE=..\helper\fileInfo.h
# End Source File
# Begin Source File

SOURCE=.\HttpAuthorizationDriver.h
# End Source File
# Begin Source File

SOURCE=.\HttpProtocolImplementation.h
# End Source File
# Begin Source File

SOURCE=..\helper\indexFileXMLParser.h
# End Source File
# Begin Source File

SOURCE=..\include\journal_iface.h
# End Source File
# Begin Source File

SOURCE=..\helper\kavset_xml.h
# End Source File
# Begin Source File

SOURCE=..\include\kavupd_component.h
# End Source File
# Begin Source File

SOURCE=..\include\kavupd_strings.h
# End Source File
# Begin Source File

SOURCE=..\helper\licensing.h
# End Source File
# Begin Source File

SOURCE=..\helper\local_file.h
# End Source File
# Begin Source File

SOURCE=..\helper\localize.h
# End Source File
# Begin Source File

SOURCE=..\include\log_iface.h
# End Source File
# Begin Source File

SOURCE=..\helper\map_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\NetFacAK.h
# End Source File
# Begin Source File

SOURCE=.\NetFacHTTP.h
# End Source File
# Begin Source File

SOURCE=..\net\netTransfer.h
# End Source File
# Begin Source File

SOURCE=..\helper\PrimaryIndexXMLParser.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\helper\sites_info.h
# End Source File
# Begin Source File

SOURCE=..\helper\sitesFileXMLParser.h
# End Source File
# Begin Source File

SOURCE=..\helper\sourceDescription.h
# End Source File
# Begin Source File

SOURCE=..\helper\stdinc.h
# End Source File
# Begin Source File

SOURCE=..\net\stdSocket.h
# End Source File
# Begin Source File

SOURCE=..\include\stringParser.h
# End Source File
# Begin Source File

SOURCE=.\update_info.h
# End Source File
# Begin Source File

SOURCE=..\include\update_source.h
# End Source File
# Begin Source File

SOURCE=..\helper\update_transaction.h
# End Source File
# Begin Source File

SOURCE=.\updater.h
# End Source File
# Begin Source File

SOURCE=.\win32\updater_ntlm.h
# End Source File
# Begin Source File

SOURCE=..\helper\updater_settings_keywords.h
# End Source File
# Begin Source File

SOURCE=..\helper\updaterStaff.h
# End Source File
# Begin Source File

SOURCE=..\include\ver_mod.h
# End Source File
# End Group
# Begin Group "Diffs"

# PROP Default_Filter "cpp,h"
# Begin Source File

SOURCE=.\diffs\apdiftb.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\base.h
# End Source File
# Begin Source File

SOURCE=.\diffs\bindiff.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\calcsum.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\calcsum.h
# End Source File
# Begin Source File

SOURCE=.\diffs\crc32.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\crc32.h
# End Source File
# Begin Source File

SOURCE=.\diffs.h
# End Source File
# Begin Source File

SOURCE=.\diftypes.h
# End Source File
# Begin Source File

SOURCE=.\diffs\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\md5.h
# End Source File
# Begin Source File

SOURCE=.\diffs\records.h
# End Source File
# Begin Source File

SOURCE=.\diffs\sq_s.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\sq_su.h
# End Source File
# Begin Source File

SOURCE=.\diffs\sq_u.cpp
# End Source File
# Begin Source File

SOURCE=.\diffs\sqze.h
# End Source File
# End Group
# End Target
# End Project
