# Microsoft Developer Studio Project File - Name="ipc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ipc - Win32 DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ipc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ipc.mak" CFG="ipc - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ipc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipc - Win32 ReleaseDll" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipc - Win32 DebugDll" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ipc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../Out/ReleaseS"
# PROP Intermediate_Dir "../../../../Temp/ReleaseS/CommonFiles/windows/ipc/klipc.dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G4 /MT /W3 /GX /Zi /O2 /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /debug /machine:I386 /def:".\ipc.def" /out:"../../../../Out/ReleaseS/klipc.dll" /pdbtype:sept /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
InputPath=\Out\ReleaseS\klipc.dll
InputName=klipc
SOURCE="$(InputPath)"

"..\..\..\Release\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\Release

# End Custom Build
# Begin Special Build Tool
TargetPath=\Out\ReleaseS\klipc.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ipc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Out/DebugS"
# PROP Intermediate_Dir "../../../../Temp/DebugS/CommonFiles/windows/ipc/klipc.dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G4 /MTd /W3 /GX /Zi /Od /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../../../Out/DebugS/klipc.dll" /pdbtype:sept
# Begin Custom Build
InputPath=\Out\DebugS\klipc.dll
InputName=klipc
SOURCE="$(InputPath)"

"..\..\..\Debug\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "ipc - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ipc___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "ipc___Win32_ReleaseDll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../Out/Release"
# PROP Intermediate_Dir "../../../../Temp/Release/CommonFiles/windows/ipc/klipc.dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MD /W3 /GX /Zi /O2 /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /Zi /O2 /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /debug /machine:I386 /def:".\ipc.def" /out:"../../../../Out/Release/klipc.dll" /pdbtype:sept /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
InputPath=\Out\Release\klipc.dll
InputName=klipc
SOURCE="$(InputPath)"

"..\..\..\ReleaseDll\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\ReleaseDll

# End Custom Build
# Begin Special Build Tool
TargetPath=\Out\Release\klipc.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ipc - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ipc___Win32_DebugDll"
# PROP BASE Intermediate_Dir "ipc___Win32_DebugDll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Out/Debug"
# PROP Intermediate_Dir "../../../../Temp/Debug/CommonFiles/windows/ipc/klipc.dll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G4 /MDd /W3 /GX /Zi /Od /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /GX /Zi /Od /I "..\..\.." /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IPC_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /incremental:no /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib advapi32.lib rpcrt4.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../../../Out/Debug/klipc.dll" /pdbtype:sept
# Begin Custom Build
InputPath=\Out\Debug\klipc.dll
InputName=klipc
SOURCE="$(InputPath)"

"..\..\..\DebugDll\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\DebugDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "ipc - Win32 Release"
# Name "ipc - Win32 Debug"
# Name "ipc - Win32 ReleaseDll"
# Name "ipc - Win32 DebugDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\channel.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc.def

!IF  "$(CFG)" == "ipc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ipc - Win32 Debug"

!ELSEIF  "$(CFG)" == "ipc - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ipc - Win32 DebugDll"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\runtime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Public"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ipc.h
# End Source File
# Begin Source File

SOURCE=..\ipc_def.h
# End Source File
# Begin Source File

SOURCE=..\ipc_err.h
# End Source File
# Begin Source File

SOURCE=..\load_ipc.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\load_ipc.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ipc_impl.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ipc.rc
# End Source File
# End Group
# End Target
# End Project
