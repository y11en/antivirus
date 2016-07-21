# Microsoft Developer Studio Project File - Name="Script Checker Interceptor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Script Checker Interceptor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "scr_ch_pg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scr_ch_pg.mak" CFG="Script Checker Interceptor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Script Checker Interceptor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Script Checker Interceptor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/SC/SC_Interceptor2", FDMIAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "avpscrch___Win32_Release"
# PROP BASE Intermediate_Dir "avpscrch___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/ppp/sc/interceptor"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /I "..\..\..\..\Prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "_PRAGUE" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /I "..\..\..\Prague\include" /I "..\..\..\PPP\include" /I "..\..\..\PPP\basegui" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "_PRAGUE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\include" /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib uuid.lib odbc32.lib odbccp32.lib oleaut32.lib sign.lib Win32Utils.lib .\scrch_ag\uuid.lib /nologo /subsystem:windows /dll /pdb:".Release_Prague\avpscrch.pdb" /debug /machine:I386 /libpath:"..\..\..\CommonFiles\ReleaseDll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ad1.lib odbc32.lib odbccp32.lib oleaut32.lib sign.lib Win32Utils.lib FSDrvLib.lib /nologo /base:"0x67500000" /subsystem:windows /dll /debug /machine:I386 /def:".\avpscrch.def" /out:"../../../out/Release/scrchpg.dll" /libpath:"..\..\..\out\release" /libpath:"..\..\..\CommonFiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\scrchpg.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Post build step go!
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "avpscrch___Win32_Debug"
# PROP BASE Intermediate_Dir "avpscrch___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/ppp/sc/sc_interceptor"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /I "..\..\..\..\Prague\include" /I "..\..\..\..\PPP\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_SHOWDEBUG" /D "_PRAGUE" /FR /YX /FD /GZ /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\CommonFiles" /I "..\..\..\Prague\include" /I "..\..\..\PPP\include" /I "..\..\..\PPP\basegui" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_SHOWDEBUG" /D "_PRAGUE" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\include" /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sign.lib Win32Utils.lib .\scrch_ag\uuid.lib /nologo /subsystem:windows /dll /debug /debugtype:both /machine:I386 /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDLL"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ad1.lib odbc32.lib odbccp32.lib sign.lib Win32Utils.lib FSDrvLib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/scrchpg.dll" /libpath:"..\..\..\out\debug" /libpath:"..\..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "Script Checker Interceptor - Win32 Release"
# Name "Script Checker Interceptor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActiveScriptHooks.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\appinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\avpscrch.cpp
# End Source File
# Begin Source File

SOURCE=.\CalcSum.c
# End Source File
# Begin Source File

SOURCE=.\ChkScript.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\debug.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\decode.c
# End Source File
# Begin Source File

SOURCE=.\gmutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Windows\Hook\R3Hook\hookbase64.c
# End Source File
# Begin Source File

SOURCE=.\HookClassFactory.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hooksymb.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IeStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\InstanceQ.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scplugins.cpp
# End Source File
# Begin Source File

SOURCE=.\scr_inst.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\signchk.c
# End Source File
# Begin Source File

SOURCE=.\siteinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Windows\Hook\R3Hook\sputnik.c
# End Source File
# Begin Source File

SOURCE=.\StatusBarIcon.c
# End Source File
# Begin Source File

SOURCE=.\SymbHooks_JS.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SymbHooks_VBS.cpp

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\scripting\activdbg.h
# End Source File
# Begin Source File

SOURCE=.\ActiveScriptHooks.h
# End Source File
# Begin Source File

SOURCE=.\scripting\activscp.h
# End Source File
# Begin Source File

SOURCE=.\appinfo.h
# End Source File
# Begin Source File

SOURCE=.\CalcSum.h
# End Source File
# Begin Source File

SOURCE=.\CCriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\ChkScript.h
# End Source File
# Begin Source File

SOURCE=.\CommonStruct.h
# End Source File
# Begin Source File

SOURCE=.\CRC.H
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\decode.h
# End Source File
# Begin Source File

SOURCE=.\Defines.h
# End Source File
# Begin Source File

SOURCE=.\gmutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Windows\Hook\R3Hook\hookbase64.h
# End Source File
# Begin Source File

SOURCE=.\HookClassFactory.h
# End Source File
# Begin Source File

SOURCE=.\hooksymb.h
# End Source File
# Begin Source File

SOURCE=.\IeStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\IMimeMessageW.h
# End Source File
# Begin Source File

SOURCE=.\InstanceQ.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\scplugin.h
# End Source File
# Begin Source File

SOURCE=.\scplugins.h
# End Source File
# Begin Source File

SOURCE=.\scr_inst.h
# End Source File
# Begin Source File

SOURCE=.\signchk.h
# End Source File
# Begin Source File

SOURCE=.\siteinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Windows\Hook\R3Hook\sputnik.h
# End Source File
# Begin Source File

SOURCE=.\StatusBarIcon.h
# End Source File
# Begin Source File

SOURCE=.\SymbHooks_JS.h
# End Source File
# Begin Source File

SOURCE=.\SymbHooks_VBS.h
# End Source File
# Begin Source File

SOURCE=.\ver_file.h
# End Source File
# Begin Source File

SOURCE=.\ver_prod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avpscrch.def

!IF  "$(CFG)" == "Script Checker Interceptor - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Script Checker Interceptor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\avpscrch.rc
# End Source File
# Begin Source File

SOURCE=.\bmpkav_d.bmp
# End Source File
# Begin Source File

SOURCE=.\kav16x16.bmp
# End Source File
# Begin Source File

SOURCE=.\scriptcheck.ico
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
# Section avpscrch : {00000000-0000-0000-0000-800000800000}
# 	1:13:IDD_DSETTINGS:102
# End Section
