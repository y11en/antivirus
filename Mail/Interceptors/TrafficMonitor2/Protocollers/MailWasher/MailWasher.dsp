# Microsoft Developer Studio Project File - Name="MailWasher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MailWasher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MailWasher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MailWasher.mak" CFG="MailWasher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MailWasher - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MailWasher - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/TrafficMonitor2/Protocollers/MailWasher", BPOHBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MailWasher - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../out/Release/"
# PROP Intermediate_Dir "../../../../../../temp/Release/PPP/MailWasher"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAILWASHER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAILWASHER_EXPORTS" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x65000000" /dll /debug /machine:I386 /out:"../../../../../../out/Release/MailDisp.ppl" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\MailDisp.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MailWasher - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../out/Debug"
# PROP Intermediate_Dir "../../../../../../temp/Debug/PPP/MailWasher"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAILWASHER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAILWASHER_EXPORTS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../../../out/Debug/MailDisp.ppl" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MailWasher - Win32 Release"
# Name "MailWasher - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DispatchProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\MailWasher.cpp
# End Source File
# Begin Source File

SOURCE=.\MailWasher.rc
# End Source File
# Begin Source File

SOURCE=.\MailWasherSession.cpp
# End Source File
# Begin Source File

SOURCE=.\MW_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_mailwasher.cpp
# End Source File
# Begin Source File

SOURCE=.\taskex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lookahead.h
# End Source File
# Begin Source File

SOURCE=.\MailWasher.h
# End Source File
# Begin Source File

SOURCE=.\MW_utils.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "ThreadImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ThreadImp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThreadImp.h
# End Source File
# End Group
# Begin Group "NOOP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\POP3Protocoller\NOOPThread.cpp
# End Source File
# Begin Source File

SOURCE=..\POP3Protocoller\NOOPThread.h
# End Source File
# End Group
# Begin Group "BufferWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\SimpleBufferWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SimpleBufferWindow.h
# End Source File
# End Group
# Begin Group "EncodingTables"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Filters\Antispam\EncodingTables\CharsetDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Filters\Antispam\EncodingTables\CharsetDecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Filters\Antispam\EncodingTables\EncodingTables.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Filters\Antispam\EncodingTables\EncodingTables.h
# End Source File
# End Group
# Begin Group "send_recv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\send_recv.cpp
# End Source File
# Begin Source File

SOURCE=.\send_recv.h
# End Source File
# End Group
# Begin Group "localtime"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\localtime.cpp
# End Source File
# Begin Source File

SOURCE=.\localtime.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MailWasher.cgo
# End Source File
# Begin Source File

SOURCE=.\MailWasher.dsc
# End Source File
# Begin Source File

SOURCE=.\MailWasher.imp
# End Source File
# Begin Source File

SOURCE=.\MailWasher.meta
# End Source File
# End Target
# End Project
