# Microsoft Developer Studio Project File - Name="NNTPProtocoller" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NNTPProtocoller - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NNTPProtocoller.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NNTPProtocoller.mak" CFG="NNTPProtocoller - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NNTPProtocoller - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NNTPProtocoller - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/TrafficMonitor2/ProtocollersEx/NNTPProtocoller", FIOJBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NNTPProtocoller - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../out/Release"
# PROP Intermediate_Dir "../../../../../../temp/Release/PPP/NNTPProtocoller"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NNTPPROTOCOLLER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../" /I "../../../../../../ppp/CSShare" /I "../../../../../../prague" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NNTPPROTOCOLLER_EXPORTS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x65c00000" /dll /debug /machine:I386 /out:"../../../../../../out/Release/NNTPProtocoller.ppl" /opt:ref
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\Release\NNTPProtocoller.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NNTPProtocoller - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../out/Debug"
# PROP Intermediate_Dir "../../../../../../temp/Debug/PPP/NNTPProtocoller"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NNTPPROTOCOLLER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../../../CommonFiles" /I "../../../../../../prague/include" /I "../../../../../../ppp/include" /I "../../" /I "../../../../../../ppp/CSShare" /I "../../../../../../prague" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NNTPPROTOCOLLER_EXPORTS" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../../../out/Debug/NNTPProtocoller.ppl" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NNTPProtocoller - Win32 Release"
# Name "NNTPProtocoller - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\nntp\NntpProtocolAnalyzerImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\NNTPProtocoller.rc
# End Source File
# Begin Source File

SOURCE=.\plugin_nntpprotocoller.cpp
# End Source File
# Begin Source File

SOURCE=.\taskex.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\nntp\NntpProtocolAnalyzer.h
# End Source File
# Begin Source File

SOURCE=..\nntp\NntpProtocolAnalyzerImpl.h
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
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\BaseProtocolAnalyzerImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\common\BaseProtocolAnalyzerImpl.h
# End Source File
# Begin Source File

SOURCE=..\common\ContentItem.h
# End Source File
# Begin Source File

SOURCE=..\common\DataBuffer.h
# End Source File
# Begin Source File

SOURCE=..\common\IProtocolAnalyzerEx.h
# End Source File
# Begin Source File

SOURCE=..\common\NOOPThread.cpp
# End Source File
# Begin Source File

SOURCE=..\common\NOOPThread.h
# End Source File
# Begin Source File

SOURCE=..\common\ProtocolAnalyzerImplAdapter.h
# End Source File
# Begin Source File

SOURCE=..\common\ProtocolProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ProtocolProcessor.h
# End Source File
# Begin Source File

SOURCE=..\common\SplitTextBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\common\SplitTextBuffer.h
# End Source File
# Begin Source File

SOURCE=..\common\StringUtil.h
# End Source File
# End Group
# Begin Group "interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\interfaces\IProtocolAnalyzer.h
# End Source File
# Begin Source File

SOURCE=..\interfaces\TrafficProtocollerTypes.h
# End Source File
# End Group
# Begin Group "ThreadImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ThreadImp.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\NNTPProtocoller.dsc
# End Source File
# Begin Source File

SOURCE=.\NNTPProtocoller.imp
# End Source File
# Begin Source File

SOURCE=.\NNTPProtocoller.meta
# End Source File
# End Target
# End Project
