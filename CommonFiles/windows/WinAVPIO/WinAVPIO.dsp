# Microsoft Developer Studio Project File - Name="WinAVPIO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WinAVPIO - Win32 UnicodeOnMbcs DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinAVPIO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinAVPIO.mak" CFG="WinAVPIO - Win32 UnicodeOnMbcs DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinAVPIO - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Windows/WinAVPIO", SCBAAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinAVPIO - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/ReleaseS"
# PROP Intermediate_Dir "../../../Temp/ReleaseS/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/DebugS"
# PROP Intermediate_Dir "../../../Temp/DebugS/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinAVPIO___Win32_DebugDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/Debug"
# PROP Intermediate_Dir "../../../Temp/Debug/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "D:\AVPDevelopment\CommonFiles" /I "D:\AVPDevelopment\Utils" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\DebugS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinAVPIO___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/Release"
# PROP Intermediate_Dir "../../../Temp/Release/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "D:\AVPDevelopment\CommonFiles" /I "D:\AVPDevelopment\Utils" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\ReleaseS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinAVPIO___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/DebugU"
# PROP Intermediate_Dir "../../../Temp/DebugU/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\Debug\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\Out\DebugU\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinAVPIO___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../../Temp/ReleaseUS/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\ReleaseS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\Out\ReleaseUS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinAVPIO___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/DebugUS"
# PROP Intermediate_Dir "../../../Temp/DebugUS/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\DebugS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\Out\DebugUS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinAVPIO___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/ReleaseU"
# PROP Intermediate_Dir "../../../Temp/ReleaseU/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\Release\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\Out\ReleaseU\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinAVPIO___Win32_UnicodeOnMbcs_Debug"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_UnicodeOnMbcs_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/DebugUOM"
# PROP Intermediate_Dir "../../../Temp/DebugUOM/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\DebugS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\DebugUOM\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUOM\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugUOM

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinAVPIO___Win32_UnicodeOnMbcs_Release"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_UnicodeOnMbcs_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/ReleaseUOM"
# PROP Intermediate_Dir "../../../Temp/ReleaseUOM/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\ReleaseS\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\ReleaseUOM\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUOM\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseUOM

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinAVPIO___Win32_UnicodeOnMbcs_DebugDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_UnicodeOnMbcs_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Out/DebugUOMDll"
# PROP Intermediate_Dir "../../../Temp/DebugUOMDll/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\Debug\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\DebugUOMDll\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUOMDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugUOMDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinAVPIO___Win32_UnicodeOnMbcs_ReleaseDll"
# PROP BASE Intermediate_Dir "WinAVPIO___Win32_UnicodeOnMbcs_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Out/ReleaseUOMDll"
# PROP Intermediate_Dir "../../../Temp/ReleaseUOMDll/CommonFiles/windows/WinAVPIO"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\Release\WinAVPIO.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\ReleaseUOMDll\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUOMDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseUOMDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "WinAVPIO - Win32 Release"
# Name "WinAVPIO - Win32 Debug"
# Name "WinAVPIO - Win32 DebugDll"
# Name "WinAVPIO - Win32 ReleaseDll"
# Name "WinAVPIO - Win32 Unicode DebugDll"
# Name "WinAVPIO - Win32 Unicode Release"
# Name "WinAVPIO - Win32 Unicode Debug"
# Name "WinAVPIO - Win32 Unicode ReleaseDll"
# Name "WinAVPIO - Win32 UnicodeOnMbcs Debug"
# Name "WinAVPIO - Win32 UnicodeOnMbcs Release"
# Name "WinAVPIO - Win32 UnicodeOnMbcs DebugDll"
# Name "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IORedirect.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\_avpff.h
# End Source File
# End Group
# End Target
# End Project
