# Microsoft Developer Studio Project File - Name="TrafficMonitor2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TrafficMonitor2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TrafficMonitor2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TrafficMonitor2.mak" CFG="TrafficMonitor2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TrafficMonitor2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TrafficMonitor2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TrafficMonitor2 - Win32 Release_SSL_PDB" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/TrafficMonitor2", BBQFBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TrafficMonitor2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../out/Release"
# PROP Intermediate_Dir "../../../../temp/Release/ppp/TrafficMonitor2"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "..\TrafficMonitor2" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_CKAHUM" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 advapi32.lib kernel32.lib user32.lib CKAHUM.lib ws2_32.lib fsdrvlib.lib fssync.lib ole32.lib ssleay32.lib libeay32.lib /nologo /base:"0x68300000" /subsystem:windows /dll /debug /machine:I386 /out:"../../../../out/Release/TrafficMonitor2.ppl" /libpath:"../../../../out/release" /libpath:"../../../../CommonFiles\ReleaseDll" /libpath:"dll\release" /opt:ref /IGNORE:4089
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
ProjDir=.
TargetDir=\Release\Kaspersky Anti-Virus 6.0\out\Release
TargetPath=\Release\Kaspersky Anti-Virus 6.0\out\Release\TrafficMonitor2.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "$(ProjDir)\dll\release" "$(TargetDir)" /Y /R /S /I	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TrafficMonitor2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../out/Debug"
# PROP Intermediate_Dir "../../../../temp/Debug/ppp/TrafficMonitor2"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "..\TrafficMonitor2" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_CKAHUM" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib user32.lib kernel32.lib Rpcrt4.lib CKAHUM.lib ole32.lib ws2_32.lib fsdrvlib.lib fssync.lib ssleay32.lib libeay32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../../out/Debug/TrafficMonitor2.ppl" /pdbtype:sept /libpath:"..\..\..\..\out\debug" /libpath:"..\..\..\..\commonfiles\debugdll" /libpath:"dll\debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
ProjDir=.
TargetDir=\Release\Kaspersky Anti-Virus 6.0\out\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "$(ProjDir)\dll\debug" "$(TargetDir)" /Y /R /S /I
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TrafficMonitor2 - Win32 Release_SSL_PDB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TrafficMonitor2___Win32_Release_SSL_PDB"
# PROP BASE Intermediate_Dir "TrafficMonitor2___Win32_Release_SSL_PDB"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TrafficMonitor2___Win32_Release_SSL_PDB"
# PROP Intermediate_Dir "TrafficMonitor2___Win32_Release_SSL_PDB"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "..\TrafficMonitor2" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_CKAHUM" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /MDd /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "..\TrafficMonitor2" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_CKAHUM" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /i "../../../../CommonFiles" /d "NDEBUG"
# ADD RSC /l 0x417 /i "../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib user32.lib CKAHUM.lib ws2_32.lib fsdrvlib.lib fssync.lib ole32.lib ssleay32.lib libeay32.lib /nologo /base:"0x68300000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../../../../out/Release/TrafficMonitor2.ppl" /libpath:"../../../../out/release" /libpath:"../../../../CommonFiles\ReleaseDll" /libpath:"dll\release" /opt:ref /IGNORE:4089
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 advapi32.lib kernel32.lib user32.lib CKAHUM.lib ws2_32.lib fsdrvlib.lib fssync.lib ole32.lib ssleay32.lib libeay32.lib /nologo /base:"0x68300000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../../../../out/Release/TrafficMonitor2.ppl" /libpath:"../../../../out/release" /libpath:"../../../../CommonFiles\ReleaseDll" /libpath:"dll\debug" /opt:ref /IGNORE:4089
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
ProjDir=.
TargetDir=\Release\Kaspersky Anti-Virus 6.0\out\Release
TargetPath=\Release\Kaspersky Anti-Virus 6.0\out\Release\TrafficMonitor2.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(ProjDir)\dll\release $(TargetDir) /Y /R /S /I	tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "TrafficMonitor2 - Win32 Release"
# Name "TrafficMonitor2 - Win32 Debug"
# Name "TrafficMonitor2 - Win32 Release_SSL_PDB"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BufferWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\mailtask.cpp
# End Source File
# Begin Source File

SOURCE=.\mcguardops.cpp
# End Source File
# Begin Source File

SOURCE=.\mchandleops.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_trafficmonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxySession.cpp
# End Source File
# Begin Source File

SOURCE=.\ProxySessionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadWriteSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleBufferWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ssl_util.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadImp.cpp
# End Source File
# Begin Source File

SOURCE=.\TrafficMonitor2.rc
# End Source File
# Begin Source File

SOURCE=.\TransparentSocket.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BufferWindow.h
# End Source File
# Begin Source File

SOURCE=.\critical_section.h
# End Source File
# Begin Source File

SOURCE=.\inline.h
# End Source File
# Begin Source File

SOURCE=.\mcguardops.h
# End Source File
# Begin Source File

SOURCE=.\mchandleops.h
# End Source File
# Begin Source File

SOURCE=.\mcretcodes.h
# End Source File
# Begin Source File

SOURCE=.\ProxySession.h
# End Source File
# Begin Source File

SOURCE=.\ProxySessionManager.h
# End Source File
# Begin Source File

SOURCE=.\ReadWriteSocket.h
# End Source File
# Begin Source File

SOURCE=.\SimpleBufferWindow.h
# End Source File
# Begin Source File

SOURCE=.\ssl_util.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ThreadImp.h
# End Source File
# Begin Source File

SOURCE=.\TrafficProtocoller.h
# End Source File
# Begin Source File

SOURCE=.\TransparentSocket.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "wincript"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wincript\AddCertificate.cpp
# End Source File
# Begin Source File

SOURCE=.\wincript\CheckCertificate.cpp
# End Source File
# Begin Source File

SOURCE=.\wincript\CheckCertificateChain.cpp
# End Source File
# Begin Source File

SOURCE=.\wincript\MyHandleError.cpp
# End Source File
# Begin Source File

SOURCE=.\wincript\RemoveCertificate.cpp
# End Source File
# Begin Source File

SOURCE=.\wincript\WinVerifyTrust.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\TrafficMonitor2.dsc
# End Source File
# Begin Source File

SOURCE=.\TrafficMonitor2.imp
# End Source File
# Begin Source File

SOURCE=.\TrafficMonitor2.meta
# End Source File
# End Target
# End Project
