# Microsoft Developer Studio Project File - Name="rar" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rar - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rar.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rar.mak" CFG="rar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rar - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/rar", LSGDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rar - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/MiniArchiver/Rar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/MiniArchiver/Rar"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib m_utils.lib /nologo /base:"0x66b00000" /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Release/rar.ppl" /libpath:"..\..\..\out\Release" /libpath:"..\..\..\CommonFiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\rar.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "rar - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/MiniArchiver/Rar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/MiniArchiver/Rar"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/rar.ppl" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "rar - Win32 Release"
# Name "rar - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\rar.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\base\const.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\base\proto.h
# End Source File
# Begin Source File

SOURCE=.\rar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\rar.imp
# End Source File
# Begin Source File

SOURCE=.\rar.meta
# End Source File
# Begin Source File

SOURCE=.\rar.rc
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\cblock.c
# End Source File
# Begin Source File

SOURCE=.\base\common.c
# End Source File
# Begin Source File

SOURCE=.\base\crc.c
# End Source File
# Begin Source File

SOURCE=.\base\crypt.c
# End Source File
# Begin Source File

SOURCE=.\base\input.c
# End Source File
# Begin Source File

SOURCE=.\base\mem.c
# End Source File
# Begin Source File

SOURCE=.\base\model.c
# End Source File
# Begin Source File

SOURCE=.\base\mtree.c
# End Source File
# Begin Source File

SOURCE=.\base\pack20.c
# End Source File
# Begin Source File

SOURCE=.\base\rarvm.c
# End Source File
# Begin Source File

SOURCE=.\base\rijndael.c
# End Source File
# Begin Source File

SOURCE=.\base\sha1.c

!IF  "$(CFG)" == "rar - Win32 Release"

# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "rar - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\base\stdflt.c
# End Source File
# Begin Source File

SOURCE=.\base\suballoc.c
# End Source File
# Begin Source File

SOURCE=.\base\unpack15.c
# End Source File
# Begin Source File

SOURCE=.\base\unpack20.c
# End Source File
# Begin Source File

SOURCE=.\base\unpack29.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\rar.dep
# End Source File
# Begin Source File

SOURCE=.\rar.dsc
# End Source File
# Begin Source File

SOURCE=.\rar.mak
# End Source File
# End Target
# End Project
