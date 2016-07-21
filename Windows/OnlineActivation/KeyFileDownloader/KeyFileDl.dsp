# Microsoft Developer Studio Project File - Name="KeyFileDownloader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=KeyFileDownloader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KeyFileDl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KeyFileDl.mak" CFG="KeyFileDownloader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KeyFileDownloader - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "KeyFileDownloader - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/OnlineActivation/KeyFileDownloader", VPUEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KeyFileDownloader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KeyFileDownloader___Win32_Release_Dll"
# PROP BASE Intermediate_Dir "KeyFileDownloader___Win32_Release_Dll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/release/"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KEYFILEDOWNLOADER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KEYFILEDOWNLOADER_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib wininet.lib KLKeyRec.lib Property.lib DTUtils.lib WinAvpIO.lib Win32Utils.lib KLDTSer.lib sign.lib swm.lib advapi32.lib /nologo /dll /machine:I386 /out:"../../../out/releases/keyfiledl.dll" /implib:"../../../out/releases/keyfiledl.lib" /libpath:"..\..\..\Commonfiles\release\\"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib wininet.lib ..\..\..\Commonfiles\releasedll\KLKeyRec.lib Property.lib DTUtils.lib WinAvpIO.lib Win32Utils.lib KLDTSer.lib sign.lib swm.lib advapi32.lib /nologo /dll /debug /machine:I386 /libpath:"../../../out/release" /opt:ref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "KeyFileDownloader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "KeyFileDownloader___Win32_Debug_Dll"
# PROP BASE Intermediate_Dir "KeyFileDownloader___Win32_Debug_Dll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KEYFILEDOWNLOADER_EXPORTS" /FD /GZ /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KEYFILEDOWNLOADER_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib wininet.lib KLKeyRec.lib Property.lib DTUtils.lib WinAvpIO.lib Win32Utils.lib KLDTSer.lib sign.lib swm.lib advapi32.lib /nologo /dll /debug /machine:I386 /out:"../../../out/debugs/keyfiledl.dll" /implib:"../../../out/debugs/keyfiledl.lib" /pdbtype:sept /libpath:"..\..\..\Commonfiles\debug\\"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib wininet.lib ..\..\..\CommonFiles\DebugDll\KLKeyRec.lib Property.lib DTUtils.lib WinAvpIO.lib Win32Utils.lib KLDTSer.lib sign.lib swm.lib advapi32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../../out/debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "KeyFileDownloader - Win32 Release"
# Name "KeyFileDownloader - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cmndefs.cpp
# End Source File
# Begin Source File

SOURCE=.\httpmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\keyfiledl.cpp
# End Source File
# Begin Source File

SOURCE=.\keyfiledl.def
# End Source File
# Begin Source File

SOURCE=.\workerthr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cmndefs.h
# End Source File
# Begin Source File

SOURCE=.\httpmsg.h
# End Source File
# Begin Source File

SOURCE=.\keyfiledl.h
# End Source File
# Begin Source File

SOURCE=.\workerthr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
