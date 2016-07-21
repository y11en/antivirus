# Microsoft Developer Studio Project File - Name="httpprotocoller" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=httpprotocoller - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "httpprotocoller.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "httpprotocoller.mak" CFG="httpprotocoller - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "httpprotocoller - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "httpprotocoller - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/TrafficMonitor2/ProtocollersEx/HTTPProtocoller", MEOJBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "httpprotocoller - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../out/Release"
# PROP Intermediate_Dir "../../../../../../temp/Release/PPP/HTTPProtocoller"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /O1 /Ob2 /I ".." /I "../../.." /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../" /I "../../../../../../ppp/CSShare" /I "../../../../../../prague" /D "NDEBUG" /D "_DUMP_PACKET" /D "_PRAGUE_TRACE_" /D "_PRAGUE_TRACE_FRAME_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BOOST_SPIRIT_THREADSAFE" /D "BOOST_THREAD_CONFIG_WEK01032003_HPP" /D BOOST_THREAD_DECL="" /FD /Zm1200 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /base:"0x64100000" /subsystem:windows /dll /debug /machine:I386 /out:"../../../../../../out/Release/httpanlz.ppl" /opt:ref /release /opt:nowin98 /ignore:4078
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\httpanlz.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Post Build step ...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "httpprotocoller - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../out/Debug"
# PROP Intermediate_Dir "../../../../../../temp/Debug/PPP/HTTPProtocoller"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "../../.." /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../../../../../ppp/CSShare" /I "../../../../../../prague" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BOOST_SPIRIT_THREADSAFE" /D "BOOST_THREAD_CONFIG_WEK01032003_HPP" /D BOOST_THREAD_DECL="" /FD /GZ /Zm1200 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"../../../../../../out/Debug/httpanlz.ppl" /pdbtype:sept
# Begin Special Build Tool
TargetPath=\out\Debug\httpanlz.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Post Build step ...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "httpprotocoller - Win32 Release"
# Name "httpprotocoller - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\http\analyzerimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\cached_strategy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\CSShare\libs\thread\src\exceptions.cpp
# End Source File
# Begin Source File

SOURCE=.\generic_strategy.cpp
# End Source File
# Begin Source File

SOURCE=.\http_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\httpprotocoller.rc
# End Source File
# Begin Source File

SOURCE=.\httpprtocoller.imp
# End Source File
# Begin Source File

SOURCE=.\httpprtocoller.meta
# End Source File
# Begin Source File

SOURCE=..\http\message.cpp
# End Source File
# Begin Source File

SOURCE=.\message_builder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\CSShare\libs\thread\src\mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\CSShare\libs\thread\src\once.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_httpprotocoller.cpp
# End Source File
# Begin Source File

SOURCE=.\processor.cpp
# End Source File
# Begin Source File

SOURCE=..\http\recognizer.cpp
# End Source File
# Begin Source File

SOURCE=.\service_proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\session.cpp
# End Source File
# Begin Source File

SOURCE=..\http\skip_app.cpp
# End Source File
# Begin Source File

SOURCE=.\stream_strategy.cpp
# End Source File
# Begin Source File

SOURCE=..\http\takeurl.cpp
# End Source File
# Begin Source File

SOURCE=.\taskex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\CSShare\libs\thread\src\tss.cpp
# End Source File
# Begin Source File

SOURCE=..\http\verifyssl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\CSShare\libs\thread\src\xtime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\http\action.h
# End Source File
# Begin Source File

SOURCE=..\http\analyzerimpl.h
# End Source File
# Begin Source File

SOURCE=.\avp3_engine_proxy.h
# End Source File
# Begin Source File

SOURCE=..\http\cache.hpp
# End Source File
# Begin Source File

SOURCE=.\cached_strategy.h
# End Source File
# Begin Source File

SOURCE=..\http\dump.hpp
# End Source File
# Begin Source File

SOURCE=..\http\exceptions.hpp
# End Source File
# Begin Source File

SOURCE=.\generic_strategy.h
# End Source File
# Begin Source File

SOURCE=.\http_utils.h
# End Source File
# Begin Source File

SOURCE=..\http\message.h
# End Source File
# Begin Source File

SOURCE=..\http\message.hpp
# End Source File
# Begin Source File

SOURCE=.\message_builder.h
# End Source File
# Begin Source File

SOURCE=..\http\passthrough.h
# End Source File
# Begin Source File

SOURCE=.\preprocessing.h
# End Source File
# Begin Source File

SOURCE=.\proc_strategy.h
# End Source File
# Begin Source File

SOURCE=.\processor.h
# End Source File
# Begin Source File

SOURCE=.\service_observer.h
# End Source File
# Begin Source File

SOURCE=.\service_proxy.h
# End Source File
# Begin Source File

SOURCE=.\service_proxy.hpp
# End Source File
# Begin Source File

SOURCE=.\session.h
# End Source File
# Begin Source File

SOURCE=..\http\skip_app.h
# End Source File
# Begin Source File

SOURCE=.\stream_engine_proxy.h
# End Source File
# Begin Source File

SOURCE=..\http\stream_error.h
# End Source File
# Begin Source File

SOURCE=.\stream_strategy.h
# End Source File
# Begin Source File

SOURCE=..\http\support_persistent.h
# End Source File
# Begin Source File

SOURCE=..\http\takeurl.h
# End Source File
# Begin Source File

SOURCE=..\http\verifyssl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\blood_gif.bin
# End Source File
# Begin Source File

SOURCE=.\res\bloodstain.gif
# End Source File
# Begin Source File

SOURCE=.\easter_e.bin
# End Source File
# Begin Source File

SOURCE=.\res\easter_html.bin
# End Source File
# Begin Source File

SOURCE=.\res\infected_wav.bin
# End Source File
# Begin Source File

SOURCE=.\res\kl_gif.bin
# End Source File
# Begin Source File

SOURCE=.\res\vir_gif.bin
# End Source File
# Begin Source File

SOURCE=.\res\vir_red_gif.bin
# End Source File
# End Group
# Begin Group "ThreadImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ThreadImp.cpp
# End Source File
# End Group
# End Target
# End Project
