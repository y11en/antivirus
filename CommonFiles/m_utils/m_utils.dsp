# Microsoft Developer Studio Project File - Name="m_utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=m_utils - Win32 DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "m_utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m_utils.mak" CFG="m_utils - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m_utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/m_utils", EBIJAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "m_utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/m_utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob0 /I "../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/m_utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "m_utils___Win32_DebugDll"
# PROP BASE Intermediate_Dir "m_utils___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/m_utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"DebugDll/m_utils.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\debug\m_utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "m_utils___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "m_utils___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/m_utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob0 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob0 /I "../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"ReleaseDll/m_utils.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\ReleaseS\m_utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)"  ..\ReleaseDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "m_utils - Win32 Release"
# Name "m_utils - Win32 Debug"
# Name "m_utils - Win32 DebugDll"
# Name "m_utils - Win32 ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chkesp.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Fo$(IntDir)\$(InputName).obj $(InputPath) 
	ml /c /coff /Fo$(IntDir)\$(InputName).obj $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dbg_rpt.c

!IF  "$(CFG)" == "m_utils - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lldiv.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\llmul.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\llrem.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\m_memcmp.c
# End Source File
# Begin Source File

SOURCE=.\m_memcpy.c
# End Source File
# Begin Source File

SOURCE=.\m_memmov.c
# End Source File
# Begin Source File

SOURCE=.\m_memset.c
# End Source File
# Begin Source File

SOURCE=.\m_strcat.c
# End Source File
# Begin Source File

SOURCE=.\m_strcmp.c
# End Source File
# Begin Source File

SOURCE=.\m_strcpy.c
# End Source File
# Begin Source File

SOURCE=.\m_strlen.c
# End Source File
# Begin Source File

SOURCE=.\m_strncp.c
# End Source File
# Begin Source File

SOURCE=.\m_wcscat.c
# End Source File
# Begin Source File

SOURCE=.\m_wcscmp.c
# End Source File
# Begin Source File

SOURCE=.\m_wcscpy.c
# End Source File
# Begin Source File

SOURCE=.\m_wcslen.c
# End Source File
# Begin Source File

SOURCE=.\ulldiv.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	/Zi /Zd echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ullrem.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	/Zi /Zd echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

# Begin Custom Build - -------------------- ASM Step on $(InputPath) --------------------
IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xtoa.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\m_utils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\m_utils.dep
# End Source File
# Begin Source File

SOURCE=.\m_utils.mak
# End Source File
# End Target
# End Project
