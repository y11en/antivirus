# Microsoft Developer Studio Project File - Name="CKAHUM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CKAHUM - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CKAHUM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CKAHUM.mak" CFG="CKAHUM - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CKAHUM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CKAHUM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/Kaspersky Anti-Virus Personal 5.0 MP1/Components/Windows/KAH/CKAHUM", CQHLAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CKAHUM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\temp\Release\CKAHUM"
# PROP Intermediate_Dir "..\..\..\..\temp\Release\CKAHUM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CKAHUM_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "../../../../Windows/Hook/" /I "../../../../CommonFiles" /I "..\..\..\..\klava\kernel\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CKAHUM_EXPORTS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib FSDrvLib.lib advapi32.lib rpcrt4.lib User32.lib Ws2_32.lib win32utils.lib version.lib iphlpapi.lib msvcrt.lib /nologo /base:"0x6A000000" /dll /pdb:"..\..\..\..\out\Release\CKAHUM.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\..\out\Release\CKAHUM.dll" /implib:"..\..\..\..\out\Release\CKAHUM.lib" /libpath:"../../../../CommonFiles/ReleaseDll" /libpath:"../../../../Out/Release" /opt:ref
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetPath=\Perforce\ckah\out\Release\CKAHUM.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=call tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CKAHUM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\temp\Debug\CKAHUM"
# PROP Intermediate_Dir "..\..\..\..\temp\Debug\CKAHUM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CKAHUM_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../../Windows/Hook/" /I "../../../../CommonFiles" /I "..\..\..\..\klava\kernel\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CKAHUM_EXPORTS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib FSDrvLib.lib advapi32.lib rpcrt4.lib User32.lib Ws2_32.lib win32utils.lib version.lib iphlpapi.lib /nologo /base:"0x6A000000" /dll /pdb:"..\..\..\..\out\Debug\CKAHUM.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\..\..\out\Debug\CKAHUM.dll" /implib:"..\..\..\..\out\Debug\CKAHUM.lib" /pdbtype:sept /libpath:"../../../../CommonFiles/DebugDll" /libpath:"../../../../Out/Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CKAHUM - Win32 Release"
# Name "CKAHUM - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "IDS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CKAHBanomet.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHIDS.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahids.h
# End Source File
# Begin Source File

SOURCE=.\IDS.cpp
# End Source File
# Begin Source File

SOURCE=.\IDS.h
# End Source File
# Begin Source File

SOURCE=.\IDSClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IDSClient.h
# End Source File
# End Group
# Begin Group "Stealth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CKAHStM.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahstm.h
# End Source File
# End Group
# Begin Group "Firewall"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ApplicationClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplicationClient.h
# End Source File
# Begin Source File

SOURCE=.\CKAHApplicationRules.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHApplicationRules.h
# End Source File
# Begin Source File

SOURCE=.\CKAHFW.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahfw.h
# End Source File
# Begin Source File

SOURCE=.\CKAHNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahnetwork.h
# End Source File
# Begin Source File

SOURCE=.\CKAHPacketRules.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHPacketRules.h
# End Source File
# Begin Source File

SOURCE=.\CKAHPorts.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahports.h
# End Source File
# Begin Source File

SOURCE=.\MD5Hash.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Hash.h
# End Source File
# Begin Source File

SOURCE=.\PacketClient.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketClient.h
# End Source File
# Begin Source File

SOURCE=.\PathFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\PathFuncs.h
# End Source File
# End Group
# Begin Group "IFace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CKAHMain.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHMain.h
# End Source File
# Begin Source File

SOURCE=.\CKAHUM.cpp
# End Source File
# Begin Source File

SOURCE=..\include\ckahum.h
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\avpsrvex.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PluginUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\RebootCheckHlp.cpp
# End Source File
# Begin Source File

SOURCE=.\Services.cpp
# End Source File
# Begin Source File

SOURCE=.\Unpacker.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# End Group
# Begin Group "Redirector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\ckahcr.h
# End Source File
# Begin Source File

SOURCE=.\CKAHReDirector.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CKAHManifest.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFileVer.cpp
# End Source File
# Begin Source File

SOURCE=.\StartedPlugins.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\avpsrvex.h
# End Source File
# Begin Source File

SOURCE=.\CKAHManifest.h
# End Source File
# Begin Source File

SOURCE=.\CKAHStorage.h
# End Source File
# Begin Source File

SOURCE=.\ClientIDs.h
# End Source File
# Begin Source File

SOURCE=.\GetFileVer.h
# End Source File
# Begin Source File

SOURCE=.\PluginUtils.h
# End Source File
# Begin Source File

SOURCE=.\PoliStorage.h
# End Source File
# Begin Source File

SOURCE=.\PoliType.h
# End Source File
# Begin Source File

SOURCE=.\RebootCheckHlp.h
# End Source File
# Begin Source File

SOURCE=.\Services.h
# End Source File
# Begin Source File

SOURCE=.\StartedPlugins.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\stringdefs.h
# End Source File
# Begin Source File

SOURCE=.\UnicodeConv.h
# End Source File
# Begin Source File

SOURCE=.\Unpacker.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\CKAHUM.RC
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\Version\ver_mod.rc2
# End Source File
# End Group
# Begin Group "External"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\Stuff\RegStorage.cpp
# End Source File
# End Group
# End Target
# End Project
