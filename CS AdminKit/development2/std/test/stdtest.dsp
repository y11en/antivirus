# Microsoft Developer Studio Project File - Name="stdtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=stdtest - Win32 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stdtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stdtest.mak" CFG="stdtest - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stdtest - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stdtest - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stdtest - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stdtest - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CS AdminKit/development2/std/test", VYJWAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stdtest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\dll"
# PROP Intermediate_Dir "..\..\bin\debug\std\test\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDTEST_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "STDTEST_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "WIN32" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /dll /map /debug /machine:I386 /libpath:"O:\CommonFiles\ReleaseDll\\" /libpath:"../../lib/release"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "stdtest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin\dlld"
# PROP Intermediate_Dir "..\..\bin\debug\std\test\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDTEST_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "UNICODE" /D "_DEBUG" /D "WIN32" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /libpath:"O:\CommonFiles\DebugDll\\" /libpath:"../../lib/debug"

!ELSEIF  "$(CFG)" == "stdtest - Win32 Release MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "stdtest___Win32_Release_MBCS"
# PROP BASE Intermediate_Dir "stdtest___Win32_Release_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\nudll"
# PROP Intermediate_Dir "..\..\bin\release_mbcs\std\test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "STDTEST_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "WIN32" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /Ob2 /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "NDEBUG" /D "STDTEST_EXPORTS" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_STLP_NEW_PLATFORM_SDK" /D "WIN32" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /dll /map /debug /machine:I386 /libpath:"O:\CommonFiles\ReleaseDll\\" /libpath:"../../lib/release"
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib delayimp.lib /nologo /dll /map /debug /machine:I386 /implib:"../../lib/release_mbcs/stdtest.lib" /libpath:"O:\CommonFiles\ReleaseDll\\" /libpath:"../../lib/release_mbcs"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "stdtest - Win32 Debug MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stdtest___Win32_Debug_MBCS"
# PROP BASE Intermediate_Dir "stdtest___Win32_Debug_MBCS"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin\nudlld"
# PROP Intermediate_Dir "..\..\bin\debug_mbcs\std\test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /D "UNICODE" /D "_DEBUG" /D "WIN32" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "../../include" /I "../../gsoap" /I "../../tst/kltester/" /I "../../gsoap/std" /I "../../include/stlport/stlport" /D "_STLP_DEBUG" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_STLP_NEW_PLATFORM_SDK" /D "WIN32" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /libpath:"O:\CommonFiles\DebugDll\\" /libpath:"../../lib/debug"
# ADD LINK32 klcsrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /implib:"../../lib/debug_mbcs/stdtest.lib" /libpath:"O:\CommonFiles\DebugDll\\" /libpath:"../../lib/debug_mbcs"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "stdtest - Win32 Release"
# Name "stdtest - Win32 Debug"
# Name "stdtest - Win32 Release MBCS"
# Name "stdtest - Win32 Debug MBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\err\errtest.cpp
# End Source File
# Begin Source File

SOURCE=.\par\partest.cpp
# End Source File
# Begin Source File

SOURCE=.\tp\queuetest.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\schtest.cpp
# End Source File
# Begin Source File

SOURCE=.\sch\schtestfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\stdtest.cpp
# End Source File
# Begin Source File

SOURCE=.\testing\testing.cpp
# End Source File
# Begin Source File

SOURCE=.\thr\thrtest.cpp
# End Source File
# Begin Source File

SOURCE=.\tp\tptest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\err\errtest.h
# End Source File
# Begin Source File

SOURCE=.\par\partest.h
# End Source File
# Begin Source File

SOURCE=.\tp\queuetest.h
# End Source File
# Begin Source File

SOURCE=.\sch\schtest.h
# End Source File
# Begin Source File

SOURCE=.\testing\testing.h
# End Source File
# Begin Source File

SOURCE=.\thr\thrtest.h
# End Source File
# Begin Source File

SOURCE=.\tp\tptest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\input.txt
# End Source File
# End Target
# End Project
