# Microsoft Developer Studio Project File - Name="klcsstd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klcsstd - Win32 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "klcsstd.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "klcsstd.mak" CFG="klcsstd - Win32 Debug MBCS"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "klcsstd - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcsstd - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcsstd - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klcsstd - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CS AdminKit/development2/std", GEZTAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klcsstd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\dll"
# PROP Intermediate_Dir "..\bin\release\std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /I "../../../CommonFiles/stuff" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "WIN32" /D "KLUSEFSSYNC" /YX /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "NDEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 delayimp.lib zlib.lib SWM.lib Property.lib dskm.lib klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x67c00000" /dll /map /debug /machine:I386 /implib:"..\lib\release/klcsstd.lib" /libpath:"../lib/release" /libpath:"../../../out/release" /OPT:REF /OPT:ICF /OPT:NOWIN98 /DELAYLOAD:ADVAPI32.DLL
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\dlld"
# PROP Intermediate_Dir "..\bin\debug\std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /I "../../../CommonFiles/stuff" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "_DEBUG" /D "WIN32" /D "KLUSEFSSYNC" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "_DEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zlibd.lib SWM.lib Property.lib dskm.lib klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /implib:"../lib/debug/klcsstd.lib" /libpath:"../lib/debug" /libpath:"../../../out/debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Debug MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "klcsstd___Win32_Debug_MBCS"
# PROP BASE Intermediate_Dir "klcsstd___Win32_Debug_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\nudlld"
# PROP Intermediate_Dir "..\bin\debug_mbcs\std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "_DEBUG" /D "WIN32" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /I "../../../CommonFiles/stuff" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "_DEBUG" /D "WIN32" /D "KLUSEFSSYNC" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "_DEBUG" /d "_DLL"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "_DEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"..\bin\dlld/klcsstd.dll" /implib:"../lib/debug/klcsstd.lib" /libpath:"../lib/debug"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 zlibd.lib SWM.lib Property.lib dskm.lib klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /implib:"../lib/debug_mbcs/klcsstd.lib"  /libpath:"../lib/debug_mbcs" /libpath:"../../../out/debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Release MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "klcsstd___Win32_Release_MBCS"
# PROP BASE Intermediate_Dir "klcsstd___Win32_Release_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\nudll"
# PROP Intermediate_Dir "..\bin\release_mbcs\std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "WIN32" /YX /FD /c
# SUBTRACT BASE CPP /WX /Fr
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../include" /I "../gsoap" /I "../gsoap/std" /I "../../../CommonFiles" /I "../include/stlport/stlport" /I "../openssl/inc32" /I "../../../CommonFiles/stuff" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCSSTD_EXPORTS" /D "KLCSC_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "BOOST_ALL_NO_LIB" /D SOAP_FMAC5=__declspec(dllexport) /D SOAP_CMAC=__declspec(dllexport) /D SOAP_FMAC1=__declspec(dllexport) /D SOAP_FMAC3=__declspec(dllexport) /D "WIN32" /D "KLUSEFSSYNC" /YX /FD /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "NDEBUG" /d "_DLL"
# ADD RSC /l 0x417 /i "../../../CommonFiles" /d "NDEBUG" /d "_DLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /base:"0x67A90000" /dll /map /debug /machine:I386 /out:"..\bin\dll/klcsstd.dll" /implib:"..\lib\release/klcsstd.lib"  /libpath:"../lib/release" /OPT:REF /OPT:ICF /OPT:NOWIN98 /DELAYLOAD:ADVAPI32.DLL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 delayimp.lib zlib.lib SWM.lib Property.lib dskm.lib klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x67A90000" /dll /map /debug /machine:I386 /implib:"..\lib\release_mbcs/klcsstd.lib" /libpath:"../lib/release_mbcs" /libpath:"../../../out/release" /OPT:REF /OPT:ICF /DELAYLOAD:ADVAPI32.DLL
# SUBTRACT LINK32 /pdb:none

!ENDIF

# Begin Target

# Name "klcsstd - Win32 Release"
# Name "klcsstd - Win32 Debug"
# Name "klcsstd - Win32 Debug MBCS"
# Name "klcsstd - Win32 Release MBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\baseobj.cpp
# End Source File
# Begin Source File

SOURCE=.\base\klbase.cpp
# End Source File
# End Group
# Begin Group "conv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\conv\klconv.cpp
# End Source File
# End Group
# Begin Group "err"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\err\errlocmodule.cpp
# End Source File
# Begin Source File

SOURCE=.\err\error.cpp
# End Source File
# Begin Source File

SOURCE=.\err\error2string.cpp
# End Source File
# Begin Source File

SOURCE=.\err\errorimp.h
# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\io\del_path.cpp
# End Source File
# Begin Source File

SOURCE=.\io\klio.cpp
# End Source File
# End Group
# Begin Group "par"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\bin2hex.cpp
# End Source File
# Begin Source File

SOURCE=.\par\make_env.cpp
# End Source File
# Begin Source File

SOURCE=.\par\ops_par.cpp
# End Source File
# Begin Source File

SOURCE=.\par\paramsimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\par\paramsimpl.h
# End Source File
# Begin Source File

SOURCE=.\par\paramslogger.cpp
# End Source File
# Begin Source File

SOURCE=.\par\parbinser.cpp
# End Source File
# Begin Source File

SOURCE=.\par\parbinser.h
# End Source File
# Begin Source File

SOURCE=.\par\parserialize.cpp
# End Source File
# Begin Source File

SOURCE=.\par\parstrdata.cpp
# End Source File
# Begin Source File

SOURCE=..\include\std\par\parstrdata.h
# End Source File
# Begin Source File

SOURCE=.\par\parstreams.cpp
# End Source File
# Begin Source File

SOURCE=..\include\common\pooled_vals.h
# End Source File
# Begin Source File

SOURCE=.\par\s_params.cpp
# End Source File
# Begin Source File

SOURCE=.\par\ser.cpp
# End Source File
# Begin Source File

SOURCE=.\par\sertypes.h
# End Source File
# Begin Source File

SOURCE=.\par\valuesimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\par\valuesimpl.h
# End Source File
# End Group
# Begin Group "sch"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sch\commonsch.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\everydayscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\everydayscheduleimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\everymonthscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\everymonthscheduleimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\everyweekscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\everyweekscheduleimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\millisecondsscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\millisecondsscheduleImp.h
# End Source File
# Begin Source File

SOURCE=.\sch\private.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\private.h
# End Source File
# Begin Source File

SOURCE=.\sch\schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\schedulerimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\schedulerimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\secondsscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\secondsscheduleimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\taskclasseslist.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\taskclasseslist.h
# End Source File
# Begin Source File

SOURCE=.\sch\taskimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\taskimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\tasklist.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\tasklist.h
# End Source File
# Begin Source File

SOURCE=.\sch\tasksiteratorimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\tasksiteratorimp.h
# End Source File
# Begin Source File

SOURCE=.\sch\weeksdaysscheduleimp.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\weeksdaysscheduleimp.h
# End Source File
# End Group
# Begin Group "sign"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sign\chkfile.cpp
# End Source File
# Begin Source File

SOURCE=.\sign\chksgn.cpp
# End Source File
# Begin Source File

SOURCE=.\sign\prsecmdl.h
# End Source File
# Begin Source File

SOURCE=..\avp\sign\sigdet\sigdet.cpp
# End Source File
# Begin Source File

SOURCE=..\avp\sign\sigdet\sigdet.h
# End Source File
# Begin Source File

SOURCE=..\avp\sign\include\sign_struc.h
# End Source File
# End Group
# Begin Group "thr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\thr\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\thr\thread.cpp
# End Source File
# End Group
# Begin Group "thrstore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\thrstore\threadstore.cpp
# End Source File
# End Group
# Begin Group "tmstg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tmstg\timeoutstore.cpp
# End Source File
# End Group
# Begin Group "tp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tp\threadspool.cpp
# End Source File
# Begin Source File

SOURCE=.\tp\threadspoolmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\tp\threadspoolmanager.h
# End Source File
# Begin Source File

SOURCE=.\tp\workingthread.cpp
# End Source File
# Begin Source File

SOURCE=.\tp\workingthread.h
# End Source File
# End Group
# Begin Group "trc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\trc\trace.cpp
# End Source File
# End Group
# Begin Group "gsoap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gsoap\std\stub\gsoapstubs.cpp
# End Source File
# Begin Source File

SOURCE=..\gsoap\common\namespaces.cpp
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\soapC.cpp
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\soapClient.cpp
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\soapH.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\soapServer.cpp
# End Source File
# Begin Source File

SOURCE=..\gsoap\std\soapStub.h
# End Source File
# Begin Source File

SOURCE=..\gsoap\stdsoap2.cpp

!IF  "$(CFG)" == "klcsstd - Win32 Release"

# ADD CPP /W3 /GX- /Ob2 /D SOAP_BLKLEN=8192

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Debug"

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Debug MBCS"

!ELSEIF  "$(CFG)" == "klcsstd - Win32 Release MBCS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\gsoap\stubrouter.cpp
# End Source File
# End Group
# Begin Group "boost"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\boost\src\thread\src\_thread.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\condition.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\exceptions.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\once.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\recursive_mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\threadmon.cpp
# End Source File
# Begin Source File

SOURCE=..\include\boost\src\thread\src\xtime.cpp
# End Source File
# End Group
# Begin Group "stress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\stress\st_prefix.cpp
# End Source File
# End Group
# Begin Group "hstd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hstd\hostdomain.cpp
# End Source File
# End Group
# Begin Group "time"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\time\kltime.cpp
# End Source File
# End Group
# Begin Group "pwd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\klcspwd\conv.h
# End Source File
# Begin Source File

SOURCE=.\klcspwd\crypto.cpp
# End Source File
# Begin Source File

SOURCE=..\include\std\klcspwd\errorcodes.h
# End Source File
# Begin Source File

SOURCE=.\klcspwd\klcspwd.cpp
# End Source File
# Begin Source File

SOURCE=.\klcspwd\klcspwd_inst.cpp
# End Source File
# Begin Source File

SOURCE=.\klcspwd\strg_9x.cpp
# End Source File
# Begin Source File

SOURCE=.\klcspwd\strg_nt.cpp
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\memory\klmem.cpp
# End Source File
# End Group
# Begin Group "init"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\init\init_std.cpp
# End Source File
# End Group
# Begin Group "rpt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rpt\globalreporter.cpp
# End Source File
# End Group
# Begin Group "fmt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fmt\stdfmtstr.cpp
# End Source File
# Begin Source File

SOURCE=..\include\std\fmt\stdfmtstr.h
# End Source File
# End Group
# Begin Group "avp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\avp\text2bin.c
# End Source File
# End Group
# Begin Group "conf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\conf\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\std\conf\std_conf.h
# End Source File
# Begin Source File

SOURCE=.\conf\std_parse_conf.cpp
# End Source File
# End Group
# Begin Group "md5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\md5\md5util.cpp
# End Source File
# Begin Source File

SOURCE=..\include\std\md5\md5util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\klcsstd.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "base headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\base\klbase.h
# End Source File
# Begin Source File

SOURCE=..\include\std\base\kldefs.h
# End Source File
# Begin Source File

SOURCE=..\include\std\base\klstd.h
# End Source File
# Begin Source File

SOURCE=..\include\std\base\klstdutils.h
# End Source File
# End Group
# Begin Group "conv headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\conv\klconv.h
# End Source File
# End Group
# Begin Group "err headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\err\errintervals.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\errloc_intervals.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\errlocids.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\errlocmodule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\error.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\error_localized.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\error_localized2.h
# End Source File
# Begin Source File

SOURCE=..\include\std\err\klerrors.h
# End Source File
# End Group
# Begin Group "io headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\io\klio.h
# End Source File
# End Group
# Begin Group "par headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\par\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\helpers.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\helpersi.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\make_env.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\par_conv.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\par_gsoap.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\params.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\paramsi.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\paramslogger.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\parserialize.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\parstreams.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\valenum.h
# End Source File
# Begin Source File

SOURCE=..\include\std\par\value.h
# End Source File
# End Group
# Begin Group "sch headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\sch\common.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\errors.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\everydayschedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\everymonthschedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\everyweekschedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\millisecondsschedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\schedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\scheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\secondsschedule.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\task.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\taskparams.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\taskresults.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\tasksiterator.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sch\weeksdaysschedule.h
# End Source File
# End Group
# Begin Group "sign headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\sign\chksgn.h
# End Source File
# Begin Source File

SOURCE=..\include\std\sign\sign_opt.h
# End Source File
# End Group
# Begin Group "thr headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\thr\locks.h
# End Source File
# Begin Source File

SOURCE=..\include\std\thr\sync.h
# End Source File
# Begin Source File

SOURCE=..\include\std\thr\thr_callbacks.h
# End Source File
# Begin Source File

SOURCE=..\include\std\thr\thread.h
# End Source File
# End Group
# Begin Group "thrstore headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\thrstore\threadstore.h
# End Source File
# End Group
# Begin Group "tmstg headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\tmstg\timeoutstore.h
# End Source File
# End Group
# Begin Group "tp headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\tp\threadspool.h
# End Source File
# Begin Source File

SOURCE=..\include\std\tp\tpcmdqueue.h
# End Source File
# End Group
# Begin Group "trc headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\trc\trace.h
# End Source File
# Begin Source File

SOURCE=..\include\std\trc\tracewatch.h
# End Source File
# End Group
# Begin Group "gsoap headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\gsoap\stubrouter.h
# End Source File
# End Group
# Begin Group "stress header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\stress\st_prefix.h
# End Source File
# End Group
# Begin Group "hstd header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\hstd\hostdomain.h
# End Source File
# End Group
# Begin Group "time headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\time\kltime.h
# End Source File
# End Group
# Begin Group "pwd headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\klcspwd\crypto.h
# End Source File
# Begin Source File

SOURCE=..\include\std\klcspwd\klcspwd.h
# End Source File
# Begin Source File

SOURCE=..\include\std\klcspwd\klcspwd_inst.h
# End Source File
# Begin Source File

SOURCE=..\include\std\klcspwd\klcspwdr.h
# End Source File
# Begin Source File

SOURCE=..\include\std\klcspwd\prtstrg.h
# End Source File
# End Group
# Begin Group "memory headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\memory\klmem.h
# End Source File
# End Group
# Begin Group "win32 headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\win32\klos_win32_errors.h
# End Source File
# Begin Source File

SOURCE=..\include\std\win32\klos_win32v40.h
# End Source File
# Begin Source File

SOURCE=..\include\std\win32\klos_win32v50.h
# End Source File
# End Group
# Begin Group "init headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\std\init\init_std.h
# End Source File
# End Group
# Begin Group "avp headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\avp\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\avpport.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\byteorder.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\klids.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\parray.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\stddefs.h
# End Source File
# Begin Source File

SOURCE=..\include\avp\text2bin.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\include\common\measurer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\klcsstd.rc
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
