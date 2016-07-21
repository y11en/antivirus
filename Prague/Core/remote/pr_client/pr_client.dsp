# Microsoft Developer Studio Project File - Name="pr_client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pr_client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pr_client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pr_client.mak" CFG="pr_client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_client - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_client - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/remote/pr_client", OESDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/remote/pr_client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/remote/pr_client"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\commonfiles" /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 rpcrt4.lib /nologo /dll /debug /machine:I386 /out:"../../../out/Release/pr_client.ppl" /implib:"../../Release/pr_client.lib" /pdbtype:sept /opt:ref /ALIGN:4096 /IGNORE:4108
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\pr_client.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/remote/pr_client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/remote/pr_client"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rpcrt4.lib /nologo /dll /debug /machine:I386 /out:"../../../out/Debug/pr_client.ppl" /implib:"../../Debug/pr_client.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pr_client - Win32 Release"
# Name "pr_client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\buffer_c.cpp
# End Source File
# Begin Source File

SOURCE=.\engine_c.cpp
# End Source File
# Begin Source File

SOURCE=.\ifenum_c.cpp
# End Source File
# Begin Source File

SOURCE=.\insider_c.cpp
# End Source File
# Begin Source File

SOURCE=.\io_c.cpp
# End Source File
# Begin Source File

SOURCE=.\mchk_c.cpp
# End Source File
# Begin Source File

SOURCE=.\objptr_c.cpp
# End Source File
# Begin Source File

SOURCE=.\os_c.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_client.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_client.rc

!IF  "$(CFG)" == "pr_client - Win32 Release"

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419 /i "../../../commonfiles"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\reg_c.cpp
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_buffer.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_buffer_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_engine.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_E="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_engine_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_ifenum.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_I="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_ifenum_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_insider.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_insider_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_io.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_io.idl
InputName=rpc_io

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_IO="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_io.idl
InputName=rpc_io

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_io_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_mchk.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_mchk_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_objptr.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_O="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_objptr_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_os.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_os.idl
InputName=rpc_os

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_OS="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_os.idl
InputName=rpc_os

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_os_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_reg.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_R="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_reg_c.c
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_string.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_string.idl
InputName=rpc_string

BuildCmds= \
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RPC_S="../idl/rpc.rsp"	"../idl/$(InputName).acf"	
# Begin Custom Build - MIDL step on $(InputName)
InputPath=..\idl\rpc_string.idl
InputName=rpc_string

BuildCmds= \
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf

"$(InputName)_c.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rpc_string_c.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\string_c.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\rpc_buffer.h
# End Source File
# Begin Source File

SOURCE=.\rpc_engine.h
# End Source File
# Begin Source File

SOURCE=.\rpc_ifenum.h
# End Source File
# Begin Source File

SOURCE=.\rpc_insider.h
# End Source File
# Begin Source File

SOURCE=.\rpc_io.h
# End Source File
# Begin Source File

SOURCE=.\rpc_mchk.h
# End Source File
# Begin Source File

SOURCE=.\rpc_objptr.h
# End Source File
# Begin Source File

SOURCE=.\rpc_os.h
# End Source File
# Begin Source File

SOURCE=.\rpc_reg.h
# End Source File
# Begin Source File

SOURCE=.\rpc_string.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pr_client.dep
# End Source File
# Begin Source File

SOURCE=.\pr_client.mak
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_buffer.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_engine.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_ifenum.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_insider.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_io.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_mchk.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_objptr.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_os.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_reg.acf
# End Source File
# Begin Source File

SOURCE=..\idl\rpc_string.acf
# End Source File
# End Target
# End Project
