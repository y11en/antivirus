# Microsoft Developer Studio Project File - Name="pr_remote" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pr_remote - Win32 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pr_remote.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pr_remote.mak" CFG="pr_remote - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_remote - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/remote", MDSDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pr_remote - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/remote"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/remote"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_remote_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIB" /libpath:"..\release" /libpath:"..\..\CommonFiles\ReleaseDll" /ALIGN:4096 /IGNORE:4108
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\pr_remote.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/remote"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/remote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_remote_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../../commonfiles" /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIBD" /libpath:"..\debug" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug MBCS"
# PROP BASE Intermediate_Dir "../../temp/Debug MBCS/prague/remote"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug MBCS"
# PROP Intermediate_Dir "../../temp/Debug MBCS/prague/remote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "../../commonfiles" /d "_DEBUG"
# ADD RSC /l 0x409 /i "../../commonfiles" /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib pr_client.lib pr_server.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /implib:"../Debug/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIBD" /libpath:"..\debug" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /implib:"../Debug/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\NULIBD" /libpath:"..\debug" /libpath:"..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Debug MBCS\pr_remote.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release MBCS"
# PROP BASE Intermediate_Dir "../../temp/Release MBCS/prague/remote"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release MBCS"
# PROP Intermediate_Dir "../../temp/Release MBCS/prague/remote"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib pr_client.lib pr_server.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /implib:"../Release/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIB" /libpath:"..\release" /libpath:"..\..\CommonFiles\ReleaseDll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /debug /machine:I386 /implib:"../Release/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\NULIB" /libpath:"..\release" /libpath:"..\..\CommonFiles\ReleaseDll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\out\Release MBCS
TargetPath=\out\Release MBCS\pr_remote.dll
TargetName=pr_remote
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"	addsym $(TargetDir)\$(TargetName).pdb $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "pr_remote - Win32 Release"
# Name "pr_remote - Win32 Debug"
# Name "pr_remote - Win32 Debug MBCS"
# Name "pr_remote - Win32 Release MBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "pr_sytem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pr_system\pr_system_c.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_system\pr_system_s.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_system\rpc_system_c.c
# End Source File
# Begin Source File

SOURCE=.\pr_system\rpc_system_s.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\bl_component.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_internal.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_library.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_remote.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_remote.rc
# End Source File
# Begin Source File

SOURCE=.\pr_root_c.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_root_s.cpp
# End Source File
# Begin Source File

SOURCE=.\rpc_connect.cpp
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_remote.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_remote.idl
InputName=rpc_remote

BuildCmds= \
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_remote.idl
InputName=rpc_remote

BuildCmds= \
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_remote_c.c
# End Source File
# Begin Source File

SOURCE=.\rpc_remote_s.c
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_root.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_root.idl
InputName=rpc_root

BuildCmds= \
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_root.idl
InputName=rpc_root

BuildCmds= \
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_root_c.c
# End Source File
# Begin Source File

SOURCE=.\rpc_root_s.c
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_system.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_system.idl
InputName=rpc_system

BuildCmds= \
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -out ./pr_system -I ../include/iface

"pr_system/$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pr_system/$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pr_system/$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=.\idl\rpc_system.idl
InputName=rpc_system

BuildCmds= \
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -out ./pr_system -I ../include/iface

"pr_system/$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pr_system/$(InputName)_s.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"pr_system/$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Service\sa.cpp
# End Source File
# Begin Source File

SOURCE=.\SharedTable.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cm_remote.h
# End Source File
# Begin Source File

SOURCE=.\pr_common.h
# End Source File
# Begin Source File

SOURCE=.\pr_manager.h
# End Source File
# Begin Source File

SOURCE=.\rpc_connect.h
# End Source File
# Begin Source File

SOURCE=.\rpc_remote.h
# End Source File
# Begin Source File

SOURCE=.\rpc_root.h
# End Source File
# Begin Source File

SOURCE=.\pr_system\rpc_system.h
# End Source File
# Begin Source File

SOURCE=.\SharedTable.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pr_remote.dep
# End Source File
# Begin Source File

SOURCE=.\pr_remote.mak
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_remote.acf
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_root.acf
# End Source File
# Begin Source File

SOURCE=.\idl\rpc_system.acf
# End Source File
# End Target
# End Project
