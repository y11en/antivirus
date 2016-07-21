# Microsoft Developer Studio Project File - Name="libola" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libola - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libola.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libola.mak" CFG="libola - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libola - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libola - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libola - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libola - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libola - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/Release"
# PROP Intermediate_Dir "../../../../../Temp/Release/Windows/OnlineActivation/Libola"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../../../../CommonFiles" /I "$(BOOST_DIR)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 wininet.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "libola - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/Debug"
# PROP Intermediate_Dir "../../../../../Temp/Debug/Windows/OnlineActivation/Libola"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../../CommonFiles" /I "$(BOOST_DIR)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /FD /GZ /c
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
# ADD LINK32 wininet.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "libola - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libola___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "libola___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/ReleaseU"
# PROP Intermediate_Dir "../../../../../Temp/ReleaseU/Windows/OnlineActivation/Libola"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../../../../CommonFiles" /I "$(BOOST_DIR)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_USRDLL" /D "LIBOLA_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib /nologo /dll /machine:I386
# ADD LINK32 wininet.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "libola - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libola___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "libola___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/DebugU"
# PROP Intermediate_Dir "../../../../../Temp/DebugU/Windows/OnlineActivation/Libola"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOLA_EXPORTS" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../../CommonFiles" /I "$(BOOST_DIR)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_USRDLL" /D "LIBOLA_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wininet.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "libola - Win32 Release"
# Name "libola - Win32 Debug"
# Name "libola - Win32 Release Unicode"
# Name "libola - Win32 Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\Activation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ActivationError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ActivationFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpResponse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\InternetSession.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Request.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Response.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\Activation.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ActivationError.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ActivationFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ActivationTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Definitions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpRequest.h
# End Source File
# Begin Source File

SOURCE=..\..\src\HttpResponse.h
# End Source File
# Begin Source File

SOURCE=..\..\src\InternetObserver.h
# End Source File
# Begin Source File

SOURCE=..\..\src\InternetSession.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OnlineActivation.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Request.h
# End Source File
# Begin Source File

SOURCE=..\..\include\RequestObserver.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Response.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SmartPointer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Typedefs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
