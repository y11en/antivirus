# Microsoft Developer Studio Project File - Name="lzmalib_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lzmalib_vc6 - Win32 Debug STLPort
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lzmalib_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lzmalib_vc6.mak" CFG="lzmalib_vc6 - Win32 Debug STLPort"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lzmalib_vc6 - Win32 Debug STLPort" (based on "Win32 (x86) Static Library")
!MESSAGE "lzmalib_vc6 - Win32 Release STLPort" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lzmalib_vc6 - Win32 Debug STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug STLPort"
# PROP BASE Intermediate_Dir "Debug STLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug STLPort"
# PROP Intermediate_Dir "Debug STLPort"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../out/Debug/lzmalib.lib"
# ADD LIB32 /nologo /out:"../../CS AdminKit/development2/lib/debug/lzmalib.lib"

!ELSEIF  "$(CFG)" == "lzmalib_vc6 - Win32 Release STLPort"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release STLPort"
# PROP BASE Intermediate_Dir "Release STLPort"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release STLPort"
# PROP Intermediate_Dir "Release STLPort"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../out/Debug/lzmalib.lib"
# ADD LIB32 /nologo /out:"../../CS AdminKit/development2/lib/release/lzmalib.lib"

!ENDIF 

# Begin Target

# Name "lzmalib_vc6 - Win32 Debug STLPort"
# Name "lzmalib_vc6 - Win32 Release STLPort"
# Begin Group "lzmasdk"

# PROP Default_Filter ""
# Begin Group "7zip Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\FileStreams.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\FileStreams.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\InBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\InBuffer.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\OutBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\OutBuffer.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\StreamUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Common\StreamUtils.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Alloc.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Alloc.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\C_FileIO.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\C_FileIO.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\CommandLineParser.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\CommandLineParser.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\ComTry.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\CRC.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\CRC.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Defs.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\MyCom.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\MyGuidDef.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\MyInitGuid.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\MyUnknown.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\MyWindows.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\String.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\String.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\StringConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\StringConvert.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\StringToInt.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\StringToInt.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Types.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\Common\Vector.h
# End Source File
# End Group
# Begin Group "Compress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTree.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTree2.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTree3.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTree3Z.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTree4.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\BinTree\BinTreeMain.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\HashChain\HC2.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\HashChain\HC3.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\HashChain\HC4.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\HashChain\HCMain.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\IMatchFinder.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\LZInWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\LZInWindow.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\LZMA.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\LZMADecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\LZMADecoder.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\LZMAEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\LZMAEncoder.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\LZOutWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\LZOutWindow.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\RangeCoder.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\RangeCoderBit.cpp
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\RangeCoderBit.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\RangeCoderBitTree.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\RangeCoderOpt.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZ\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\RangeCoder\StdAfx.h
# End Source File
# End Group
# Begin Group "Decompress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lzmasdk\C\7zip\Compress\LZMA_C\LzmaDecode.c
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\lzma_config.h
# End Source File
# Begin Source File

SOURCE=.\lzma_decode.cpp
# End Source File
# Begin Source File

SOURCE=.\lzma_decode.h
# End Source File
# Begin Source File

SOURCE=.\lzma_encode.cpp
# End Source File
# Begin Source File

SOURCE=.\lzma_encode.h
# End Source File
# End Target
# End Project
