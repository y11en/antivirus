# Microsoft Developer Studio Project File - Name="diffs_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=diffs_vc6 - Win32 Debug STLPort
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "diffs_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "diffs_vc6.mak" CFG="diffs_vc6 - Win32 Debug STLPort"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "diffs_vc6 - Win32 Debug STLPort" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "diffs_vc6 - Win32 Release STLPort" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "diffs_vc6 - Win32 Debug STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug STLPort"
# PROP BASE Intermediate_Dir "Debug STLPort"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug STLPort"
# PROP Intermediate_Dir "Debug STLPort"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../../external/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVPACK_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../../external/" /I "../../CS AdminKit/development2/include/stlport/stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVPACK_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /D "_STLP_DEBUG" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "../" /d "_DEBUG"
# ADD RSC /l 0x419 /i "../" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/diffs_vc6.dll" /implib:"../../out/Debug/diffs_vc6.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib /nologo /dll /pdb:"../../CS AdminKit/development2/bin/dlld/diffsAK.pdb" /debug /machine:I386 /out:"../../CS AdminKit/development2/bin/dlld/diffsAK.dll" /implib:"../../CS AdminKit/development2/lib/debug/diffsAK.lib" /pdbtype:sept /libpath:"../../CS AdminKit/development2/lib/debug/"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "diffs_vc6 - Win32 Release STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release STLPort"
# PROP BASE Intermediate_Dir "Release STLPort"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release STLPort"
# PROP Intermediate_Dir "Release STLPort"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../external/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVPACK_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../external/" /I "../../CS AdminKit/development2/include/stlport/stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVPACK_EXPORTS" /D "_STLP_NEW_PLATFORM_SDK" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "../" /d "NDEBUG"
# ADD RSC /l 0x419 /i "../" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../out/Release/diffs_vc6.dll" /implib:"../../out/Release/diffs_vc6.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 klcsrt.lib /nologo /dll /pdb:"../../CS AdminKit/development2/bin/dll/diffsAK.pdb" /machine:I386 /out:"../../CS AdminKit/development2/bin/dll/diffsAK.dll" /implib:"../../CS AdminKit/development2/lib/release/diffsAK.lib" /libpath:"../../CS AdminKit/development2/lib/release/"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "diffs_vc6 - Win32 Debug STLPort"
# Name "diffs_vc6 - Win32 Release STLPort"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\diffs.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\comdefs.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "klavpacklib"

# PROP Default_Filter ""
# Begin Group "KFB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\klava\klavpacklib\kfb\kfb_common.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\kfb\kfb_decode.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\kfb\kfb_encode.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\kfb\kfb_pack.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\klava\klavpacklib\asm_loader.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\dlist.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\dlist.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\klavpack_decode.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\klavpack_decode.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\klavpack_encode.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\klavpack_encode.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\loader.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\loader.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\loader_import.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\loader_import.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\pelib.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\pelib.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\section_writer.cpp
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\section_writer.h
# End Source File
# Begin Source File

SOURCE=.\klava\klavpacklib\types.h
# End Source File
# End Group
# End Target
# End Project
