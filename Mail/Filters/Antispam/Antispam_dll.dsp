# Microsoft Developer Studio Project File - Name="Antispam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Antispam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Antispam_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Antispam_dll.mak" CFG="Antispam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Antispam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Antispam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Filters/Antispam", WPISAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Antispam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../out/Release"
# PROP Intermediate_Dir "../../../../temp/Release/ppp/antispam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTISPAM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O1 /I "..\..\..\..\..\\" /I "..\..\include" /I "..\..\..\prague\include" /I "..\..\..\..\..\windows\hook" /I "..\..\..\CommonFiles" /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "html_parser" /I "bayes" /I "encodingtables" /I "..\..\..\..\prague" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTISPAM_EXPORTS" /D "USE_FILE_TYPE_PRAGUE" /D "_USE_XYZ" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i ".\..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x61600000" /dll /debug /machine:I386 /out:"../../../../out/Release/Antispam.ppl" /OPT:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\Antispam.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Antispam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../out/Debug"
# PROP Intermediate_Dir "../../../../temp/Debug/ppp/antispam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTISPAM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\..\prague\include" /I "..\..\..\..\..\..\windows\hook" /I "..\..\..\..\CommonFiles" /I "html_parser" /I "bayes" /I "encodingtables" /I "..\..\..\..\prague" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANTISPAM_EXPORTS" /D "USE_FILE_TYPE_PRAGUE" /D "_USE_XYZ" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i ".\..\..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../out/debug/Antispam.ppl" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Antispam - Win32 Release"
# Name "Antispam - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\KLAntispamFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectToCheck_hIO.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectToCheck_hObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectToCheck_hOS.cpp
# End Source File
# Begin Source File

SOURCE=.\sfdb_table.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\antispam_interface2.h
# End Source File
# Begin Source File

SOURCE=.\antispam_interface_imp2.h
# End Source File
# Begin Source File

SOURCE=.\sfdb_table.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\antispam.rc
# End Source File
# Begin Source File

SOURCE=.\AntispamTask.meta
# End Source File
# End Group
# Begin Group "AntispamTask"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\antispamfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\antispamfilter.h
# End Source File
# Begin Source File

SOURCE=.\AntispamTask.imp
# End Source File
# Begin Source File

SOURCE=.\plugin_antispamtask.cpp
# End Source File
# Begin Source File

SOURCE=.\taskex.cpp
# End Source File
# Begin Source File

SOURCE=.\taskex.h
# End Source File
# End Group
# Begin Group "Html"

# PROP Default_Filter ""
# Begin Group "file_al"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Prague\Diff\file_al.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Prague\Diff\file_al.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Prague\Diff\file_al.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\html_parser\colors.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\html_parser\colortab.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\cssparse.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\html_def.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\html_out.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\html_utl.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlFilter.h
# End Source File
# Begin Source File

SOURCE=.\html_parser\htmparse.h
# End Source File
# Begin Source File

SOURCE=.\html_parser\htmunits.h
# End Source File
# Begin Source File

SOURCE=.\html_parser\htparser.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\util\mballoc.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\util\mballoc.h
# End Source File
# Begin Source File

SOURCE=.\outlook_rtf2html.cpp
# End Source File
# Begin Source File

SOURCE=.\html_parser\util\rcobj.h
# End Source File
# Begin Source File

SOURCE=.\html_parser\validate.cpp
# End Source File
# End Group
# Begin Group "Bayes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BayesFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\BayesFilter.h
# End Source File
# Begin Source File

SOURCE=.\Bayes\bayestable.cpp
# End Source File
# Begin Source File

SOURCE=.\Bayes\bayestable.h
# End Source File
# End Group
# Begin Group "Text"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fill_stop_words.h
# End Source File
# Begin Source File

SOURCE=.\stl_port.cpp
# End Source File
# Begin Source File

SOURCE=.\stl_port.h
# End Source File
# Begin Source File

SOURCE=.\text_normalize.cpp
# End Source File
# Begin Source File

SOURCE=.\text_normalize.h
# End Source File
# Begin Source File

SOURCE=.\TextFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\TextFilter.h
# End Source File
# Begin Source File

SOURCE=.\ToUpper.h
# End Source File
# End Group
# Begin Group "EncodingTables"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EncodingTables\CharsetDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\EncodingTables\CharsetDecoder.h
# End Source File
# Begin Source File

SOURCE=.\EncodingTables\EncodingTables.cpp
# End Source File
# Begin Source File

SOURCE=.\EncodingTables\EncodingTables.h
# End Source File
# End Group
# Begin Group "WhiteList"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\whitelist.cpp
# End Source File
# Begin Source File

SOURCE=.\whitelist.h
# End Source File
# End Group
# Begin Group "ThreadImp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ThreadImp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ThreadImp.h
# End Source File
# End Group
# Begin Group "FilterManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FilterManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterManager.h
# End Source File
# End Group
# Begin Group "RegExp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RegExp\pcre.cpp
# End Source File
# Begin Source File

SOURCE=.\RegExp\pcre.h
# End Source File
# Begin Source File

SOURCE=.\RegExp\pcre_internal.h
# End Source File
# Begin Source File

SOURCE=.\RegExp\reg_expr.cpp
# End Source File
# Begin Source File

SOURCE=.\RegExp\reg_expr.h
# End Source File
# Begin Source File

SOURCE=.\RegExp\smart_ptr.h
# End Source File
# End Group
# End Target
# End Project
