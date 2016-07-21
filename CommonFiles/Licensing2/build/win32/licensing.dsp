# Microsoft Developer Studio Project File - Name="licensing" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=licensing - Win32 Unicode DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "licensing.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "licensing.mak" CFG="licensing - Win32 Unicode DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "licensing - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "licensing - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing2/build/win32", ONKVBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "licensing - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/releases/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/releases/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../out/releases/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debugs/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/debugs/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../out/debugs/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licensing___Win32_DebugDll"
# PROP BASE Intermediate_Dir "licensing___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debug/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/debug/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../out/debug/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licensing___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "licensing___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/release/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/release/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../out/release/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licensing___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "licensing___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/releaseus/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/releaseus/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../out/releases/licensing.lib"
# ADD LIB32 /nologo /out:"../../../../out/releaseus/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "licensing___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "licensing___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/releaseu/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/releaseu/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../out/release/licensing.lib"
# ADD LIB32 /nologo /out:"../../../../out/releaseu/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licensing___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "licensing___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debugus/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/debugus/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../src" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../out/debugs/licensing.lib"
# ADD LIB32 /nologo /out:"../../../../out/debugus/licensing.lib"

!ELSEIF  "$(CFG)" == "licensing - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "licensing___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "licensing___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debugu/commonfiles/licensing2"
# PROP Intermediate_Dir "../../../../temp/debugu/commonfiles/licensing2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "NO_PARTIAL_SPECIALIZATION" /Yu"precompiled.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../out/debug/licensing.lib"
# ADD LIB32 /nologo /out:"../../../../out/debugu/licensing.lib"

!ENDIF 

# Begin Target

# Name "licensing - Win32 Release"
# Name "licensing - Win32 Debug"
# Name "licensing - Win32 DebugDll"
# Name "licensing - Win32 ReleaseDll"
# Name "licensing - Win32 Unicode Release"
# Name "licensing - Win32 Unicode ReleaseDll"
# Name "licensing - Win32 Unicode Debug"
# Name "licensing - Win32 Unicode DebugDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\ControlDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\datatree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\expression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseCondition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseKeyspace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObjectHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObjectWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseRestriction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseRestrictionWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseUtility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Licensing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\utility\LicensingError.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\src\precompiled.cpp
# ADD CPP /Yc"precompiled.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\SignatureVerifier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Verdict.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\utility\AutoPointer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ControlDatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\datatree.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility\Enumerator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\EnumeratorImp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\expression.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicenseContext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicenseKey.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicenseObject.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicenseRestriction.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicenseUtility.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicensing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ILicensingError.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IObjectImage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IStorageR.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IStorageRW.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IVerdict.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseCondition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseContext.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseEnvironment.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseKey.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseKeyPid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseKeyspace.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObject.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObjectHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\LicenseObjectId.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseObjectWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseRestriction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseRestrictionWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LicenseUtility.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Licensing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility\LicensingError.h
# End Source File
# Begin Source File

SOURCE=..\..\include\LicensingInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\include\LicensingTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectImageHolder.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility\pointainer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility\pointerator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\precompiled.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ProductInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SignatureVerifier.h
# End Source File
# Begin Source File

SOURCE=..\..\src\strconv.hpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Verdict.h
# End Source File
# End Group
# End Target
# End Project
