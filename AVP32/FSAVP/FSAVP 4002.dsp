# Microsoft Developer Studio Project File - Name="FSAVP 4002" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FSAVP 4002 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FSAVP 4002.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSAVP 4002.mak" CFG="FSAVP 4002 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSAVP 4002 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FSAVP 4002 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FSAVP 4002 - Win32 Release Static" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/FSAVP", MDEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FSAVP 4002 - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "Release\402"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /Zi /I "..\\" /I "..\GKNT2" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(BASEDIR)\inc" /I "..\..\Commonfiles" /D "_WIN32_WINNT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D _DLL=1 /D FPO=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /i "$(BASEDIR)\inc" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib vdw.lib property.lib kldtser.lib sign.lib swm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../out/release/FSAVP402.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free" /libpath:"..\..\out\releases" /libpath:"..\..\commonfiles\release" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -optidata -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:notmapped,MINIMAL /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\release\FSAVP402.sys
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=rebase -b 0x10000 -x . -a $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "Debug\402"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /Gz /MTd /W3 /Zi /Od /I "..\\" /I "..\GKNT2" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(BASEDIR)\inc" /I "i386\\" /I "." /I "..\..\Commonfiles" /D "_WIN32_WINNT" /D "_TRACER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D _DLL=1 /D DBG=1 /D FPO=0 /YX /FD /Zel -cbstring /QIfdiv- /QIf /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "$(BASEDIR)\inc" /i "..\..\commonfiles" /d "_DEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib vdw.lib chkesp.obj /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../out/debug/FSAVP402.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERWORKS)\lib\i386\checked" /libpath:"..\..\commonfiles\release" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4075 -MERGE:.rdata=.text -optidata -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:notmapped,FULL
# SUBTRACT LINK32 /pdb:none /incremental:no /map
# Begin Special Build Tool
TargetPath=\out\debug\FSAVP402.sys
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=rebase -b 0x10000 -x . -a $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Release Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FSAVP_4002___Win32_Release_Static"
# PROP BASE Intermediate_Dir "FSAVP_4002___Win32_Release_Static"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release_static"
# PROP Intermediate_Dir ".\release_static"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /MT /W3 /Zi /I "..\\" /I "..\GKNT2" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(BASEDIR)\inc" /I "..\..\Commonfiles" /D "_WIN32_WINNT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D _DLL=1 /D FPO=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Gz /MT /W3 /Zi /I ".\\" /I "..\\" /I "..\GKNT2" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(BASEDIR)\inc" /I "..\..\Commonfiles" /D "DONT_USE_FILEIO_CACHE" /D "USE_FM" /D "_WIN32_WINNT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D _DLL=1 /D FPO=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /i "$(BASEDIR)\inc" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
# ADD RSC /l 0x419 /i "$(BASEDIR)\inc" /i "..\..\commonfiles" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 int64.lib ntoskrnl.lib hal.lib vdw.lib property.lib kldtser.lib sign.lib swm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../out/release/FSAVP402.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free" /libpath:"..\..\out\releases" /libpath:"..\..\commonfiles\release" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -optidata -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:notmapped,MINIMAL /MAPINFO:LINES
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib vdw.lib property.lib kldtser.lib sign.lib swm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"..\..\out\release_static/FSAVP402.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free" /libpath:"..\..\out\release_static" /libpath:"..\..\commonfiles\release" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -optidata -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:notmapped,MINIMAL /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\release_static\FSAVP402.sys
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=rebase -b 0x10000 -x . -a $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FSAVP 4002 - Win32 Release"
# Name "FSAVP 4002 - Win32 Debug"
# Name "FSAVP 4002 - Win32 Release Static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DBCleanUp\DBCleanUp.cpp

!IF  "$(CFG)" == "FSAVP 4002 - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Debug"

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Release Static"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DBCleanUp\DirIteratorsNT.cpp

!IF  "$(CFG)" == "FSAVP 4002 - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Debug"

!ELSEIF  "$(CFG)" == "FSAVP 4002 - Win32 Release Static"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FSAVP.cpp
# End Source File
# Begin Source File

SOURCE=.\FSAVP.rc
# End Source File
# Begin Source File

SOURCE=.\FSAVPDev.cpp
# End Source File
# Begin Source File

SOURCE=.\FSBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Sysio.cpp
# End Source File
# Begin Source File

SOURCE=.\Syssign.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Avpioctl.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\BaseAPI.h
# End Source File
# Begin Source File

SOURCE=..\DBCleanUp\DBCleanUp.h
# End Source File
# Begin Source File

SOURCE=.\FSAVP.h
# End Source File
# Begin Source File

SOURCE=.\FSAVPDev.h
# End Source File
# Begin Source File

SOURCE=.\FSBase.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=..\Gknt2\Gkapi.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\RetFlags.h
# End Source File
# Begin Source File

SOURCE=..\ScanAPI\Scanobj.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Sysio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Fsavp.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\FSAVP.ini
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=..\Gknt2\Fsgk.lib
# End Source File
# End Target
# End Project
