# Microsoft Developer Studio Project File - Name="Licensing60" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Licensing60 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lic60.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lic60.mak" CFG="Licensing60 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Licensing60 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Licensing60 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/Licensing60", JVCSAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/licence"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 licpolicy_no_xml.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib kernel32.lib ole32.lib /nologo /base:"0x64c00000" /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\out/Release/lic60.ppl" /libpath:"../../out/release" /opt:ref
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\lic60.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/licence"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 licpolicy_no_xml.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib kernel32.lib ole32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\out/Debug/lic60.ppl" /implib:"" /pdbtype:sept /libpath:"../../out/debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "Licensing60 - Win32 Release"
# Name "Licensing60 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\licensing.cpp

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\plugin_licensing60.cpp

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\1165b.h
# End Source File
# Begin Source File

SOURCE=.\1165r.h
# End Source File
# Begin Source File

SOURCE=.\1169b.h
# End Source File
# Begin Source File

SOURCE=.\1169r.h
# End Source File
# Begin Source File

SOURCE=.\1190b.h
# End Source File
# Begin Source File

SOURCE=.\1190r.h
# End Source File
# Begin Source File

SOURCE=.\1191b.h
# End Source File
# Begin Source File

SOURCE=.\1191r.h
# End Source File
# Begin Source File

SOURCE=..\Include\iface\i_licensing.h
# End Source File
# Begin Source File

SOURCE=.\licensing.h
# End Source File
# Begin Source File

SOURCE=..\Include\plugin\p_licensing60.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_licensing.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\lic60_res.h"
# End Source File
# Begin Source File

SOURCE=".\lic60_res.rc"
# End Source File
# Begin Source File

SOURCE=.\licensing.imp
# End Source File
# Begin Source File

SOURCE=.\licensing.meta
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lic60.dep
# End Source File
# Begin Source File

SOURCE=.\lic60.mak
# End Source File
# Begin Source File

SOURCE=..\Include\prt\licensing.prt
# End Source File
# End Target
# End Project
