# Microsoft Developer Studio Project File - Name="licpolicy_no_xml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=licpolicy_no_xml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "licpolicy_no_xml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "licpolicy_no_xml.mak" CFG="licpolicy_no_xml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "licpolicy_no_xml - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "licpolicy_no_xml - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing/build/win32/licpolicy_no_xml", ACPOBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "licpolicy_no_xml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licpolicy___Win32_Debug"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../Out/Debug"
# PROP Intermediate_Dir "../../../../../Temp/Debug/CommonFiles/Licensing/build/win32/licpolicy_no_xml"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__DO_NOT_USE_XML" /FD /GZ /c
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
InputPath=\Out\Debug\licpolicy_no_xml.lib
InputName=licpolicy_no_xml
SOURCE="$(InputPath)"

"..\..\..\..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "licpolicy_no_xml - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licpolicy___Win32_Release"
# PROP BASE Intermediate_Dir "licpolicy___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../Out/Release"
# PROP Intermediate_Dir "../../../../../Temp/Release/CommonFiles/Licensing/build/win32/licpolicy_no_xml"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\..\..\include" /I "..\..\..\..\environment\include\win32" /I "..\..\..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__DO_NOT_USE_XML" /FD /c
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
InputPath=\Out\Release\licpolicy_no_xml.lib
InputName=licpolicy_no_xml
SOURCE="$(InputPath)"

"..\..\..\..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\..\..\Release

# End Custom Build

!ENDIF 

# Begin Target

# Name "licpolicy_no_xml - Win32 Debug"
# Name "licpolicy_no_xml - Win32 Release"
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
