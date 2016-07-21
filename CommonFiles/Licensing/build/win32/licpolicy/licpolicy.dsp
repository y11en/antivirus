# Microsoft Developer Studio Project File - Name="licpolicy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=licpolicy - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "licpolicy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "licpolicy.mak" CFG="licpolicy - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "licpolicy - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing/build/win32/licpolicy", MLBIAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "licpolicy - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licpolicy___Win32_Debug"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/Debug"
# PROP Intermediate_Dir "../../../../../Temp/Debug/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\Debug\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licpolicy___Win32_Release"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/Release"
# PROP Intermediate_Dir "../../../../../Temp/Release/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\Release\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licpolicy___Win32_Unicode_Debug_StaticRTL"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Unicode_Debug_StaticRTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/DebugUS"
# PROP Intermediate_Dir "../../../../../Temp/DebugUS/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\DebugUS\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licpolicy___Win32_Unicode_Release_StaticRTL"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Unicode_Release_StaticRTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../../../../Temp/ReleaseUS/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\ReleaseUS\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licpolicy___Win32_Debug_StaticRTL"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Debug_StaticRTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/DebugS"
# PROP Intermediate_Dir "../../../../../Temp/DebugS/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\DebugS\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licpolicy___Win32_Release_StaticRTL"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Release_StaticRTL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/ReleaseS"
# PROP Intermediate_Dir "../../../../../Temp/ReleaseS/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\ReleaseS\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licpolicy___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/DebugU"
# PROP Intermediate_Dir "../../../../../Temp/DebugU/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\DebugU\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licpolicy___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/ReleaseU"
# PROP Intermediate_Dir "../../../../../Temp/ReleaseU/CommonFiles/Licensing/build/win32/licpolicy"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=\projects\Out\ReleaseU\licpolicy.lib
InputName=licpolicy
SOURCE="$(InputPath)"

"..\..\..\..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\ReleaseUDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "licpolicy - Win32 DebugDll"
# Name "licpolicy - Win32 ReleaseDll"
# Name "licpolicy - Win32 Unicode Debug"
# Name "licpolicy - Win32 Unicode Release"
# Name "licpolicy - Win32 Debug"
# Name "licpolicy - Win32 Release"
# Name "licpolicy - Win32 Unicode DebugDll"
# Name "licpolicy - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\appinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\blacklist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\datatree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\date_t.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\err_defs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\keyfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\licensing_policy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\licensing_policy_c_intf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\licpolicy_msg.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\src\LoadResStr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\novel_io_redir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\secure_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\updateconfig_win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\appinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\appinfo_struct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\blacklist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\datatree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\date_t.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\err_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\getupdateconfig_win32.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\keyfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\lic_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\lic_storage_intf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\licensing_intf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\licensing_policy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\licensing_policy_c_intf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\LoadResStr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\messages.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\msxmlhelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\novell_io_redir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\platform_compat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\secure_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\updateconfig.h
# End Source File
# End Group
# End Target
# End Project
