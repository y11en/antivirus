# Microsoft Developer Studio Project File - Name="librules" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=librules - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "librules_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "librules_kis.mak" CFG="librules - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "librules - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "librules - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Filters/Spamtest", EVPXBAAA"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "librules - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../kis/lib"
# PROP Intermediate_Dir "../../tmp/kis/Release/librules"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../.." /I "../../STL/stlport" /I "../../_include" /I "../../Filtration/API" /I "." /I "../include" /D "KIS_USAGE" /D "FILTERDLL_IMPORT" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "KASP" /D "WITHOUT_PCRE" /D "_EXPAT_EMBED" /FR /Fd"../../tmp/kis/pdb/librules.pdb" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../kis/lib/librules.lib"

!ELSEIF  "$(CFG)" == "librules - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/libd"
# PROP Intermediate_Dir "../../tmp/kis/Debug/librules"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../STL/stlport" /I "../../_include" /I "../../Filtration/API" /I "." /I "../include" /D "KIS_USAGE" /D "FILTERDLL_IMPORT" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "KASP" /D "WITHOUT_PCRE" /D "_EXPAT_EMBED" /FR /Fd"../../tmp/kis/pdbd/librules.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../kis/libd/librules.lib"

!ENDIF 

# Begin Target

# Name "librules - Win32 Release"
# Name "librules - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\actions.cpp
# End Source File
# Begin Source File

SOURCE=.\envelope.cpp
# End Source File
# Begin Source File

SOURCE=.\interpreter.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\MainInterpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\profile.cpp
# End Source File
# Begin Source File

SOURCE=.\profile_db.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\actions.h
# End Source File
# Begin Source File

SOURCE=.\commhdr.h
# End Source File
# Begin Source File

SOURCE=.\envelope.h
# End Source File
# Begin Source File

SOURCE=.\ExpatUtilsExt.h
# End Source File
# Begin Source File

SOURCE=.\interpreter.h
# End Source File
# Begin Source File

SOURCE=.\MainInterpreter.h
# End Source File
# Begin Source File

SOURCE=.\parse.h
# End Source File
# Begin Source File

SOURCE=.\profile.h
# End Source File
# Begin Source File

SOURCE=.\profile_db.h
# End Source File
# Begin Source File

SOURCE=.\ProfileCompile.h
# End Source File
# Begin Source File

SOURCE=.\XmlFilesPool.h
# End Source File
# End Group
# Begin Group "system"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\BaseTsd.h"
# End Source File
# Begin Source File

SOURCE="..\..\gnu\db-1.85\include\cdefs.h"
# End Source File
# Begin Source File

SOURCE=..\..\gnu\libnet\cdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\regex\sys\cdefs.h
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Guiddef.h"
# End Source File
# Begin Source File

SOURCE=..\..\_include\nix_types.h
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\PropIdl.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Reason.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\StrAlign.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Tvout.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\WinEFS.h"
# End Source File
# Begin Source File

SOURCE=..\..\_include\wintypes.h
# End Source File
# End Group
# Begin Group "STL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_algobase.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_algobase.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_alloc.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_auto_ptr.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_bvector.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config_compat.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config_compat_post.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_construct.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_ctraits_fns.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_ctype.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_cwchar.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\_db1.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_deque.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_deque.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\_expat.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_function.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_function_adaptors.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_function_base.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_hash_fun.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_iosfwd.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_iterator.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_iterator_base.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_iterator_old.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_locale.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_map.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_msvc_warnings_off.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_new.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_pair.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_range_errors.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_raw_storage_iter.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_relops_cont.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_site_config.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_stack.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string_fwd.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string_fwd.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string_hash.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string_io.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_string_io.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_tempbuf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_tempbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_threads.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_threads.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_tree.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_uninitialized.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_vector.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\c_locale.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\char_traits.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_confix.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_msvc.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_select_lib.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl_user_config.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stlcomp.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\type_traits.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\vc_select_lib.h
# End Source File
# End Group
# Begin Group "SpamFilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SpamTest\CFilterDll.h
# End Source File
# Begin Source File

SOURCE=..\include\Critsect.h
# End Source File
# Begin Source File

SOURCE=..\SpamTest\CSigDB.h
# End Source File
# Begin Source File

SOURCE=..\PluginOLOE\idStr.h
# End Source File
# Begin Source File

SOURCE=..\include\LogFile.h
# End Source File
# Begin Source File

SOURCE=..\include\Progress.h
# End Source File
# Begin Source File

SOURCE=..\include\progress.rh
# End Source File
# Begin Source File

SOURCE=..\include\SpamFilter.h
# End Source File
# Begin Source File

SOURCE=..\SpamTest\SpamTest.h
# End Source File
# Begin Source File

SOURCE=..\include\SpamTestCFG.h
# End Source File
# End Group
# Begin Group "CFLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Filtration\API\categories.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\API\cfdata.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\patterns\clingproc.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\language.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\letters\letters.h
# End Source File
# Begin Source File

SOURCE=..\..\libmorph\LexID.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\API\message.h
# End Source File
# Begin Source File

SOURCE=..\..\MIMEParser\MIMEParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\libmorph\Morph.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\API\relevant.h
# End Source File
# Begin Source File

SOURCE=.\samples.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\signatures\sigdb.h
# End Source File
# Begin Source File

SOURCE=..\..\Filtration\stopdict\stopdict.h
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\gnu\expat-1.95.2\lib\expat.h"
# End Source File
# Begin Source File

SOURCE=..\..\ZTools\ExpatUtils\ExpatUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\ZTools\ExpatUtils\XMLStruct.h
# End Source File
# Begin Source File

SOURCE=..\..\ZTools\ExpatUtils\XMLStructDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\ZTools\ExpatUtils\XMLTag.h
# End Source File
# End Group
# Begin Group "Profiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\smtp-filter\adc\adc.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\common\config.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\libgsg2\gsg.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\filter-config\xmlfiles\keywords.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\libsigs\libsigs.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\paths\paths.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\contrib\libpatricia\patricia.h"
# End Source File
# Begin Source File

SOURCE=..\..\gnu\regex\regex.h
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\filter-config\xmlfiles\XMLFiles.h"
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\_include\CodePages.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\Cp2Uni.h
# End Source File
# Begin Source File

SOURCE="..\..\gnu\db-1.85\include\db.h"
# End Source File
# Begin Source File

SOURCE=..\..\ZTools\ZUtils\EString.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\md5a\md5a.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\libnet\nameser.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\libnet\nameser_compat.h
# End Source File
# End Group
# Begin Group "smtp-filter-common"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\smtp-filter\common\ip_extractor.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\common\list_db.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\common\logger.h"
# End Source File
# Begin Source File

SOURCE="..\..\smtp-filter\common\merge.h"
# End Source File
# End Group
# End Target
# End Project
