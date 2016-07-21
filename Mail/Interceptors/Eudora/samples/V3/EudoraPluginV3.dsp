# Microsoft Developer Studio Project File - Name="EudoraPluginV3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=EudoraPluginV3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EudoraPluginV3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EudoraPluginV3.mak" CFG="EudoraPluginV3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EudoraPluginV3 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "EudoraPluginV3 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
# PROP WCE_FormatVersion ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EudoraPluginV3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\build\win32\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EUDORAPLUGINV3_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\shared" /I "..\..\..\shared\win32" /I "..\..\..\..\libs2\pfl\common" /I "..\..\..\..\libs2\pfl\win32" /I "..\..\..\..\libs2\pgpsdk\pub\include" /I "..\..\..\..\libs2\pgpsdk\win32\pub\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EUDORAPLUGINV3_EXPORTS" /D PGP_WIN32=1 /D _WIN32_IE=0x0300 /D PGP_DEBUG=0 /D OVERRIDE_BUILDFLAGS=1 /Yu"windows.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d OVERRIDE_BUILDFLAGS=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x31000000" /dll /machine:I386

!ELSEIF  "$(CFG)" == "EudoraPluginV3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\build\win32\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EUDORAPLUGINV3_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\shared" /I "..\..\..\shared\win32" /I "..\..\..\..\libs2\pfl\common" /I "..\..\..\..\libs2\pfl\win32" /I "..\..\..\..\libs2\pgpsdk\pub\include" /I "..\..\..\..\libs2\pgpsdk\win32\pub\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "EUDORAPLUGINV3_EXPORTS" /D PGP_WIN32=1 /D _WIN32_IE=0x0300 /D PGP_DEBUG=1 /D OVERRIDE_BUILDFLAGS=1 /Yu"windows.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d OVERRIDE_BUILDFLAGS=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x31000000" /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "EudoraPluginV3 - Win32 Release"
# Name "EudoraPluginV3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\shared\win32\BlockUtils.c
# End Source File
# Begin Source File

SOURCE=.\CBTProc.c
# End Source File
# Begin Source File

SOURCE=.\CreateToolbar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\DecryptVerify.c
# End Source File
# Begin Source File

SOURCE=.\DisplayMessage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\EncryptSign.c
# End Source File
# Begin Source File

SOURCE=.\EudoraMailHeaders.c
# End Source File
# Begin Source File

SOURCE=.\EudoraMainWndProc.c
# End Source File
# Begin Source File

SOURCE=.\EudoraPluginV3.def
# End Source File
# Begin Source File

SOURCE=.\EudoraPluginV3.rc
# End Source File
# Begin Source File

SOURCE=.\EudoraReadMailWndProc.c
# End Source File
# Begin Source File

SOURCE=.\EudoraSendMailWndProc.c
# End Source File
# Begin Source File

SOURCE=.\MapFile.c
# End Source File
# Begin Source File

SOURCE=.\emssdk\MIMETYPE.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MyMIMEUtils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\ParseMime.c
# End Source File
# Begin Source File

SOURCE=.\PGPPlug.c
# ADD CPP /Yc"windows.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\pgpVersionHeader.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\Prefs.c
# End Source File
# Begin Source File

SOURCE=.\ReadMailToolbarWndProc.c
# End Source File
# Begin Source File

SOURCE=.\emssdk\RFC822.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\RichEdit_IO.c
# End Source File
# Begin Source File

SOURCE=.\SendMailToolbarWndProc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\SigEvent.c
# End Source File
# Begin Source File

SOURCE=.\strstri.c
# End Source File
# Begin Source File

SOURCE=.\transbmp.c
# End Source File
# Begin Source File

SOURCE=.\Translators.c
# End Source File
# Begin Source File

SOURCE=.\TranslatorUtils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\VerificationBlock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\shared\win32\Working.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CBTProc.h
# End Source File
# Begin Source File

SOURCE=.\CreateToolbar.h
# End Source File
# Begin Source File

SOURCE=.\DisplayMessage.h
# End Source File
# Begin Source File

SOURCE=.\EudoraINI.h
# End Source File
# Begin Source File

SOURCE=.\EudoraMailHeaders.h
# End Source File
# Begin Source File

SOURCE=.\EudoraMainWndProc.h
# End Source File
# Begin Source File

SOURCE=.\EudoraReadMailWndProc.h
# End Source File
# Begin Source File

SOURCE=.\EudoraSendMailWndProc.h
# End Source File
# Begin Source File

SOURCE=.\MapFile.h
# End Source File
# Begin Source File

SOURCE=.\MyMIMEUtils.h
# End Source File
# Begin Source File

SOURCE=.\PGPDefinedMessages.h
# End Source File
# Begin Source File

SOURCE=.\PGPPlug.h
# End Source File
# Begin Source File

SOURCE=.\PGPPlugTypes.h
# End Source File
# Begin Source File

SOURCE=.\ReadMailToolbarWndProc.h
# End Source File
# Begin Source File

SOURCE=.\SendMailToolbarWndProc.h
# End Source File
# Begin Source File

SOURCE=.\strstri.h
# End Source File
# Begin Source File

SOURCE=.\transbmp.h
# End Source File
# Begin Source File

SOURCE=.\TranslatorIDs.h
# End Source File
# Begin Source File

SOURCE=.\Translators.h
# End Source File
# Begin Source File

SOURCE=.\TranslatorUtils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Res\lock.ico
# End Source File
# Begin Source File

SOURCE=..\Res\pgp.ico
# End Source File
# Begin Source File

SOURCE=..\Res\readtool.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\sendtool.bmp
# End Source File
# Begin Source File

SOURCE=..\Res\sign.ico
# End Source File
# Begin Source File

SOURCE=..\Res\unlock.ico
# End Source File
# End Group
# End Target
# End Project
