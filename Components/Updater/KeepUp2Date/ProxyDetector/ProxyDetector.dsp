# Microsoft Developer Studio Project File - Name="ProxyDetector" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ProxyDetector - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProxyDetector.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProxyDetector.mak" CFG="ProxyDetector - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProxyDetector - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProxyDetector - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ProxyDetector"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProxyDetector - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PROXYDETECTOR_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "../../KeepUp2Date/Client/" /I "../../../CommonFiles/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PROXYDETECTOR_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 advapi32.lib wininet.lib upd_core.lib /nologo /dll /pdb:"../../../out/Release/proxyDetector.pdb" /debug /machine:I386 /out:"../../../out/Release/proxyDetector.dll" /implib:"../../../out/Release/proxyDetector.lib" /pdbtype:sept /libpath:"../../../out/Release/"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ProxyDetector - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PROXYDETECTOR_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../KeepUp2Date/Client/" /I "../../../CommonFiles/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PROXYDETECTOR_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib wininet.lib upd_core.lib /nologo /dll /pdb:"../../../out/Debug/proxyDetector.pdb" /debug /machine:I386 /out:"../../../out/Debug/proxyDetector.dll" /implib:"../../../out/Debug/proxyDetector.lib" /pdbtype:sept /libpath:"../../../out/Debug/"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ProxyDetector - Win32 Release"
# Name "ProxyDetector - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\JSProxyWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\proxyDetector.cpp
# End Source File
# Begin Source File

SOURCE=.\registryReader.cpp
# End Source File
# Begin Source File

SOURCE=.\windowsRegistryWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\winInetReader.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\commonHeaders.h
# End Source File
# Begin Source File

SOURCE=.\JSProxyWrapper.h
# End Source File
# Begin Source File

SOURCE=.\proxyDetector.h
# End Source File
# Begin Source File

SOURCE=.\registryReader.h
# End Source File
# Begin Source File

SOURCE=.\windowsRegistryWrapper.h
# End Source File
# Begin Source File

SOURCE=.\winInet_NotFromSDK.h
# End Source File
# Begin Source File

SOURCE=.\winInetReader.h
# End Source File
# End Group
# End Target
# End Project
