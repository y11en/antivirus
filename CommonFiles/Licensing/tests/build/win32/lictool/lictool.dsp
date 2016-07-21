# Microsoft Developer Studio Project File - Name="lictool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=lictool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lictool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lictool.mak" CFG="lictool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lictool - Win32 Unicode DebugDll" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 DebugDll" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 ReleaseDll" (based on "Win32 (x86) Console Application")
!MESSAGE "lictool - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing/tests/build/win32/lictool", GOBIAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lictool - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lictool___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "lictool___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../Out/DebugU"
# PROP Intermediate_Dir "../../../../../../Temp/DebugU/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k_DLL_RTL/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\DebugDll" /libpath:"..\..\..\..\bin\Debug2k_DLL_RTL"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../DebugDll" /libpath:"../../../../../../Out/DebugU"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lictool___Win32_Unicode_Debug_StaticRTL"
# PROP BASE Intermediate_Dir "lictool___Win32_Unicode_Debug_StaticRTL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../Out/DebugUS"
# PROP Intermediate_Dir "../../../../../../Temp/DebugUS/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\Debug" /libpath:"..\..\..\..\bin\Debug2k"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../Debug" /libpath:"../../../../../../Out/DebugUS"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lictool___Win32_Unicode_Release_StaticRTL"
# PROP BASE Intermediate_Dir "lictool___Win32_Unicode_Release_StaticRTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../../../../../Temp/ReleaseUS/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k/pdb/lictool.pdb" /debug /machine:I386 /libpath:"..\..\..\..\..\Release" /libpath:"..\..\..\..\bin\Release2k"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k/pdb/lictool.pdb" /debug /machine:I386 /libpath:"../../../../../Release" /libpath:"../../../../../../Out/ReleaseUS"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lictool___Win32_Debug_StaticRTL"
# PROP BASE Intermediate_Dir "lictool___Win32_Debug_StaticRTL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../Out/DebugS"
# PROP Intermediate_Dir "../../../../../../Temp/DebugS/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k_MBCS/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\Debug" /libpath:"..\..\..\..\bin\Debug2k_MBCS"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k_MBCS/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../Debug" /libpath:"../../../../../../Out/DebugS"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lictool___Win32_Release_StaticRTL"
# PROP BASE Intermediate_Dir "lictool___Win32_Release_StaticRTL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../Out/ReleaseS"
# PROP Intermediate_Dir "../../../../../../Temp/ReleaseS/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_MBCS/pdb/lictool.pdb" /debug /machine:I386 /libpath:"..\..\..\..\..\Release" /libpath:"..\..\..\..\bin\Release2k_MBCS"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_MBCS/pdb/lictool.pdb" /debug /machine:I386 /libpath:"../../../../../Release" /libpath:"../../../../../../Out/ReleaseS"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lictool___Win32_Debug"
# PROP BASE Intermediate_Dir "lictool___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../../Out/Debug"
# PROP Intermediate_Dir "../../../../../../Temp/Debug/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\environment\include\win32" /I "..\..\..\include" /I "..\..\..\..\..\property" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\source\include" /I "..\..\include" /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k_DLL_RTL_MBCS/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\DebugDll" /libpath:"..\..\..\..\bin\Debug2k_DLL_RTL_MBCS"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Debug2k_DLL_RTL_MBCS/pdb/lictool.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../DebugDll" /libpath:"../../../../../../Out/Debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lictool___Win32_Release"
# PROP BASE Intermediate_Dir "lictool___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../Out/Release"
# PROP Intermediate_Dir "../../../../../../Temp/Release/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_DLL_RTL_MBCS/pdb/lictool.pdb" /debug /machine:I386 /libpath:"..\..\..\..\..\ReleaseDll" /libpath:"..\..\..\..\bin\Release2k_DLL_RTL_MBCS"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_DLL_RTL_MBCS/pdb/lictool.pdb" /debug /machine:I386 /libpath:"../../../../../ReleaseDll" /libpath:"../../../../../../Out/Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lictool - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lictool___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "lictool___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../../Out/ReleaseU"
# PROP Intermediate_Dir "../../../../../../Temp/ReleaseU/CommonFiles/Licensing/tests/build/win32/"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\include" /I "..\..\..\include" /I "..\..\..\..\..\property" /I "..\..\..\environment\include\win32" /I "..\..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_DLL_RTL/pdb/lictool.pdb" /debug /machine:I386 /libpath:"..\..\..\..\..\ReleaseDll" /libpath:"..\..\..\..\bin\Release2k_DLL_RTL"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib KLDTSer.lib property.lib win32utils.lib winavpio.lib swm.lib sign.lib licpolicy.lib /nologo /subsystem:console /pdb:"..\..\..\..\bin\Release2k_DLL_RTL/pdb/lictool.pdb" /debug /machine:I386 /libpath:"../../../../../ReleaseDll" /libpath:"../../../../../../Out/ReleaseU"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "lictool - Win32 Unicode DebugDll"
# Name "lictool - Win32 Unicode Debug"
# Name "lictool - Win32 Unicode Release"
# Name "lictool - Win32 Debug"
# Name "lictool - Win32 Release"
# Name "lictool - Win32 DebugDll"
# Name "lictool - Win32 ReleaseDll"
# Name "lictool - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\lictool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\lictool_main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\lictool.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\..\..\src\licpolicy_msg.rc
# End Source File
# End Group
# End Target
# End Project
