# Microsoft Developer Studio Generated NMAKE File, Based on kav50.dsp
!IF "$(CFG)" == ""
CFG=kav50 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to kav50 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "kav50 - Win32 Release" && "$(CFG)" != "kav50 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kav50.mak" CFG="kav50 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kav50 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kav50 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kav50 - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/kav50
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\avp.exe"

!ELSE 

ALL : "CAB - Win32 Release" "ArcSession - Win32 Release" "connector - Win32 Release" "lha - Win32 Release" "MKavIO - Win32 Release" "tm - Win32 Release" "AntiPhishingTask - Win32 Release" "OEAntiSpam - Win32 Release" "AntiPhishing - Win32 Release" "wingui - Win32 Release" "TrainSupport - Win32 Release" "Socket - Win32 Release" "ProductInfo - Win32 Release" "NTLM - Win32 Release" "NetSession - Win32 Release" "HTTPSession - Win32 Release" "httpscan - Win32 Release" "FTPSession - Win32 Release" "FileSession - Win32 Release" "basegui - Win32 Release" "AntiDialHook - Win32 Release" "AntiDial - Win32 Release" "httpprotocoller - Win32 Release" "PopupChk - Win32 Release" "NNTPProtocoller - Win32 Release" "CryptoHelper - Win32 Release" "winfw - Win32 Release" "IniFile - Win32 Release" "StartUpEnum2 - Win32 Release" "MailWasher - Win32 Release" "IMAPProtocoller - Win32 Release" "GetSystemInfo - Win32 Release" "diff - Win32 Release" "Mail Checker2 - Win32 Release" "SMTPProtocoller - Win32 Release" "POP3Protocoller - Win32 Release" "TrafficMonitor2 - Win32 Release" "TheBatAntispam - Win32 Release" "iCheckerSA - Win32 Release" "TheBatPlugin - Win32 Release" "ComStreamIO - Win32 Release"\
 "contentflt - Win32 Release" "antispy - Win32 Release" "metadata - Win32 Release" "dofw - Win32 Release" "FSSync - Win32 Release" "FSDrvPlgn - Win32 Release" "resip - Win32 Release" "AVP3Info - Win32 Release" "thpoolimp - Win32 Release" "WinDiskIO - Win32 Release" "btimages - Win32 Release" "btdisk - Win32 Release" "Timer - Win32 Release" "xorio - Win32 Release" "WinReg - Win32 Release" "Util - Win32 Release" "UnStored - Win32 Release" "UNSHRINK - Win32 Release" "Unreduce - Win32 Release" "UnLZX - Win32 Release" "UniArc - Win32 Release" "UnArj - Win32 Release" "TempFile - Win32 Release" "superio - Win32 Release" "stream - Win32 Release" "stored - Win32 Release" "StdCompare - Win32 Release" "sfdb - Win32 Release" "seqio - Win32 Release" "rar - Win32 Release" "passdmap - Win32 Release" "NTFSstream - Win32 Release" "nfio - Win32 Release" "MultiDMAP - Win32 Release" "msoe - Win32 Release" "minizip - Win32 Release" "MemScan - Win32 Release" "MemModScan - Win32 Release" "mdb - Win32 Release" "mail_msg - Win32 Release" "L_llio - Win32 Release" "Inflate - Win32 Release" "IChecker2 - Win32 Release" "HCCOMPARE - Win32 Release" "HashMD5 - Win32 Release" "HashContainer - Win32 Release"\
 "Explode - Win32 Release" "dtreg - Win32 Release" "dmap - Win32 Release" "deflate - Win32 Release" "compare - Win32 Release" "buffer - Win32 Release" "avpg - Win32 Release" "avlib - Win32 Release" "ArjPack - Win32 Release" "Arj - Win32 Release" "MAPIEDK - Win32 Release" "AvpMgr - Win32 Release" "Avp1 - Win32 Release" "mcou_antispam - Win32 Release" "ahstm - Win32 Release" "Antispam - Win32 Release" "wmihlpr - Win32 Release" "kavshell - Win32 Release" "Updater2005 - Win32 Release" "ahfw - Win32 Release" "Screensaver - Win32 Release" "ScIEPlugin - Win32 Release" "Licensing60 - Win32 Release" "VBA Interceptor - Win32 Release" "OG Task - Win32 Release" "SC AntiVirus Plugin - Win32 Release" "avspm - Win32 Release" "mcou - Win32 Release" "IWGen - Win32 Release" "ids - Win32 Release" "avpgui - Win32 Release" "klogon - Win32 Release" "PDM - Win32 Release" "pxstub - Win32 Release" "NetDetect - Win32 Release" "ShellEx - Win32 Release" "Script Checker Interceptor - Win32 Release" "Scheduler - Win32 Release" "Loader - Win32 Release" "Script Checker - Win32 Release" "qb - Win32 Release" "Kernel - Win32 Release" "params - Win32 Release" "Report - Win32 Release" "pr_remote - Win32 Release"\
 "ods - Win32 Release" "oas - Win32 Release" "bl - Win32 Release" "avs - Win32 Release" "$(OUTDIR)\avp.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"avs - Win32 ReleaseCLEAN" "bl - Win32 ReleaseCLEAN" "oas - Win32 ReleaseCLEAN" "ods - Win32 ReleaseCLEAN" "pr_remote - Win32 ReleaseCLEAN" "Report - Win32 ReleaseCLEAN" "params - Win32 ReleaseCLEAN" "Kernel - Win32 ReleaseCLEAN" "qb - Win32 ReleaseCLEAN" "Script Checker - Win32 ReleaseCLEAN" "Loader - Win32 ReleaseCLEAN" "Scheduler - Win32 ReleaseCLEAN" "Script Checker Interceptor - Win32 ReleaseCLEAN" "ShellEx - Win32 ReleaseCLEAN" "NetDetect - Win32 ReleaseCLEAN" "pxstub - Win32 ReleaseCLEAN" "PDM - Win32 ReleaseCLEAN" "klogon - Win32 ReleaseCLEAN" "avpgui - Win32 ReleaseCLEAN" "ids - Win32 ReleaseCLEAN" "IWGen - Win32 ReleaseCLEAN" "mcou - Win32 ReleaseCLEAN" "avspm - Win32 ReleaseCLEAN" "SC AntiVirus Plugin - Win32 ReleaseCLEAN" "OG Task - Win32 ReleaseCLEAN" "VBA Interceptor - Win32 ReleaseCLEAN" "Licensing60 - Win32 ReleaseCLEAN" "ScIEPlugin - Win32 ReleaseCLEAN" "Screensaver - Win32 ReleaseCLEAN" "ahfw - Win32 ReleaseCLEAN" "Updater2005 - Win32 ReleaseCLEAN" "kavshell - Win32 ReleaseCLEAN" "wmihlpr - Win32 ReleaseCLEAN" "Antispam - Win32 ReleaseCLEAN" "ahstm - Win32 ReleaseCLEAN" "mcou_antispam - Win32 ReleaseCLEAN" "Avp1 - Win32 ReleaseCLEAN"\
 "AvpMgr - Win32 ReleaseCLEAN" "MAPIEDK - Win32 ReleaseCLEAN" "Arj - Win32 ReleaseCLEAN" "ArjPack - Win32 ReleaseCLEAN" "avlib - Win32 ReleaseCLEAN" "avpg - Win32 ReleaseCLEAN" "buffer - Win32 ReleaseCLEAN" "compare - Win32 ReleaseCLEAN" "deflate - Win32 ReleaseCLEAN" "dmap - Win32 ReleaseCLEAN" "dtreg - Win32 ReleaseCLEAN" "Explode - Win32 ReleaseCLEAN" "HashContainer - Win32 ReleaseCLEAN" "HashMD5 - Win32 ReleaseCLEAN" "HCCOMPARE - Win32 ReleaseCLEAN" "IChecker2 - Win32 ReleaseCLEAN" "Inflate - Win32 ReleaseCLEAN" "L_llio - Win32 ReleaseCLEAN" "mail_msg - Win32 ReleaseCLEAN" "mdb - Win32 ReleaseCLEAN" "MemModScan - Win32 ReleaseCLEAN" "MemScan - Win32 ReleaseCLEAN" "minizip - Win32 ReleaseCLEAN" "msoe - Win32 ReleaseCLEAN" "MultiDMAP - Win32 ReleaseCLEAN" "nfio - Win32 ReleaseCLEAN" "NTFSstream - Win32 ReleaseCLEAN" "passdmap - Win32 ReleaseCLEAN" "rar - Win32 ReleaseCLEAN" "seqio - Win32 ReleaseCLEAN" "sfdb - Win32 ReleaseCLEAN" "StdCompare - Win32 ReleaseCLEAN" "stored - Win32 ReleaseCLEAN" "stream - Win32 ReleaseCLEAN" "superio - Win32 ReleaseCLEAN" "TempFile - Win32 ReleaseCLEAN" "UnArj - Win32 ReleaseCLEAN" "UniArc - Win32 ReleaseCLEAN" "UnLZX - Win32 ReleaseCLEAN"\
 "Unreduce - Win32 ReleaseCLEAN" "UNSHRINK - Win32 ReleaseCLEAN" "UnStored - Win32 ReleaseCLEAN" "Util - Win32 ReleaseCLEAN" "WinReg - Win32 ReleaseCLEAN" "xorio - Win32 ReleaseCLEAN" "Timer - Win32 ReleaseCLEAN" "btdisk - Win32 ReleaseCLEAN" "btimages - Win32 ReleaseCLEAN" "WinDiskIO - Win32 ReleaseCLEAN" "thpoolimp - Win32 ReleaseCLEAN" "AVP3Info - Win32 ReleaseCLEAN" "resip - Win32 ReleaseCLEAN" "FSDrvPlgn - Win32 ReleaseCLEAN" "FSSync - Win32 ReleaseCLEAN" "dofw - Win32 ReleaseCLEAN" "metadata - Win32 ReleaseCLEAN" "antispy - Win32 ReleaseCLEAN" "contentflt - Win32 ReleaseCLEAN" "ComStreamIO - Win32 ReleaseCLEAN" "TheBatPlugin - Win32 ReleaseCLEAN" "iCheckerSA - Win32 ReleaseCLEAN" "TheBatAntispam - Win32 ReleaseCLEAN" "TrafficMonitor2 - Win32 ReleaseCLEAN" "POP3Protocoller - Win32 ReleaseCLEAN" "SMTPProtocoller - Win32 ReleaseCLEAN" "Mail Checker2 - Win32 ReleaseCLEAN" "diff - Win32 ReleaseCLEAN" "GetSystemInfo - Win32 ReleaseCLEAN" "IMAPProtocoller - Win32 ReleaseCLEAN" "MailWasher - Win32 ReleaseCLEAN" "StartUpEnum2 - Win32 ReleaseCLEAN" "IniFile - Win32 ReleaseCLEAN" "winfw - Win32 ReleaseCLEAN" "CryptoHelper - Win32 ReleaseCLEAN" "NNTPProtocoller - Win32 ReleaseCLEAN"\
 "PopupChk - Win32 ReleaseCLEAN" "httpprotocoller - Win32 ReleaseCLEAN" "AntiDial - Win32 ReleaseCLEAN" "AntiDialHook - Win32 ReleaseCLEAN" "basegui - Win32 ReleaseCLEAN" "FileSession - Win32 ReleaseCLEAN" "FTPSession - Win32 ReleaseCLEAN" "httpscan - Win32 ReleaseCLEAN" "HTTPSession - Win32 ReleaseCLEAN" "NetSession - Win32 ReleaseCLEAN" "NTLM - Win32 ReleaseCLEAN" "ProductInfo - Win32 ReleaseCLEAN" "Socket - Win32 ReleaseCLEAN" "TrainSupport - Win32 ReleaseCLEAN" "wingui - Win32 ReleaseCLEAN" "AntiPhishing - Win32 ReleaseCLEAN" "OEAntiSpam - Win32 ReleaseCLEAN" "AntiPhishingTask - Win32 ReleaseCLEAN" "tm - Win32 ReleaseCLEAN" "MKavIO - Win32 ReleaseCLEAN" "lha - Win32 ReleaseCLEAN" "connector - Win32 ReleaseCLEAN" "ArcSession - Win32 ReleaseCLEAN" "CAB - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\exchndl.obj"
	-@erase "$(INTDIR)\kav50.obj"
	-@erase "$(INTDIR)\kav50.res"
	-@erase "$(INTDIR)\NTService.obj"
	-@erase "$(INTDIR)\parsecmd.obj"
	-@erase "$(INTDIR)\SA.OBJ"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avp.exe"
	-@erase "$(OUTDIR)\avp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\kav50.res" /i "../../CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kav50.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Sign.lib pr_remote.lib kernel32.lib fssync.lib sign.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\avp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avp.exe" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref /fixed:no 
LINK32_OBJS= \
	"$(INTDIR)\exchndl.obj" \
	"$(INTDIR)\kav50.obj" \
	"$(INTDIR)\NTService.obj" \
	"$(INTDIR)\parsecmd.obj" \
	"$(INTDIR)\SA.OBJ" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\kav50.res" \
	"$(OUTDIR)\pr_remote.lib" \
	"$(OUTDIR)\ShellEx.lib" \
	"$(OUTDIR)\klogon.lib" \
	"$(OUTDIR)\wmihlpr.lib" \
	"$(OUTDIR)\MAPIEDK.lib" \
	"$(OUTDIR)\stream.lib" \
	"$(OUTDIR)\FSSync.lib" \
	"$(OUTDIR)\dofw.lib" \
	"$(OUTDIR)\metadata.lib" \
	"$(OUTDIR)\kav60.lib" \
	"$(OUTDIR)\GetSystemInfo.lib" \
	"$(OUTDIR)\wingui.lib" \
	"$(OUTDIR)\connector.lib"

"$(OUTDIR)\avp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avp.exe
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "CAB - Win32 Release" "ArcSession - Win32 Release" "connector - Win32 Release" "lha - Win32 Release" "MKavIO - Win32 Release" "tm - Win32 Release" "AntiPhishingTask - Win32 Release" "OEAntiSpam - Win32 Release" "AntiPhishing - Win32 Release" "wingui - Win32 Release" "TrainSupport - Win32 Release" "Socket - Win32 Release" "ProductInfo - Win32 Release" "NTLM - Win32 Release" "NetSession - Win32 Release" "HTTPSession - Win32 Release" "httpscan - Win32 Release" "FTPSession - Win32 Release" "FileSession - Win32 Release" "basegui - Win32 Release" "AntiDialHook - Win32 Release" "AntiDial - Win32 Release" "httpprotocoller - Win32 Release" "PopupChk - Win32 Release" "NNTPProtocoller - Win32 Release" "CryptoHelper - Win32 Release" "winfw - Win32 Release" "IniFile - Win32 Release" "StartUpEnum2 - Win32 Release" "MailWasher - Win32 Release" "IMAPProtocoller - Win32 Release" "GetSystemInfo - Win32 Release" "diff - Win32 Release" "Mail Checker2 - Win32 Release" "SMTPProtocoller - Win32 Release" "POP3Protocoller - Win32 Release" "TrafficMonitor2 - Win32 Release" "TheBatAntispam - Win32 Release" "iCheckerSA - Win32 Release" "TheBatPlugin - Win32 Release"\
 "ComStreamIO - Win32 Release" "contentflt - Win32 Release" "antispy - Win32 Release" "metadata - Win32 Release" "dofw - Win32 Release" "FSSync - Win32 Release" "FSDrvPlgn - Win32 Release" "resip - Win32 Release" "AVP3Info - Win32 Release" "thpoolimp - Win32 Release" "WinDiskIO - Win32 Release" "btimages - Win32 Release" "btdisk - Win32 Release" "Timer - Win32 Release" "xorio - Win32 Release" "WinReg - Win32 Release" "Util - Win32 Release" "UnStored - Win32 Release" "UNSHRINK - Win32 Release" "Unreduce - Win32 Release" "UnLZX - Win32 Release" "UniArc - Win32 Release" "UnArj - Win32 Release" "TempFile - Win32 Release" "superio - Win32 Release" "stream - Win32 Release" "stored - Win32 Release" "StdCompare - Win32 Release" "sfdb - Win32 Release" "seqio - Win32 Release" "rar - Win32 Release" "passdmap - Win32 Release" "NTFSstream - Win32 Release" "nfio - Win32 Release" "MultiDMAP - Win32 Release" "msoe - Win32 Release" "minizip - Win32 Release" "MemScan - Win32 Release" "MemModScan - Win32 Release" "mdb - Win32 Release" "mail_msg - Win32 Release" "L_llio - Win32 Release" "Inflate - Win32 Release" "IChecker2 - Win32 Release" "HCCOMPARE - Win32 Release" "HashMD5 - Win32 Release"\
 "HashContainer - Win32 Release" "Explode - Win32 Release" "dtreg - Win32 Release" "dmap - Win32 Release" "deflate - Win32 Release" "compare - Win32 Release" "buffer - Win32 Release" "avpg - Win32 Release" "avlib - Win32 Release" "ArjPack - Win32 Release" "Arj - Win32 Release" "MAPIEDK - Win32 Release" "AvpMgr - Win32 Release" "Avp1 - Win32 Release" "mcou_antispam - Win32 Release" "ahstm - Win32 Release" "Antispam - Win32 Release" "wmihlpr - Win32 Release" "kavshell - Win32 Release" "Updater2005 - Win32 Release" "ahfw - Win32 Release" "Screensaver - Win32 Release" "ScIEPlugin - Win32 Release" "Licensing60 - Win32 Release" "VBA Interceptor - Win32 Release" "OG Task - Win32 Release" "SC AntiVirus Plugin - Win32 Release" "avspm - Win32 Release" "mcou - Win32 Release" "IWGen - Win32 Release" "ids - Win32 Release" "avpgui - Win32 Release" "klogon - Win32 Release" "PDM - Win32 Release" "pxstub - Win32 Release" "NetDetect - Win32 Release" "ShellEx - Win32 Release" "Script Checker Interceptor - Win32 Release" "Scheduler - Win32 Release" "Loader - Win32 Release" "Script Checker - Win32 Release" "qb - Win32 Release" "Kernel - Win32 Release" "params - Win32 Release" "Report - Win32 Release"\
 "pr_remote - Win32 Release" "ods - Win32 Release" "oas - Win32 Release" "bl - Win32 Release" "avs - Win32 Release" "$(OUTDIR)\avp.exe"
   tsigner "\out\Release\avp.exe"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/kav50
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\avp.exe"

!ELSE 

ALL : "CAB - Win32 Debug" "ArcSession - Win32 Debug" "connector - Win32 Debug" "lha - Win32 Debug" "MKavIO - Win32 Debug" "tm - Win32 Debug" "AntiPhishingTask - Win32 Debug" "OEAntiSpam - Win32 Debug" "AntiPhishing - Win32 Debug" "wingui - Win32 Debug" "TrainSupport - Win32 Debug" "Socket - Win32 Debug" "ProductInfo - Win32 Debug" "NTLM - Win32 Debug" "NetSession - Win32 Debug" "HTTPSession - Win32 Debug" "httpscan - Win32 Debug" "FTPSession - Win32 Debug" "FileSession - Win32 Debug" "basegui - Win32 Debug" "AntiDialHook - Win32 Debug" "AntiDial - Win32 Debug" "httpprotocoller - Win32 Debug" "PopupChk - Win32 Debug" "NNTPProtocoller - Win32 Debug" "CryptoHelper - Win32 Debug" "winfw - Win32 Debug" "IniFile - Win32 Debug" "StartUpEnum2 - Win32 Debug" "MailWasher - Win32 Debug" "IMAPProtocoller - Win32 Debug" "GetSystemInfo - Win32 Debug" "diff - Win32 Debug" "Mail Checker2 - Win32 Debug" "SMTPProtocoller - Win32 Debug" "POP3Protocoller - Win32 Debug" "TrafficMonitor2 - Win32 Debug" "TheBatAntispam - Win32 Debug" "iCheckerSA - Win32 Debug" "TheBatPlugin - Win32 Debug" "ComStreamIO - Win32 Debug" "contentflt - Win32 Debug" "antispy - Win32 Debug" "metadata - Win32 Debug"\
 "dofw - Win32 Debug" "FSSync - Win32 Debug" "FSDrvPlgn - Win32 Debug" "resip - Win32 Debug" "AVP3Info - Win32 Debug" "thpoolimp - Win32 Debug" "WinDiskIO - Win32 Debug" "btimages - Win32 Debug" "btdisk - Win32 Debug" "Timer - Win32 Debug" "xorio - Win32 Debug" "WinReg - Win32 Debug" "Util - Win32 Debug" "UnStored - Win32 Debug" "UNSHRINK - Win32 Debug" "Unreduce - Win32 Debug" "UnLZX - Win32 Debug" "UniArc - Win32 Debug" "UnArj - Win32 Debug" "TempFile - Win32 Debug" "superio - Win32 Debug" "stream - Win32 Debug" "stored - Win32 Debug" "StdCompare - Win32 Debug" "sfdb - Win32 Debug" "seqio - Win32 Debug" "rar - Win32 Debug" "passdmap - Win32 Debug" "NTFSstream - Win32 Debug" "nfio - Win32 Debug" "MultiDMAP - Win32 Debug" "msoe - Win32 Debug" "minizip - Win32 Debug" "MemScan - Win32 Debug" "MemModScan - Win32 Debug" "mdb - Win32 Debug" "mail_msg - Win32 Debug" "L_llio - Win32 Debug" "Inflate - Win32 Debug" "IChecker2 - Win32 Debug" "HCCOMPARE - Win32 Debug" "HashMD5 - Win32 Debug" "HashContainer - Win32 Debug" "Explode - Win32 Debug" "dtreg - Win32 Debug" "dmap - Win32 Debug" "deflate - Win32 Debug" "compare - Win32 Debug" "buffer - Win32 Debug" "avpg - Win32 Debug"\
 "avlib - Win32 Debug" "ArjPack - Win32 Debug" "Arj - Win32 Debug" "MAPIEDK - Win32 Debug" "AvpMgr - Win32 Debug" "Avp1 - Win32 Debug" "mcou_antispam - Win32 Debug" "ahstm - Win32 Debug" "Antispam - Win32 Debug" "wmihlpr - Win32 Debug" "kavshell - Win32 Debug" "Updater2005 - Win32 Debug" "ahfw - Win32 Debug" "Screensaver - Win32 Debug" "ScIEPlugin - Win32 Debug" "Licensing60 - Win32 Debug" "VBA Interceptor - Win32 Debug" "OG Task - Win32 Debug" "SC AntiVirus Plugin - Win32 Debug" "avspm - Win32 Debug" "mcou - Win32 Debug" "IWGen - Win32 Debug" "ids - Win32 Debug" "avpgui - Win32 Debug" "klogon - Win32 Debug" "PDM - Win32 Debug" "pxstub - Win32 Debug" "NetDetect - Win32 Debug" "ShellEx - Win32 Debug" "Script Checker Interceptor - Win32 Debug" "Scheduler - Win32 Debug" "Loader - Win32 Debug" "Script Checker - Win32 Debug" "qb - Win32 Debug" "Kernel - Win32 Debug" "params - Win32 Debug" "Report - Win32 Debug" "pr_remote - Win32 Debug" "ods - Win32 Debug" "oas - Win32 Debug" "bl - Win32 Debug" "avs - Win32 Debug" "$(OUTDIR)\avp.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"avs - Win32 DebugCLEAN" "bl - Win32 DebugCLEAN" "oas - Win32 DebugCLEAN" "ods - Win32 DebugCLEAN" "pr_remote - Win32 DebugCLEAN" "Report - Win32 DebugCLEAN" "params - Win32 DebugCLEAN" "Kernel - Win32 DebugCLEAN" "qb - Win32 DebugCLEAN" "Script Checker - Win32 DebugCLEAN" "Loader - Win32 DebugCLEAN" "Scheduler - Win32 DebugCLEAN" "Script Checker Interceptor - Win32 DebugCLEAN" "ShellEx - Win32 DebugCLEAN" "NetDetect - Win32 DebugCLEAN" "pxstub - Win32 DebugCLEAN" "PDM - Win32 DebugCLEAN" "klogon - Win32 DebugCLEAN" "avpgui - Win32 DebugCLEAN" "ids - Win32 DebugCLEAN" "IWGen - Win32 DebugCLEAN" "mcou - Win32 DebugCLEAN" "avspm - Win32 DebugCLEAN" "SC AntiVirus Plugin - Win32 DebugCLEAN" "OG Task - Win32 DebugCLEAN" "VBA Interceptor - Win32 DebugCLEAN" "Licensing60 - Win32 DebugCLEAN" "ScIEPlugin - Win32 DebugCLEAN" "Screensaver - Win32 DebugCLEAN" "ahfw - Win32 DebugCLEAN" "Updater2005 - Win32 DebugCLEAN" "kavshell - Win32 DebugCLEAN" "wmihlpr - Win32 DebugCLEAN" "Antispam - Win32 DebugCLEAN" "ahstm - Win32 DebugCLEAN" "mcou_antispam - Win32 DebugCLEAN" "Avp1 - Win32 DebugCLEAN" "AvpMgr - Win32 DebugCLEAN" "MAPIEDK - Win32 DebugCLEAN" "Arj - Win32 DebugCLEAN"\
 "ArjPack - Win32 DebugCLEAN" "avlib - Win32 DebugCLEAN" "avpg - Win32 DebugCLEAN" "buffer - Win32 DebugCLEAN" "compare - Win32 DebugCLEAN" "deflate - Win32 DebugCLEAN" "dmap - Win32 DebugCLEAN" "dtreg - Win32 DebugCLEAN" "Explode - Win32 DebugCLEAN" "HashContainer - Win32 DebugCLEAN" "HashMD5 - Win32 DebugCLEAN" "HCCOMPARE - Win32 DebugCLEAN" "IChecker2 - Win32 DebugCLEAN" "Inflate - Win32 DebugCLEAN" "L_llio - Win32 DebugCLEAN" "mail_msg - Win32 DebugCLEAN" "mdb - Win32 DebugCLEAN" "MemModScan - Win32 DebugCLEAN" "MemScan - Win32 DebugCLEAN" "minizip - Win32 DebugCLEAN" "msoe - Win32 DebugCLEAN" "MultiDMAP - Win32 DebugCLEAN" "nfio - Win32 DebugCLEAN" "NTFSstream - Win32 DebugCLEAN" "passdmap - Win32 DebugCLEAN" "rar - Win32 DebugCLEAN" "seqio - Win32 DebugCLEAN" "sfdb - Win32 DebugCLEAN" "StdCompare - Win32 DebugCLEAN" "stored - Win32 DebugCLEAN" "stream - Win32 DebugCLEAN" "superio - Win32 DebugCLEAN" "TempFile - Win32 DebugCLEAN" "UnArj - Win32 DebugCLEAN" "UniArc - Win32 DebugCLEAN" "UnLZX - Win32 DebugCLEAN" "Unreduce - Win32 DebugCLEAN" "UNSHRINK - Win32 DebugCLEAN" "UnStored - Win32 DebugCLEAN" "Util - Win32 DebugCLEAN" "WinReg - Win32 DebugCLEAN"\
 "xorio - Win32 DebugCLEAN" "Timer - Win32 DebugCLEAN" "btdisk - Win32 DebugCLEAN" "btimages - Win32 DebugCLEAN" "WinDiskIO - Win32 DebugCLEAN" "thpoolimp - Win32 DebugCLEAN" "AVP3Info - Win32 DebugCLEAN" "resip - Win32 DebugCLEAN" "FSDrvPlgn - Win32 DebugCLEAN" "FSSync - Win32 DebugCLEAN" "dofw - Win32 DebugCLEAN" "metadata - Win32 DebugCLEAN" "antispy - Win32 DebugCLEAN" "contentflt - Win32 DebugCLEAN" "ComStreamIO - Win32 DebugCLEAN" "TheBatPlugin - Win32 DebugCLEAN" "iCheckerSA - Win32 DebugCLEAN" "TheBatAntispam - Win32 DebugCLEAN" "TrafficMonitor2 - Win32 DebugCLEAN" "POP3Protocoller - Win32 DebugCLEAN" "SMTPProtocoller - Win32 DebugCLEAN" "Mail Checker2 - Win32 DebugCLEAN" "diff - Win32 DebugCLEAN" "GetSystemInfo - Win32 DebugCLEAN" "IMAPProtocoller - Win32 DebugCLEAN" "MailWasher - Win32 DebugCLEAN" "StartUpEnum2 - Win32 DebugCLEAN" "IniFile - Win32 DebugCLEAN" "winfw - Win32 DebugCLEAN" "CryptoHelper - Win32 DebugCLEAN" "NNTPProtocoller - Win32 DebugCLEAN" "PopupChk - Win32 DebugCLEAN" "httpprotocoller - Win32 DebugCLEAN" "AntiDial - Win32 DebugCLEAN" "AntiDialHook - Win32 DebugCLEAN" "basegui - Win32 DebugCLEAN" "FileSession - Win32 DebugCLEAN"\
 "FTPSession - Win32 DebugCLEAN" "httpscan - Win32 DebugCLEAN" "HTTPSession - Win32 DebugCLEAN" "NetSession - Win32 DebugCLEAN" "NTLM - Win32 DebugCLEAN" "ProductInfo - Win32 DebugCLEAN" "Socket - Win32 DebugCLEAN" "TrainSupport - Win32 DebugCLEAN" "wingui - Win32 DebugCLEAN" "AntiPhishing - Win32 DebugCLEAN" "OEAntiSpam - Win32 DebugCLEAN" "AntiPhishingTask - Win32 DebugCLEAN" "tm - Win32 DebugCLEAN" "MKavIO - Win32 DebugCLEAN" "lha - Win32 DebugCLEAN" "connector - Win32 DebugCLEAN" "ArcSession - Win32 DebugCLEAN" "CAB - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\exchndl.obj"
	-@erase "$(INTDIR)\kav50.obj"
	-@erase "$(INTDIR)\kav50.res"
	-@erase "$(INTDIR)\NTService.obj"
	-@erase "$(INTDIR)\parsecmd.obj"
	-@erase "$(INTDIR)\SA.OBJ"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avp.exe"
	-@erase "$(OUTDIR)\avp.ilk"
	-@erase "$(OUTDIR)\avp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\kav50.res" /i "../../CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kav50.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pr_remote.lib kernel32.lib fssync.lib sign.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\avp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avp.exe" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\exchndl.obj" \
	"$(INTDIR)\kav50.obj" \
	"$(INTDIR)\NTService.obj" \
	"$(INTDIR)\parsecmd.obj" \
	"$(INTDIR)\SA.OBJ" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\kav50.res" \
	"$(OUTDIR)\pr_remote.lib" \
	"$(OUTDIR)\ShellEx.lib" \
	"$(OUTDIR)\klogon.lib" \
	"$(OUTDIR)\ScIEPlugin.lib" \
	"$(OUTDIR)\wmihlpr.lib" \
	"$(OUTDIR)\MAPIEDK.lib" \
	"$(OUTDIR)\stream.lib" \
	"$(OUTDIR)\dofw.lib" \
	"$(OUTDIR)\metadata.lib" \
	"$(OUTDIR)\kav60.lib" \
	"$(OUTDIR)\GetSystemInfo.lib" \
	"$(OUTDIR)\wingui.lib" \
	"$(OUTDIR)\connector.lib"

"$(OUTDIR)\avp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("kav50.dep")
!INCLUDE "kav50.dep"
!ELSE 
!MESSAGE Warning: cannot find "kav50.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "kav50 - Win32 Release" || "$(CFG)" == "kav50 - Win32 Debug"
SOURCE=.\exchndl.cpp

"$(INTDIR)\exchndl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kav50.cpp

!IF  "$(CFG)" == "kav50 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kav50.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kav50.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\NTService.cpp

"$(INTDIR)\NTService.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\parsecmd.cpp

"$(INTDIR)\parsecmd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\CommonFiles\Service\SA.CPP

"$(INTDIR)\SA.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\shell.cpp

"$(INTDIR)\shell.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kav50.rc

"$(INTDIR)\kav50.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "kav50 - Win32 Release"

"avs - Win32 Release" : 
   cd "\PPP\AVS"
   $(MAKE) /$(MAKEFLAGS) /F .\avs.mak CFG="avs - Win32 Release" 
   cd "..\KAV50"

"avs - Win32 ReleaseCLEAN" : 
   cd "\PPP\AVS"
   $(MAKE) /$(MAKEFLAGS) /F .\avs.mak CFG="avs - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"avs - Win32 Debug" : 
   cd "\PPP\AVS"
   $(MAKE) /$(MAKEFLAGS) /F .\avs.mak CFG="avs - Win32 Debug" 
   cd "..\KAV50"

"avs - Win32 DebugCLEAN" : 
   cd "\PPP\AVS"
   $(MAKE) /$(MAKEFLAGS) /F .\avs.mak CFG="avs - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"bl - Win32 Release" : 
   cd "\PPP\bl"
   $(MAKE) /$(MAKEFLAGS) /F .\bl.mak CFG="bl - Win32 Release" 
   cd "..\KAV50"

"bl - Win32 ReleaseCLEAN" : 
   cd "\PPP\bl"
   $(MAKE) /$(MAKEFLAGS) /F .\bl.mak CFG="bl - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"bl - Win32 Debug" : 
   cd "\PPP\bl"
   $(MAKE) /$(MAKEFLAGS) /F .\bl.mak CFG="bl - Win32 Debug" 
   cd "..\KAV50"

"bl - Win32 DebugCLEAN" : 
   cd "\PPP\bl"
   $(MAKE) /$(MAKEFLAGS) /F .\bl.mak CFG="bl - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"oas - Win32 Release" : 
   cd "\PPP\OAS"
   $(MAKE) /$(MAKEFLAGS) /F .\oas.mak CFG="oas - Win32 Release" 
   cd "..\KAV50"

"oas - Win32 ReleaseCLEAN" : 
   cd "\PPP\OAS"
   $(MAKE) /$(MAKEFLAGS) /F .\oas.mak CFG="oas - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"oas - Win32 Debug" : 
   cd "\PPP\OAS"
   $(MAKE) /$(MAKEFLAGS) /F .\oas.mak CFG="oas - Win32 Debug" 
   cd "..\KAV50"

"oas - Win32 DebugCLEAN" : 
   cd "\PPP\OAS"
   $(MAKE) /$(MAKEFLAGS) /F .\oas.mak CFG="oas - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ods - Win32 Release" : 
   cd "\PPP\ODS"
   $(MAKE) /$(MAKEFLAGS) /F .\ods.mak CFG="ods - Win32 Release" 
   cd "..\KAV50"

"ods - Win32 ReleaseCLEAN" : 
   cd "\PPP\ODS"
   $(MAKE) /$(MAKEFLAGS) /F .\ods.mak CFG="ods - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ods - Win32 Debug" : 
   cd "\PPP\ODS"
   $(MAKE) /$(MAKEFLAGS) /F .\ods.mak CFG="ods - Win32 Debug" 
   cd "..\KAV50"

"ods - Win32 DebugCLEAN" : 
   cd "\PPP\ODS"
   $(MAKE) /$(MAKEFLAGS) /F .\ods.mak CFG="ods - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"pr_remote - Win32 Release" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" 
   cd "..\..\PPP\KAV50"

"pr_remote - Win32 ReleaseCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"pr_remote - Win32 Debug" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"pr_remote - Win32 DebugCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Report - Win32 Release" : 
   cd "\PPP\REPORT"
   $(MAKE) /$(MAKEFLAGS) /F .\report.mak CFG="Report - Win32 Release" 
   cd "..\KAV50"

"Report - Win32 ReleaseCLEAN" : 
   cd "\PPP\REPORT"
   $(MAKE) /$(MAKEFLAGS) /F .\report.mak CFG="Report - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Report - Win32 Debug" : 
   cd "\PPP\REPORT"
   $(MAKE) /$(MAKEFLAGS) /F .\report.mak CFG="Report - Win32 Debug" 
   cd "..\KAV50"

"Report - Win32 DebugCLEAN" : 
   cd "\PPP\REPORT"
   $(MAKE) /$(MAKEFLAGS) /F .\report.mak CFG="Report - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"params - Win32 Release" : 
   cd "\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Release" 
   cd "..\KAV50"

"params - Win32 ReleaseCLEAN" : 
   cd "\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"params - Win32 Debug" : 
   cd "\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Debug" 
   cd "..\KAV50"

"params - Win32 DebugCLEAN" : 
   cd "\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Kernel - Win32 Release" : 
   cd "\prague\kernel"
   $(MAKE) /$(MAKEFLAGS) /F .\Kernel.mak CFG="Kernel - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Kernel - Win32 ReleaseCLEAN" : 
   cd "\prague\kernel"
   $(MAKE) /$(MAKEFLAGS) /F .\Kernel.mak CFG="Kernel - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Kernel - Win32 Debug" : 
   cd "\prague\kernel"
   $(MAKE) /$(MAKEFLAGS) /F .\Kernel.mak CFG="Kernel - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Kernel - Win32 DebugCLEAN" : 
   cd "\prague\kernel"
   $(MAKE) /$(MAKEFLAGS) /F .\Kernel.mak CFG="Kernel - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"qb - Win32 Release" : 
   cd "\PPP\QB"
   $(MAKE) /$(MAKEFLAGS) /F .\qb.mak CFG="qb - Win32 Release" 
   cd "..\KAV50"

"qb - Win32 ReleaseCLEAN" : 
   cd "\PPP\QB"
   $(MAKE) /$(MAKEFLAGS) /F .\qb.mak CFG="qb - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"qb - Win32 Debug" : 
   cd "\PPP\QB"
   $(MAKE) /$(MAKEFLAGS) /F .\qb.mak CFG="qb - Win32 Debug" 
   cd "..\KAV50"

"qb - Win32 DebugCLEAN" : 
   cd "\PPP\QB"
   $(MAKE) /$(MAKEFLAGS) /F .\qb.mak CFG="qb - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Script Checker - Win32 Release" : 
   cd "\PPP\sc"
   $(MAKE) /$(MAKEFLAGS) /F .\sc.mak CFG="Script Checker - Win32 Release" 
   cd "..\KAV50"

"Script Checker - Win32 ReleaseCLEAN" : 
   cd "\PPP\sc"
   $(MAKE) /$(MAKEFLAGS) /F .\sc.mak CFG="Script Checker - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Script Checker - Win32 Debug" : 
   cd "\PPP\sc"
   $(MAKE) /$(MAKEFLAGS) /F .\sc.mak CFG="Script Checker - Win32 Debug" 
   cd "..\KAV50"

"Script Checker - Win32 DebugCLEAN" : 
   cd "\PPP\sc"
   $(MAKE) /$(MAKEFLAGS) /F .\sc.mak CFG="Script Checker - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Loader - Win32 Release" : 
   cd "\prague\loader"
   $(MAKE) /$(MAKEFLAGS) /F .\loader.mak CFG="Loader - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Loader - Win32 ReleaseCLEAN" : 
   cd "\prague\loader"
   $(MAKE) /$(MAKEFLAGS) /F .\loader.mak CFG="Loader - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Loader - Win32 Debug" : 
   cd "\prague\loader"
   $(MAKE) /$(MAKEFLAGS) /F .\loader.mak CFG="Loader - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Loader - Win32 DebugCLEAN" : 
   cd "\prague\loader"
   $(MAKE) /$(MAKEFLAGS) /F .\loader.mak CFG="Loader - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Scheduler - Win32 Release" : 
   cd "\PPP\Scheduler"
   $(MAKE) /$(MAKEFLAGS) /F .\schedule.mak CFG="Scheduler - Win32 Release" 
   cd "..\KAV50"

"Scheduler - Win32 ReleaseCLEAN" : 
   cd "\PPP\Scheduler"
   $(MAKE) /$(MAKEFLAGS) /F .\schedule.mak CFG="Scheduler - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Scheduler - Win32 Debug" : 
   cd "\PPP\Scheduler"
   $(MAKE) /$(MAKEFLAGS) /F .\schedule.mak CFG="Scheduler - Win32 Debug" 
   cd "..\KAV50"

"Scheduler - Win32 DebugCLEAN" : 
   cd "\PPP\Scheduler"
   $(MAKE) /$(MAKEFLAGS) /F .\schedule.mak CFG="Scheduler - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Script Checker Interceptor - Win32 Release" : 
   cd "\PPP\SC\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Release" 
   cd "..\..\KAV50"

"Script Checker Interceptor - Win32 ReleaseCLEAN" : 
   cd "\PPP\SC\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Script Checker Interceptor - Win32 Debug" : 
   cd "\PPP\SC\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Debug" 
   cd "..\..\KAV50"

"Script Checker Interceptor - Win32 DebugCLEAN" : 
   cd "\PPP\SC\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ShellEx - Win32 Release" : 
   cd "\PPP\ShellEx"
   $(MAKE) /$(MAKEFLAGS) /F .\ShellEx.mak CFG="ShellEx - Win32 Release" 
   cd "..\KAV50"

"ShellEx - Win32 ReleaseCLEAN" : 
   cd "\PPP\ShellEx"
   $(MAKE) /$(MAKEFLAGS) /F .\ShellEx.mak CFG="ShellEx - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ShellEx - Win32 Debug" : 
   cd "\PPP\ShellEx"
   $(MAKE) /$(MAKEFLAGS) /F .\ShellEx.mak CFG="ShellEx - Win32 Debug" 
   cd "..\KAV50"

"ShellEx - Win32 DebugCLEAN" : 
   cd "\PPP\ShellEx"
   $(MAKE) /$(MAKEFLAGS) /F .\ShellEx.mak CFG="ShellEx - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"NetDetect - Win32 Release" : 
   cd "\PPP\NetDetect"
   $(MAKE) /$(MAKEFLAGS) /F .\ndetect.mak CFG="NetDetect - Win32 Release" 
   cd "..\KAV50"

"NetDetect - Win32 ReleaseCLEAN" : 
   cd "\PPP\NetDetect"
   $(MAKE) /$(MAKEFLAGS) /F .\ndetect.mak CFG="NetDetect - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"NetDetect - Win32 Debug" : 
   cd "\PPP\NetDetect"
   $(MAKE) /$(MAKEFLAGS) /F .\ndetect.mak CFG="NetDetect - Win32 Debug" 
   cd "..\KAV50"

"NetDetect - Win32 DebugCLEAN" : 
   cd "\PPP\NetDetect"
   $(MAKE) /$(MAKEFLAGS) /F .\ndetect.mak CFG="NetDetect - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"pxstub - Win32 Release" : 
   cd "\PPP\rpc"
   $(MAKE) /$(MAKEFLAGS) /F .\rpc.mak CFG="pxstub - Win32 Release" 
   cd "..\KAV50"

"pxstub - Win32 ReleaseCLEAN" : 
   cd "\PPP\rpc"
   $(MAKE) /$(MAKEFLAGS) /F .\rpc.mak CFG="pxstub - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"pxstub - Win32 Debug" : 
   cd "\PPP\rpc"
   $(MAKE) /$(MAKEFLAGS) /F .\rpc.mak CFG="pxstub - Win32 Debug" 
   cd "..\KAV50"

"pxstub - Win32 DebugCLEAN" : 
   cd "\PPP\rpc"
   $(MAKE) /$(MAKEFLAGS) /F .\rpc.mak CFG="pxstub - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"PDM - Win32 Release" : 
   cd "\PPP\PDM"
   $(MAKE) /$(MAKEFLAGS) /F .\pdm.mak CFG="PDM - Win32 Release" 
   cd "..\KAV50"

"PDM - Win32 ReleaseCLEAN" : 
   cd "\PPP\PDM"
   $(MAKE) /$(MAKEFLAGS) /F .\pdm.mak CFG="PDM - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"PDM - Win32 Debug" : 
   cd "\PPP\PDM"
   $(MAKE) /$(MAKEFLAGS) /F .\pdm.mak CFG="PDM - Win32 Debug" 
   cd "..\KAV50"

"PDM - Win32 DebugCLEAN" : 
   cd "\PPP\PDM"
   $(MAKE) /$(MAKEFLAGS) /F .\pdm.mak CFG="PDM - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"klogon - Win32 Release" : 
   cd "\PPP\KLogon"
   $(MAKE) /$(MAKEFLAGS) /F .\klogon.mak CFG="klogon - Win32 Release" 
   cd "..\KAV50"

"klogon - Win32 ReleaseCLEAN" : 
   cd "\PPP\KLogon"
   $(MAKE) /$(MAKEFLAGS) /F .\klogon.mak CFG="klogon - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"klogon - Win32 Debug" : 
   cd "\PPP\KLogon"
   $(MAKE) /$(MAKEFLAGS) /F .\klogon.mak CFG="klogon - Win32 Debug" 
   cd "..\KAV50"

"klogon - Win32 DebugCLEAN" : 
   cd "\PPP\KLogon"
   $(MAKE) /$(MAKEFLAGS) /F .\klogon.mak CFG="klogon - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"avpgui - Win32 Release" : 
   cd "\PPP\GUI60"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgui.mak CFG="avpgui - Win32 Release" 
   cd "..\KAV50"

"avpgui - Win32 ReleaseCLEAN" : 
   cd "\PPP\GUI60"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgui.mak CFG="avpgui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"avpgui - Win32 Debug" : 
   cd "\PPP\GUI60"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgui.mak CFG="avpgui - Win32 Debug" 
   cd "..\KAV50"

"avpgui - Win32 DebugCLEAN" : 
   cd "\PPP\GUI60"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgui.mak CFG="avpgui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ids - Win32 Release" : 
   cd "\PPP\IDS"
   $(MAKE) /$(MAKEFLAGS) /F .\ids.mak CFG="ids - Win32 Release" 
   cd "..\KAV50"

"ids - Win32 ReleaseCLEAN" : 
   cd "\PPP\IDS"
   $(MAKE) /$(MAKEFLAGS) /F .\ids.mak CFG="ids - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ids - Win32 Debug" : 
   cd "\PPP\IDS"
   $(MAKE) /$(MAKEFLAGS) /F .\ids.mak CFG="ids - Win32 Debug" 
   cd "..\KAV50"

"ids - Win32 DebugCLEAN" : 
   cd "\PPP\IDS"
   $(MAKE) /$(MAKEFLAGS) /F .\ids.mak CFG="ids - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"IWGen - Win32 Release" : 
   cd "\prague\Converters\IWGen"
   $(MAKE) /$(MAKEFLAGS) /F .\iwgen.mak CFG="IWGen - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"IWGen - Win32 ReleaseCLEAN" : 
   cd "\prague\Converters\IWGen"
   $(MAKE) /$(MAKEFLAGS) /F .\iwgen.mak CFG="IWGen - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"IWGen - Win32 Debug" : 
   cd "\prague\Converters\IWGen"
   $(MAKE) /$(MAKEFLAGS) /F .\iwgen.mak CFG="IWGen - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"IWGen - Win32 DebugCLEAN" : 
   cd "\prague\Converters\IWGen"
   $(MAKE) /$(MAKEFLAGS) /F .\iwgen.mak CFG="IWGen - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"mcou - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou.mak CFG="mcou - Win32 Release" 
   cd "..\..\..\..\KAV50"

"mcou - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou.mak CFG="mcou - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"mcou - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou.mak CFG="mcou - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"mcou - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou.mak CFG="mcou - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"avspm - Win32 Release" : 
   cd "\PPP\AVS\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Release" 
   cd "..\..\KAV50"

"avspm - Win32 ReleaseCLEAN" : 
   cd "\PPP\AVS\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"avspm - Win32 Debug" : 
   cd "\PPP\AVS\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Debug" 
   cd "..\..\KAV50"

"avspm - Win32 DebugCLEAN" : 
   cd "\PPP\AVS\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"SC AntiVirus Plugin - Win32 Release" : 
   cd "\PPP\SC\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Release" 
   cd "..\..\KAV50"

"SC AntiVirus Plugin - Win32 ReleaseCLEAN" : 
   cd "\PPP\SC\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"SC AntiVirus Plugin - Win32 Debug" : 
   cd "\PPP\SC\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Debug" 
   cd "..\..\KAV50"

"SC AntiVirus Plugin - Win32 DebugCLEAN" : 
   cd "\PPP\SC\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"OG Task - Win32 Release" : 
   cd "\PPP\OG"
   $(MAKE) /$(MAKEFLAGS) /F .\og.mak CFG="OG Task - Win32 Release" 
   cd "..\KAV50"

"OG Task - Win32 ReleaseCLEAN" : 
   cd "\PPP\OG"
   $(MAKE) /$(MAKEFLAGS) /F .\og.mak CFG="OG Task - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"OG Task - Win32 Debug" : 
   cd "\PPP\OG"
   $(MAKE) /$(MAKEFLAGS) /F .\og.mak CFG="OG Task - Win32 Debug" 
   cd "..\KAV50"

"OG Task - Win32 DebugCLEAN" : 
   cd "\PPP\OG"
   $(MAKE) /$(MAKEFLAGS) /F .\og.mak CFG="OG Task - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"VBA Interceptor - Win32 Release" : 
   cd "\PPP\OG\VBA_Interceptor"
   $(MAKE) /$(MAKEFLAGS) /F .\OffGuard.mak CFG="VBA Interceptor - Win32 Release" 
   cd "..\..\KAV50"

"VBA Interceptor - Win32 ReleaseCLEAN" : 
   cd "\PPP\OG\VBA_Interceptor"
   $(MAKE) /$(MAKEFLAGS) /F .\OffGuard.mak CFG="VBA Interceptor - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"VBA Interceptor - Win32 Debug" : 
   cd "\PPP\OG\VBA_Interceptor"
   $(MAKE) /$(MAKEFLAGS) /F .\OffGuard.mak CFG="VBA Interceptor - Win32 Debug" 
   cd "..\..\KAV50"

"VBA Interceptor - Win32 DebugCLEAN" : 
   cd "\PPP\OG\VBA_Interceptor"
   $(MAKE) /$(MAKEFLAGS) /F .\OffGuard.mak CFG="VBA Interceptor - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Licensing60 - Win32 Release" : 
   cd "\PPP\Licensing60"
   $(MAKE) /$(MAKEFLAGS) /F .\lic60.mak CFG="Licensing60 - Win32 Release" 
   cd "..\KAV50"

"Licensing60 - Win32 ReleaseCLEAN" : 
   cd "\PPP\Licensing60"
   $(MAKE) /$(MAKEFLAGS) /F .\lic60.mak CFG="Licensing60 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Licensing60 - Win32 Debug" : 
   cd "\PPP\Licensing60"
   $(MAKE) /$(MAKEFLAGS) /F .\lic60.mak CFG="Licensing60 - Win32 Debug" 
   cd "..\KAV50"

"Licensing60 - Win32 DebugCLEAN" : 
   cd "\PPP\Licensing60"
   $(MAKE) /$(MAKEFLAGS) /F .\lic60.mak CFG="Licensing60 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ScIEPlugin - Win32 Release" : 
   cd "\PPP\ScIEPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\ScIEPlugin.mak CFG="ScIEPlugin - Win32 Release" 
   cd "..\KAV50"

"ScIEPlugin - Win32 ReleaseCLEAN" : 
   cd "\PPP\ScIEPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\ScIEPlugin.mak CFG="ScIEPlugin - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ScIEPlugin - Win32 Debug" : 
   cd "\PPP\ScIEPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\ScIEPlugin.mak CFG="ScIEPlugin - Win32 Debug" 
   cd "..\KAV50"

"ScIEPlugin - Win32 DebugCLEAN" : 
   cd "\PPP\ScIEPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\ScIEPlugin.mak CFG="ScIEPlugin - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Screensaver - Win32 Release" : 
   cd "\PPP\Screensaver"
   $(MAKE) /$(MAKEFLAGS) /F .\Screensaver.mak CFG="Screensaver - Win32 Release" 
   cd "..\KAV50"

"Screensaver - Win32 ReleaseCLEAN" : 
   cd "\PPP\Screensaver"
   $(MAKE) /$(MAKEFLAGS) /F .\Screensaver.mak CFG="Screensaver - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Screensaver - Win32 Debug" : 
   cd "\PPP\Screensaver"
   $(MAKE) /$(MAKEFLAGS) /F .\Screensaver.mak CFG="Screensaver - Win32 Debug" 
   cd "..\KAV50"

"Screensaver - Win32 DebugCLEAN" : 
   cd "\PPP\Screensaver"
   $(MAKE) /$(MAKEFLAGS) /F .\Screensaver.mak CFG="Screensaver - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ahfw - Win32 Release" : 
   cd "\PPP\AHFW"
   $(MAKE) /$(MAKEFLAGS) /F .\ahfw.mak CFG="ahfw - Win32 Release" 
   cd "..\KAV50"

"ahfw - Win32 ReleaseCLEAN" : 
   cd "\PPP\AHFW"
   $(MAKE) /$(MAKEFLAGS) /F .\ahfw.mak CFG="ahfw - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ahfw - Win32 Debug" : 
   cd "\PPP\AHFW"
   $(MAKE) /$(MAKEFLAGS) /F .\ahfw.mak CFG="ahfw - Win32 Debug" 
   cd "..\KAV50"

"ahfw - Win32 DebugCLEAN" : 
   cd "\PPP\AHFW"
   $(MAKE) /$(MAKEFLAGS) /F .\ahfw.mak CFG="ahfw - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Updater2005 - Win32 Release" : 
   cd "\PPP\Updater2005"
   $(MAKE) /$(MAKEFLAGS) /F .\updater2005.mak CFG="Updater2005 - Win32 Release" 
   cd "..\KAV50"

"Updater2005 - Win32 ReleaseCLEAN" : 
   cd "\PPP\Updater2005"
   $(MAKE) /$(MAKEFLAGS) /F .\updater2005.mak CFG="Updater2005 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Updater2005 - Win32 Debug" : 
   cd "\PPP\Updater2005"
   $(MAKE) /$(MAKEFLAGS) /F .\updater2005.mak CFG="Updater2005 - Win32 Debug" 
   cd "..\KAV50"

"Updater2005 - Win32 DebugCLEAN" : 
   cd "\PPP\Updater2005"
   $(MAKE) /$(MAKEFLAGS) /F .\updater2005.mak CFG="Updater2005 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"kavshell - Win32 Release" : 
   cd "\PPP\KavShell"
   $(MAKE) /$(MAKEFLAGS) /F .\kavshell.mak CFG="kavshell - Win32 Release" 
   cd "..\KAV50"

"kavshell - Win32 ReleaseCLEAN" : 
   cd "\PPP\KavShell"
   $(MAKE) /$(MAKEFLAGS) /F .\kavshell.mak CFG="kavshell - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"kavshell - Win32 Debug" : 
   cd "\PPP\KavShell"
   $(MAKE) /$(MAKEFLAGS) /F .\kavshell.mak CFG="kavshell - Win32 Debug" 
   cd "..\KAV50"

"kavshell - Win32 DebugCLEAN" : 
   cd "\PPP\KavShell"
   $(MAKE) /$(MAKEFLAGS) /F .\kavshell.mak CFG="kavshell - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"wmihlpr - Win32 Release" : 
   cd "\PPP\wmihlpr"
   $(MAKE) /$(MAKEFLAGS) /F .\wmihlpr.mak CFG="wmihlpr - Win32 Release" 
   cd "..\KAV50"

"wmihlpr - Win32 ReleaseCLEAN" : 
   cd "\PPP\wmihlpr"
   $(MAKE) /$(MAKEFLAGS) /F .\wmihlpr.mak CFG="wmihlpr - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"wmihlpr - Win32 Debug" : 
   cd "\PPP\wmihlpr"
   $(MAKE) /$(MAKEFLAGS) /F .\wmihlpr.mak CFG="wmihlpr - Win32 Debug" 
   cd "..\KAV50"

"wmihlpr - Win32 DebugCLEAN" : 
   cd "\PPP\wmihlpr"
   $(MAKE) /$(MAKEFLAGS) /F .\wmihlpr.mak CFG="wmihlpr - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Antispam - Win32 Release" : 
   cd "\PPP\MailCommon\Filters\Antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\Antispam_dll.mak CFG="Antispam - Win32 Release" 
   cd "..\..\..\KAV50"

"Antispam - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Filters\Antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\Antispam_dll.mak CFG="Antispam - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Antispam - Win32 Debug" : 
   cd "\PPP\MailCommon\Filters\Antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\Antispam_dll.mak CFG="Antispam - Win32 Debug" 
   cd "..\..\..\KAV50"

"Antispam - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Filters\Antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\Antispam_dll.mak CFG="Antispam - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ahstm - Win32 Release" : 
   cd "\PPP\ahstm"
   $(MAKE) /$(MAKEFLAGS) /F .\ahstm.mak CFG="ahstm - Win32 Release" 
   cd "..\KAV50"

"ahstm - Win32 ReleaseCLEAN" : 
   cd "\PPP\ahstm"
   $(MAKE) /$(MAKEFLAGS) /F .\ahstm.mak CFG="ahstm - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ahstm - Win32 Debug" : 
   cd "\PPP\ahstm"
   $(MAKE) /$(MAKEFLAGS) /F .\ahstm.mak CFG="ahstm - Win32 Debug" 
   cd "..\KAV50"

"ahstm - Win32 DebugCLEAN" : 
   cd "\PPP\ahstm"
   $(MAKE) /$(MAKEFLAGS) /F .\ahstm.mak CFG="ahstm - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"mcou_antispam - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou_antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou_antispam.mak CFG="mcou_antispam - Win32 Release" 
   cd "..\..\..\..\KAV50"

"mcou_antispam - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou_antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou_antispam.mak CFG="mcou_antispam - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"mcou_antispam - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou_antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou_antispam.mak CFG="mcou_antispam - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"mcou_antispam - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\mcou_antispam"
   $(MAKE) /$(MAKEFLAGS) /F .\mcou_antispam.mak CFG="mcou_antispam - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Avp1 - Win32 Release" : 
   cd "\prague\avp1"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp1.mak CFG="Avp1 - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Avp1 - Win32 ReleaseCLEAN" : 
   cd "\prague\avp1"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp1.mak CFG="Avp1 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Avp1 - Win32 Debug" : 
   cd "\prague\avp1"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp1.mak CFG="Avp1 - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Avp1 - Win32 DebugCLEAN" : 
   cd "\prague\avp1"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp1.mak CFG="Avp1 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AvpMgr - Win32 Release" : 
   cd "\prague\avpmgr"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpMgr.mak CFG="AvpMgr - Win32 Release" 
   cd "..\..\PPP\KAV50"

"AvpMgr - Win32 ReleaseCLEAN" : 
   cd "\prague\avpmgr"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpMgr.mak CFG="AvpMgr - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AvpMgr - Win32 Debug" : 
   cd "\prague\avpmgr"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpMgr.mak CFG="AvpMgr - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"AvpMgr - Win32 DebugCLEAN" : 
   cd "\prague\avpmgr"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpMgr.mak CFG="AvpMgr - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MAPIEDK - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\MAPIEDK"
   $(MAKE) /$(MAKEFLAGS) /F .\MAPIEDK.mak CFG="MAPIEDK - Win32 Release" 
   cd "..\..\..\..\KAV50"

"MAPIEDK - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\MAPIEDK"
   $(MAKE) /$(MAKEFLAGS) /F .\MAPIEDK.mak CFG="MAPIEDK - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MAPIEDK - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\MAPIEDK"
   $(MAKE) /$(MAKEFLAGS) /F .\MAPIEDK.mak CFG="MAPIEDK - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"MAPIEDK - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookPlugin\MAPIEDK"
   $(MAKE) /$(MAKEFLAGS) /F .\MAPIEDK.mak CFG="MAPIEDK - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Arj - Win32 Release" : 
   cd "\prague\MiniArchiver\arj"
   $(MAKE) /$(MAKEFLAGS) /F .\Arj.mak CFG="Arj - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"Arj - Win32 ReleaseCLEAN" : 
   cd "\prague\MiniArchiver\arj"
   $(MAKE) /$(MAKEFLAGS) /F .\Arj.mak CFG="Arj - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Arj - Win32 Debug" : 
   cd "\prague\MiniArchiver\arj"
   $(MAKE) /$(MAKEFLAGS) /F .\Arj.mak CFG="Arj - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"Arj - Win32 DebugCLEAN" : 
   cd "\prague\MiniArchiver\arj"
   $(MAKE) /$(MAKEFLAGS) /F .\Arj.mak CFG="Arj - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ArjPack - Win32 Release" : 
   cd "\prague\Packers\arjpack"
   $(MAKE) /$(MAKEFLAGS) /F .\ArjPack.mak CFG="ArjPack - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"ArjPack - Win32 ReleaseCLEAN" : 
   cd "\prague\Packers\arjpack"
   $(MAKE) /$(MAKEFLAGS) /F .\ArjPack.mak CFG="ArjPack - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ArjPack - Win32 Debug" : 
   cd "\prague\Packers\arjpack"
   $(MAKE) /$(MAKEFLAGS) /F .\ArjPack.mak CFG="ArjPack - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"ArjPack - Win32 DebugCLEAN" : 
   cd "\prague\Packers\arjpack"
   $(MAKE) /$(MAKEFLAGS) /F .\ArjPack.mak CFG="ArjPack - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"avlib - Win32 Release" : 
   cd "\prague\avlib"
   $(MAKE) /$(MAKEFLAGS) /F .\avlib.mak CFG="avlib - Win32 Release" 
   cd "..\..\PPP\KAV50"

"avlib - Win32 ReleaseCLEAN" : 
   cd "\prague\avlib"
   $(MAKE) /$(MAKEFLAGS) /F .\avlib.mak CFG="avlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"avlib - Win32 Debug" : 
   cd "\prague\avlib"
   $(MAKE) /$(MAKEFLAGS) /F .\avlib.mak CFG="avlib - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"avlib - Win32 DebugCLEAN" : 
   cd "\prague\avlib"
   $(MAKE) /$(MAKEFLAGS) /F .\avlib.mak CFG="avlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"avpg - Win32 Release" : 
   cd "\prague\avpgs"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgs.mak CFG="avpg - Win32 Release" 
   cd "..\..\PPP\KAV50"

"avpg - Win32 ReleaseCLEAN" : 
   cd "\prague\avpgs"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgs.mak CFG="avpg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"avpg - Win32 Debug" : 
   cd "\prague\avpgs"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgs.mak CFG="avpg - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"avpg - Win32 DebugCLEAN" : 
   cd "\prague\avpgs"
   $(MAKE) /$(MAKEFLAGS) /F .\avpgs.mak CFG="avpg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"buffer - Win32 Release" : 
   cd "\prague\buffer"
   $(MAKE) /$(MAKEFLAGS) /F .\buffer.mak CFG="buffer - Win32 Release" 
   cd "..\..\PPP\KAV50"

"buffer - Win32 ReleaseCLEAN" : 
   cd "\prague\buffer"
   $(MAKE) /$(MAKEFLAGS) /F .\buffer.mak CFG="buffer - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"buffer - Win32 Debug" : 
   cd "\prague\buffer"
   $(MAKE) /$(MAKEFLAGS) /F .\buffer.mak CFG="buffer - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"buffer - Win32 DebugCLEAN" : 
   cd "\prague\buffer"
   $(MAKE) /$(MAKEFLAGS) /F .\buffer.mak CFG="buffer - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"compare - Win32 Release" : 
   cd "\prague\zip\compare"
   $(MAKE) /$(MAKEFLAGS) /F .\compare.mak CFG="compare - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"compare - Win32 ReleaseCLEAN" : 
   cd "\prague\zip\compare"
   $(MAKE) /$(MAKEFLAGS) /F .\compare.mak CFG="compare - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"compare - Win32 Debug" : 
   cd "\prague\zip\compare"
   $(MAKE) /$(MAKEFLAGS) /F .\compare.mak CFG="compare - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"compare - Win32 DebugCLEAN" : 
   cd "\prague\zip\compare"
   $(MAKE) /$(MAKEFLAGS) /F .\compare.mak CFG="compare - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"deflate - Win32 Release" : 
   cd "\prague\Packers\deflate"
   $(MAKE) /$(MAKEFLAGS) /F .\deflate.mak CFG="deflate - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"deflate - Win32 ReleaseCLEAN" : 
   cd "\prague\Packers\deflate"
   $(MAKE) /$(MAKEFLAGS) /F .\deflate.mak CFG="deflate - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"deflate - Win32 Debug" : 
   cd "\prague\Packers\deflate"
   $(MAKE) /$(MAKEFLAGS) /F .\deflate.mak CFG="deflate - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"deflate - Win32 DebugCLEAN" : 
   cd "\prague\Packers\deflate"
   $(MAKE) /$(MAKEFLAGS) /F .\deflate.mak CFG="deflate - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"dmap - Win32 Release" : 
   cd "\prague\Wrappers\dmap"
   $(MAKE) /$(MAKEFLAGS) /F .\dmap.mak CFG="dmap - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"dmap - Win32 ReleaseCLEAN" : 
   cd "\prague\Wrappers\dmap"
   $(MAKE) /$(MAKEFLAGS) /F .\dmap.mak CFG="dmap - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"dmap - Win32 Debug" : 
   cd "\prague\Wrappers\dmap"
   $(MAKE) /$(MAKEFLAGS) /F .\dmap.mak CFG="dmap - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"dmap - Win32 DebugCLEAN" : 
   cd "\prague\Wrappers\dmap"
   $(MAKE) /$(MAKEFLAGS) /F .\dmap.mak CFG="dmap - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"dtreg - Win32 Release" : 
   cd "\prague\dtreg"
   $(MAKE) /$(MAKEFLAGS) /F .\dtreg.mak CFG="dtreg - Win32 Release" 
   cd "..\..\PPP\KAV50"

"dtreg - Win32 ReleaseCLEAN" : 
   cd "\prague\dtreg"
   $(MAKE) /$(MAKEFLAGS) /F .\dtreg.mak CFG="dtreg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"dtreg - Win32 Debug" : 
   cd "\prague\dtreg"
   $(MAKE) /$(MAKEFLAGS) /F .\dtreg.mak CFG="dtreg - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"dtreg - Win32 DebugCLEAN" : 
   cd "\prague\dtreg"
   $(MAKE) /$(MAKEFLAGS) /F .\dtreg.mak CFG="dtreg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Explode - Win32 Release" : 
   cd "\prague\Extract\explode"
   $(MAKE) /$(MAKEFLAGS) /F .\Explode.mak CFG="Explode - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"Explode - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\explode"
   $(MAKE) /$(MAKEFLAGS) /F .\Explode.mak CFG="Explode - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Explode - Win32 Debug" : 
   cd "\prague\Extract\explode"
   $(MAKE) /$(MAKEFLAGS) /F .\Explode.mak CFG="Explode - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"Explode - Win32 DebugCLEAN" : 
   cd "\prague\Extract\explode"
   $(MAKE) /$(MAKEFLAGS) /F .\Explode.mak CFG="Explode - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"HashContainer - Win32 Release" : 
   cd "\prague\hashcontainer"
   $(MAKE) /$(MAKEFLAGS) /F .\HashContainer.mak CFG="HashContainer - Win32 Release" 
   cd "..\..\PPP\KAV50"

"HashContainer - Win32 ReleaseCLEAN" : 
   cd "\prague\hashcontainer"
   $(MAKE) /$(MAKEFLAGS) /F .\HashContainer.mak CFG="HashContainer - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"HashContainer - Win32 Debug" : 
   cd "\prague\hashcontainer"
   $(MAKE) /$(MAKEFLAGS) /F .\HashContainer.mak CFG="HashContainer - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"HashContainer - Win32 DebugCLEAN" : 
   cd "\prague\hashcontainer"
   $(MAKE) /$(MAKEFLAGS) /F .\HashContainer.mak CFG="HashContainer - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"HashMD5 - Win32 Release" : 
   cd "\prague\Hash\md5"
   $(MAKE) /$(MAKEFLAGS) /F .\HashMD5.mak CFG="HashMD5 - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"HashMD5 - Win32 ReleaseCLEAN" : 
   cd "\prague\Hash\md5"
   $(MAKE) /$(MAKEFLAGS) /F .\HashMD5.mak CFG="HashMD5 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"HashMD5 - Win32 Debug" : 
   cd "\prague\Hash\md5"
   $(MAKE) /$(MAKEFLAGS) /F .\HashMD5.mak CFG="HashMD5 - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"HashMD5 - Win32 DebugCLEAN" : 
   cd "\prague\Hash\md5"
   $(MAKE) /$(MAKEFLAGS) /F .\HashMD5.mak CFG="HashMD5 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"HCCOMPARE - Win32 Release" : 
   cd "\prague\hashcontainer\hccompare"
   $(MAKE) /$(MAKEFLAGS) /F .\HCCOMPARE.mak CFG="HCCOMPARE - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"HCCOMPARE - Win32 ReleaseCLEAN" : 
   cd "\prague\hashcontainer\hccompare"
   $(MAKE) /$(MAKEFLAGS) /F .\HCCOMPARE.mak CFG="HCCOMPARE - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"HCCOMPARE - Win32 Debug" : 
   cd "\prague\hashcontainer\hccompare"
   $(MAKE) /$(MAKEFLAGS) /F .\HCCOMPARE.mak CFG="HCCOMPARE - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"HCCOMPARE - Win32 DebugCLEAN" : 
   cd "\prague\hashcontainer\hccompare"
   $(MAKE) /$(MAKEFLAGS) /F .\HCCOMPARE.mak CFG="HCCOMPARE - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"IChecker2 - Win32 Release" : 
   cd "\prague\ISLite\ichecker2"
   $(MAKE) /$(MAKEFLAGS) /F .\ichecker2.mak CFG="IChecker2 - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"IChecker2 - Win32 ReleaseCLEAN" : 
   cd "\prague\ISLite\ichecker2"
   $(MAKE) /$(MAKEFLAGS) /F .\ichecker2.mak CFG="IChecker2 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"IChecker2 - Win32 Debug" : 
   cd "\prague\ISLite\ichecker2"
   $(MAKE) /$(MAKEFLAGS) /F .\ichecker2.mak CFG="IChecker2 - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"IChecker2 - Win32 DebugCLEAN" : 
   cd "\prague\ISLite\ichecker2"
   $(MAKE) /$(MAKEFLAGS) /F .\ichecker2.mak CFG="IChecker2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Inflate - Win32 Release" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"Inflate - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Inflate - Win32 Debug" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"Inflate - Win32 DebugCLEAN" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"L_llio - Win32 Release" : 
   cd "\prague\l_llio"
   $(MAKE) /$(MAKEFLAGS) /F .\l_llio.mak CFG="L_llio - Win32 Release" 
   cd "..\..\PPP\KAV50"

"L_llio - Win32 ReleaseCLEAN" : 
   cd "\prague\l_llio"
   $(MAKE) /$(MAKEFLAGS) /F .\l_llio.mak CFG="L_llio - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"L_llio - Win32 Debug" : 
   cd "\prague\l_llio"
   $(MAKE) /$(MAKEFLAGS) /F .\l_llio.mak CFG="L_llio - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"L_llio - Win32 DebugCLEAN" : 
   cd "\prague\l_llio"
   $(MAKE) /$(MAKEFLAGS) /F .\l_llio.mak CFG="L_llio - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"mail_msg - Win32 Release" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mail_msg.mak CFG="mail_msg - Win32 Release" 
   cd "..\..\PPP\KAV50"

"mail_msg - Win32 ReleaseCLEAN" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mail_msg.mak CFG="mail_msg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"mail_msg - Win32 Debug" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mail_msg.mak CFG="mail_msg - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"mail_msg - Win32 DebugCLEAN" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mail_msg.mak CFG="mail_msg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"mdb - Win32 Release" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mdb.mak CFG="mdb - Win32 Release" 
   cd "..\..\PPP\KAV50"

"mdb - Win32 ReleaseCLEAN" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mdb.mak CFG="mdb - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"mdb - Win32 Debug" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mdb.mak CFG="mdb - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"mdb - Win32 DebugCLEAN" : 
   cd "\prague\outlook"
   $(MAKE) /$(MAKEFLAGS) /F .\mdb.mak CFG="mdb - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MemModScan - Win32 Release" : 
   cd "\prague\memmodscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemModScan.mak CFG="MemModScan - Win32 Release" 
   cd "..\..\PPP\KAV50"

"MemModScan - Win32 ReleaseCLEAN" : 
   cd "\prague\memmodscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemModScan.mak CFG="MemModScan - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MemModScan - Win32 Debug" : 
   cd "\prague\memmodscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemModScan.mak CFG="MemModScan - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"MemModScan - Win32 DebugCLEAN" : 
   cd "\prague\memmodscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemModScan.mak CFG="MemModScan - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MemScan - Win32 Release" : 
   cd "\prague\memscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemScan.mak CFG="MemScan - Win32 Release" 
   cd "..\..\PPP\KAV50"

"MemScan - Win32 ReleaseCLEAN" : 
   cd "\prague\memscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemScan.mak CFG="MemScan - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MemScan - Win32 Debug" : 
   cd "\prague\memscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemScan.mak CFG="MemScan - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"MemScan - Win32 DebugCLEAN" : 
   cd "\prague\memscan"
   $(MAKE) /$(MAKEFLAGS) /F .\MemScan.mak CFG="MemScan - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"minizip - Win32 Release" : 
   cd "\prague\MiniArchiver\Zip"
   $(MAKE) /$(MAKEFLAGS) /F .\minizip.mak CFG="minizip - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"minizip - Win32 ReleaseCLEAN" : 
   cd "\prague\MiniArchiver\Zip"
   $(MAKE) /$(MAKEFLAGS) /F .\minizip.mak CFG="minizip - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"minizip - Win32 Debug" : 
   cd "\prague\MiniArchiver\Zip"
   $(MAKE) /$(MAKEFLAGS) /F .\minizip.mak CFG="minizip - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"minizip - Win32 DebugCLEAN" : 
   cd "\prague\MiniArchiver\Zip"
   $(MAKE) /$(MAKEFLAGS) /F .\minizip.mak CFG="minizip - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"msoe - Win32 Release" : 
   cd "\prague\MSOE"
   $(MAKE) /$(MAKEFLAGS) /F .\msoe.mak CFG="msoe - Win32 Release" 
   cd "..\..\PPP\KAV50"

"msoe - Win32 ReleaseCLEAN" : 
   cd "\prague\MSOE"
   $(MAKE) /$(MAKEFLAGS) /F .\msoe.mak CFG="msoe - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"msoe - Win32 Debug" : 
   cd "\prague\MSOE"
   $(MAKE) /$(MAKEFLAGS) /F .\msoe.mak CFG="msoe - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"msoe - Win32 DebugCLEAN" : 
   cd "\prague\MSOE"
   $(MAKE) /$(MAKEFLAGS) /F .\msoe.mak CFG="msoe - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MultiDMAP - Win32 Release" : 
   cd "\prague\Wrappers\multidmap"
   $(MAKE) /$(MAKEFLAGS) /F .\MultiDMAP.mak CFG="MultiDMAP - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"MultiDMAP - Win32 ReleaseCLEAN" : 
   cd "\prague\Wrappers\multidmap"
   $(MAKE) /$(MAKEFLAGS) /F .\MultiDMAP.mak CFG="MultiDMAP - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MultiDMAP - Win32 Debug" : 
   cd "\prague\Wrappers\multidmap"
   $(MAKE) /$(MAKEFLAGS) /F .\MultiDMAP.mak CFG="MultiDMAP - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"MultiDMAP - Win32 DebugCLEAN" : 
   cd "\prague\Wrappers\multidmap"
   $(MAKE) /$(MAKEFLAGS) /F .\MultiDMAP.mak CFG="MultiDMAP - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"nfio - Win32 Release" : 
   cd "\prague\nfio"
   $(MAKE) /$(MAKEFLAGS) /F .\nfio.mak CFG="nfio - Win32 Release" 
   cd "..\..\PPP\KAV50"

"nfio - Win32 ReleaseCLEAN" : 
   cd "\prague\nfio"
   $(MAKE) /$(MAKEFLAGS) /F .\nfio.mak CFG="nfio - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"nfio - Win32 Debug" : 
   cd "\prague\nfio"
   $(MAKE) /$(MAKEFLAGS) /F .\nfio.mak CFG="nfio - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"nfio - Win32 DebugCLEAN" : 
   cd "\prague\nfio"
   $(MAKE) /$(MAKEFLAGS) /F .\nfio.mak CFG="nfio - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"NTFSstream - Win32 Release" : 
   cd "\prague\ntfsstream"
   $(MAKE) /$(MAKEFLAGS) /F .\NTFSstream.mak CFG="NTFSstream - Win32 Release" 
   cd "..\..\PPP\KAV50"

"NTFSstream - Win32 ReleaseCLEAN" : 
   cd "\prague\ntfsstream"
   $(MAKE) /$(MAKEFLAGS) /F .\NTFSstream.mak CFG="NTFSstream - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"NTFSstream - Win32 Debug" : 
   cd "\prague\ntfsstream"
   $(MAKE) /$(MAKEFLAGS) /F .\NTFSstream.mak CFG="NTFSstream - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"NTFSstream - Win32 DebugCLEAN" : 
   cd "\prague\ntfsstream"
   $(MAKE) /$(MAKEFLAGS) /F .\NTFSstream.mak CFG="NTFSstream - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"passdmap - Win32 Release" : 
   cd "\prague\Wrappers\passdmap"
   $(MAKE) /$(MAKEFLAGS) /F .\passdmap.mak CFG="passdmap - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"passdmap - Win32 ReleaseCLEAN" : 
   cd "\prague\Wrappers\passdmap"
   $(MAKE) /$(MAKEFLAGS) /F .\passdmap.mak CFG="passdmap - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"passdmap - Win32 Debug" : 
   cd "\prague\Wrappers\passdmap"
   $(MAKE) /$(MAKEFLAGS) /F .\passdmap.mak CFG="passdmap - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"passdmap - Win32 DebugCLEAN" : 
   cd "\prague\Wrappers\passdmap"
   $(MAKE) /$(MAKEFLAGS) /F .\passdmap.mak CFG="passdmap - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"rar - Win32 Release" : 
   cd "\prague\MiniArchiver\rar"
   $(MAKE) /$(MAKEFLAGS) /F .\rar.mak CFG="rar - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"rar - Win32 ReleaseCLEAN" : 
   cd "\prague\MiniArchiver\rar"
   $(MAKE) /$(MAKEFLAGS) /F .\rar.mak CFG="rar - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"rar - Win32 Debug" : 
   cd "\prague\MiniArchiver\rar"
   $(MAKE) /$(MAKEFLAGS) /F .\rar.mak CFG="rar - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"rar - Win32 DebugCLEAN" : 
   cd "\prague\MiniArchiver\rar"
   $(MAKE) /$(MAKEFLAGS) /F .\rar.mak CFG="rar - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"seqio - Win32 Release" : 
   cd "\prague\seqio"
   $(MAKE) /$(MAKEFLAGS) /F .\seqio.mak CFG="seqio - Win32 Release" 
   cd "..\..\PPP\KAV50"

"seqio - Win32 ReleaseCLEAN" : 
   cd "\prague\seqio"
   $(MAKE) /$(MAKEFLAGS) /F .\seqio.mak CFG="seqio - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"seqio - Win32 Debug" : 
   cd "\prague\seqio"
   $(MAKE) /$(MAKEFLAGS) /F .\seqio.mak CFG="seqio - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"seqio - Win32 DebugCLEAN" : 
   cd "\prague\seqio"
   $(MAKE) /$(MAKEFLAGS) /F .\seqio.mak CFG="seqio - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"sfdb - Win32 Release" : 
   cd "\prague\ISLite\sfdb"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"sfdb - Win32 ReleaseCLEAN" : 
   cd "\prague\ISLite\sfdb"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"sfdb - Win32 Debug" : 
   cd "\prague\ISLite\sfdb"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"sfdb - Win32 DebugCLEAN" : 
   cd "\prague\ISLite\sfdb"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"StdCompare - Win32 Release" : 
   cd "\prague\StdCompare"
   $(MAKE) /$(MAKEFLAGS) /F .\StdCompare.mak CFG="StdCompare - Win32 Release" 
   cd "..\..\PPP\KAV50"

"StdCompare - Win32 ReleaseCLEAN" : 
   cd "\prague\StdCompare"
   $(MAKE) /$(MAKEFLAGS) /F .\StdCompare.mak CFG="StdCompare - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"StdCompare - Win32 Debug" : 
   cd "\prague\StdCompare"
   $(MAKE) /$(MAKEFLAGS) /F .\StdCompare.mak CFG="StdCompare - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"StdCompare - Win32 DebugCLEAN" : 
   cd "\prague\StdCompare"
   $(MAKE) /$(MAKEFLAGS) /F .\StdCompare.mak CFG="StdCompare - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"stored - Win32 Release" : 
   cd "\prague\Packers\stored"
   $(MAKE) /$(MAKEFLAGS) /F .\stored.mak CFG="stored - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"stored - Win32 ReleaseCLEAN" : 
   cd "\prague\Packers\stored"
   $(MAKE) /$(MAKEFLAGS) /F .\stored.mak CFG="stored - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"stored - Win32 Debug" : 
   cd "\prague\Packers\stored"
   $(MAKE) /$(MAKEFLAGS) /F .\stored.mak CFG="stored - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"stored - Win32 DebugCLEAN" : 
   cd "\prague\Packers\stored"
   $(MAKE) /$(MAKEFLAGS) /F .\stored.mak CFG="stored - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"stream - Win32 Release" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" 
   cd "..\..\PPP\KAV50"

"stream - Win32 ReleaseCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"stream - Win32 Debug" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"stream - Win32 DebugCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"superio - Win32 Release" : 
   cd "\prague\superio"
   $(MAKE) /$(MAKEFLAGS) /F .\superio.mak CFG="superio - Win32 Release" 
   cd "..\..\PPP\KAV50"

"superio - Win32 ReleaseCLEAN" : 
   cd "\prague\superio"
   $(MAKE) /$(MAKEFLAGS) /F .\superio.mak CFG="superio - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"superio - Win32 Debug" : 
   cd "\prague\superio"
   $(MAKE) /$(MAKEFLAGS) /F .\superio.mak CFG="superio - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"superio - Win32 DebugCLEAN" : 
   cd "\prague\superio"
   $(MAKE) /$(MAKEFLAGS) /F .\superio.mak CFG="superio - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"TempFile - Win32 Release" : 
   cd "\prague\tempfile"
   $(MAKE) /$(MAKEFLAGS) /F .\TempFile.mak CFG="TempFile - Win32 Release" 
   cd "..\..\PPP\KAV50"

"TempFile - Win32 ReleaseCLEAN" : 
   cd "\prague\tempfile"
   $(MAKE) /$(MAKEFLAGS) /F .\TempFile.mak CFG="TempFile - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"TempFile - Win32 Debug" : 
   cd "\prague\tempfile"
   $(MAKE) /$(MAKEFLAGS) /F .\TempFile.mak CFG="TempFile - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"TempFile - Win32 DebugCLEAN" : 
   cd "\prague\tempfile"
   $(MAKE) /$(MAKEFLAGS) /F .\TempFile.mak CFG="TempFile - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"UnArj - Win32 Release" : 
   cd "\prague\Extract\unarj"
   $(MAKE) /$(MAKEFLAGS) /F .\UnArj.mak CFG="UnArj - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"UnArj - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\unarj"
   $(MAKE) /$(MAKEFLAGS) /F .\UnArj.mak CFG="UnArj - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"UnArj - Win32 Debug" : 
   cd "\prague\Extract\unarj"
   $(MAKE) /$(MAKEFLAGS) /F .\UnArj.mak CFG="UnArj - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"UnArj - Win32 DebugCLEAN" : 
   cd "\prague\Extract\unarj"
   $(MAKE) /$(MAKEFLAGS) /F .\UnArj.mak CFG="UnArj - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"UniArc - Win32 Release" : 
   cd "\prague\uniarc"
   $(MAKE) /$(MAKEFLAGS) /F .\UniArc.mak CFG="UniArc - Win32 Release" 
   cd "..\..\PPP\KAV50"

"UniArc - Win32 ReleaseCLEAN" : 
   cd "\prague\uniarc"
   $(MAKE) /$(MAKEFLAGS) /F .\UniArc.mak CFG="UniArc - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"UniArc - Win32 Debug" : 
   cd "\prague\uniarc"
   $(MAKE) /$(MAKEFLAGS) /F .\UniArc.mak CFG="UniArc - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"UniArc - Win32 DebugCLEAN" : 
   cd "\prague\uniarc"
   $(MAKE) /$(MAKEFLAGS) /F .\UniArc.mak CFG="UniArc - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"UnLZX - Win32 Release" : 
   cd "\prague\Extract\unlzx"
   $(MAKE) /$(MAKEFLAGS) /F .\UnLZX.mak CFG="UnLZX - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"UnLZX - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\unlzx"
   $(MAKE) /$(MAKEFLAGS) /F .\UnLZX.mak CFG="UnLZX - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"UnLZX - Win32 Debug" : 
   cd "\prague\Extract\unlzx"
   $(MAKE) /$(MAKEFLAGS) /F .\UnLZX.mak CFG="UnLZX - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"UnLZX - Win32 DebugCLEAN" : 
   cd "\prague\Extract\unlzx"
   $(MAKE) /$(MAKEFLAGS) /F .\UnLZX.mak CFG="UnLZX - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Unreduce - Win32 Release" : 
   cd "\prague\Extract\unreduce"
   $(MAKE) /$(MAKEFLAGS) /F .\Unreduce.mak CFG="Unreduce - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"Unreduce - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\unreduce"
   $(MAKE) /$(MAKEFLAGS) /F .\Unreduce.mak CFG="Unreduce - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Unreduce - Win32 Debug" : 
   cd "\prague\Extract\unreduce"
   $(MAKE) /$(MAKEFLAGS) /F .\Unreduce.mak CFG="Unreduce - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"Unreduce - Win32 DebugCLEAN" : 
   cd "\prague\Extract\unreduce"
   $(MAKE) /$(MAKEFLAGS) /F .\Unreduce.mak CFG="Unreduce - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"UNSHRINK - Win32 Release" : 
   cd "\prague\Extract\unshrink"
   $(MAKE) /$(MAKEFLAGS) /F .\UNSHRINK.mak CFG="UNSHRINK - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"UNSHRINK - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\unshrink"
   $(MAKE) /$(MAKEFLAGS) /F .\UNSHRINK.mak CFG="UNSHRINK - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"UNSHRINK - Win32 Debug" : 
   cd "\prague\Extract\unshrink"
   $(MAKE) /$(MAKEFLAGS) /F .\UNSHRINK.mak CFG="UNSHRINK - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"UNSHRINK - Win32 DebugCLEAN" : 
   cd "\prague\Extract\unshrink"
   $(MAKE) /$(MAKEFLAGS) /F .\UNSHRINK.mak CFG="UNSHRINK - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"UnStored - Win32 Release" : 
   cd "\prague\Extract\unstored"
   $(MAKE) /$(MAKEFLAGS) /F .\UnStored.mak CFG="UnStored - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"UnStored - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\unstored"
   $(MAKE) /$(MAKEFLAGS) /F .\UnStored.mak CFG="UnStored - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"UnStored - Win32 Debug" : 
   cd "\prague\Extract\unstored"
   $(MAKE) /$(MAKEFLAGS) /F .\UnStored.mak CFG="UnStored - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"UnStored - Win32 DebugCLEAN" : 
   cd "\prague\Extract\unstored"
   $(MAKE) /$(MAKEFLAGS) /F .\UnStored.mak CFG="UnStored - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Util - Win32 Release" : 
   cd "\prague\util"
   $(MAKE) /$(MAKEFLAGS) /F .\Util.mak CFG="Util - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Util - Win32 ReleaseCLEAN" : 
   cd "\prague\util"
   $(MAKE) /$(MAKEFLAGS) /F .\Util.mak CFG="Util - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Util - Win32 Debug" : 
   cd "\prague\util"
   $(MAKE) /$(MAKEFLAGS) /F .\Util.mak CFG="Util - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Util - Win32 DebugCLEAN" : 
   cd "\prague\util"
   $(MAKE) /$(MAKEFLAGS) /F .\Util.mak CFG="Util - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"WinReg - Win32 Release" : 
   cd "\prague\winreg"
   $(MAKE) /$(MAKEFLAGS) /F .\WinReg.mak CFG="WinReg - Win32 Release" 
   cd "..\..\PPP\KAV50"

"WinReg - Win32 ReleaseCLEAN" : 
   cd "\prague\winreg"
   $(MAKE) /$(MAKEFLAGS) /F .\WinReg.mak CFG="WinReg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"WinReg - Win32 Debug" : 
   cd "\prague\winreg"
   $(MAKE) /$(MAKEFLAGS) /F .\WinReg.mak CFG="WinReg - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"WinReg - Win32 DebugCLEAN" : 
   cd "\prague\winreg"
   $(MAKE) /$(MAKEFLAGS) /F .\WinReg.mak CFG="WinReg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"xorio - Win32 Release" : 
   cd "\prague\Wrappers\xorio"
   $(MAKE) /$(MAKEFLAGS) /F .\xorio.mak CFG="xorio - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"xorio - Win32 ReleaseCLEAN" : 
   cd "\prague\Wrappers\xorio"
   $(MAKE) /$(MAKEFLAGS) /F .\xorio.mak CFG="xorio - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"xorio - Win32 Debug" : 
   cd "\prague\Wrappers\xorio"
   $(MAKE) /$(MAKEFLAGS) /F .\xorio.mak CFG="xorio - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"xorio - Win32 DebugCLEAN" : 
   cd "\prague\Wrappers\xorio"
   $(MAKE) /$(MAKEFLAGS) /F .\xorio.mak CFG="xorio - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Timer - Win32 Release" : 
   cd "\prague\Timer"
   $(MAKE) /$(MAKEFLAGS) /F .\timer.mak CFG="Timer - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Timer - Win32 ReleaseCLEAN" : 
   cd "\prague\Timer"
   $(MAKE) /$(MAKEFLAGS) /F .\timer.mak CFG="Timer - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Timer - Win32 Debug" : 
   cd "\prague\Timer"
   $(MAKE) /$(MAKEFLAGS) /F .\timer.mak CFG="Timer - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Timer - Win32 DebugCLEAN" : 
   cd "\prague\Timer"
   $(MAKE) /$(MAKEFLAGS) /F .\timer.mak CFG="Timer - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"btdisk - Win32 Release" : 
   cd "\prague\btdisk"
   $(MAKE) /$(MAKEFLAGS) /F .\btdisk.mak CFG="btdisk - Win32 Release" 
   cd "..\..\PPP\KAV50"

"btdisk - Win32 ReleaseCLEAN" : 
   cd "\prague\btdisk"
   $(MAKE) /$(MAKEFLAGS) /F .\btdisk.mak CFG="btdisk - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"btdisk - Win32 Debug" : 
   cd "\prague\btdisk"
   $(MAKE) /$(MAKEFLAGS) /F .\btdisk.mak CFG="btdisk - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"btdisk - Win32 DebugCLEAN" : 
   cd "\prague\btdisk"
   $(MAKE) /$(MAKEFLAGS) /F .\btdisk.mak CFG="btdisk - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"btimages - Win32 Release" : 
   cd "\prague\btimages"
   $(MAKE) /$(MAKEFLAGS) /F .\btimages.mak CFG="btimages - Win32 Release" 
   cd "..\..\PPP\KAV50"

"btimages - Win32 ReleaseCLEAN" : 
   cd "\prague\btimages"
   $(MAKE) /$(MAKEFLAGS) /F .\btimages.mak CFG="btimages - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"btimages - Win32 Debug" : 
   cd "\prague\btimages"
   $(MAKE) /$(MAKEFLAGS) /F .\btimages.mak CFG="btimages - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"btimages - Win32 DebugCLEAN" : 
   cd "\prague\btimages"
   $(MAKE) /$(MAKEFLAGS) /F .\btimages.mak CFG="btimages - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"WinDiskIO - Win32 Release" : 
   cd "\prague\windiskio"
   $(MAKE) /$(MAKEFLAGS) /F .\WinDiskIO.mak CFG="WinDiskIO - Win32 Release" 
   cd "..\..\PPP\KAV50"

"WinDiskIO - Win32 ReleaseCLEAN" : 
   cd "\prague\windiskio"
   $(MAKE) /$(MAKEFLAGS) /F .\WinDiskIO.mak CFG="WinDiskIO - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"WinDiskIO - Win32 Debug" : 
   cd "\prague\windiskio"
   $(MAKE) /$(MAKEFLAGS) /F .\WinDiskIO.mak CFG="WinDiskIO - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"WinDiskIO - Win32 DebugCLEAN" : 
   cd "\prague\windiskio"
   $(MAKE) /$(MAKEFLAGS) /F .\WinDiskIO.mak CFG="WinDiskIO - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"thpoolimp - Win32 Release" : 
   cd "\prague\thpoolimp"
   $(MAKE) /$(MAKEFLAGS) /F .\thpoolimp.mak CFG="thpoolimp - Win32 Release" 
   cd "..\..\PPP\KAV50"

"thpoolimp - Win32 ReleaseCLEAN" : 
   cd "\prague\thpoolimp"
   $(MAKE) /$(MAKEFLAGS) /F .\thpoolimp.mak CFG="thpoolimp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"thpoolimp - Win32 Debug" : 
   cd "\prague\thpoolimp"
   $(MAKE) /$(MAKEFLAGS) /F .\thpoolimp.mak CFG="thpoolimp - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"thpoolimp - Win32 DebugCLEAN" : 
   cd "\prague\thpoolimp"
   $(MAKE) /$(MAKEFLAGS) /F .\thpoolimp.mak CFG="thpoolimp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AVP3Info - Win32 Release" : 
   cd "\prague\AVP3Info"
   $(MAKE) /$(MAKEFLAGS) /F .\avp3info.mak CFG="AVP3Info - Win32 Release" 
   cd "..\..\PPP\KAV50"

"AVP3Info - Win32 ReleaseCLEAN" : 
   cd "\prague\AVP3Info"
   $(MAKE) /$(MAKEFLAGS) /F .\avp3info.mak CFG="AVP3Info - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AVP3Info - Win32 Debug" : 
   cd "\prague\AVP3Info"
   $(MAKE) /$(MAKEFLAGS) /F .\avp3info.mak CFG="AVP3Info - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"AVP3Info - Win32 DebugCLEAN" : 
   cd "\prague\AVP3Info"
   $(MAKE) /$(MAKEFLAGS) /F .\avp3info.mak CFG="AVP3Info - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"resip - Win32 Release" : 
   cd "\prague\resip"
   $(MAKE) /$(MAKEFLAGS) /F .\resip.mak CFG="resip - Win32 Release" 
   cd "..\..\PPP\KAV50"

"resip - Win32 ReleaseCLEAN" : 
   cd "\prague\resip"
   $(MAKE) /$(MAKEFLAGS) /F .\resip.mak CFG="resip - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"resip - Win32 Debug" : 
   cd "\prague\resip"
   $(MAKE) /$(MAKEFLAGS) /F .\resip.mak CFG="resip - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"resip - Win32 DebugCLEAN" : 
   cd "\prague\resip"
   $(MAKE) /$(MAKEFLAGS) /F .\resip.mak CFG="resip - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"FSDrvPlgn - Win32 Release" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Release" 
   cd "..\..\PPP\KAV50"

"FSDrvPlgn - Win32 ReleaseCLEAN" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"FSDrvPlgn - Win32 Debug" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"FSDrvPlgn - Win32 DebugCLEAN" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"FSSync - Win32 Release" : 
   cd "\Windows\Hook\FSSync"
   $(MAKE) /$(MAKEFLAGS) /F .\FSSync.mak CFG="FSSync - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"FSSync - Win32 ReleaseCLEAN" : 
   cd "\Windows\Hook\FSSync"
   $(MAKE) /$(MAKEFLAGS) /F .\FSSync.mak CFG="FSSync - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"FSSync - Win32 Debug" : 
   cd "\Windows\Hook\FSSync"
   $(MAKE) /$(MAKEFLAGS) /F .\FSSync.mak CFG="FSSync - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"FSSync - Win32 DebugCLEAN" : 
   cd "\Windows\Hook\FSSync"
   $(MAKE) /$(MAKEFLAGS) /F .\FSSync.mak CFG="FSSync - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"dofw - Win32 Release" : 
   cd "\PPP\Install\FolderProtection\dofw"
   $(MAKE) /$(MAKEFLAGS) /F .\dofw.mak CFG="dofw - Win32 Release" 
   cd "..\..\..\KAV50"

"dofw - Win32 ReleaseCLEAN" : 
   cd "\PPP\Install\FolderProtection\dofw"
   $(MAKE) /$(MAKEFLAGS) /F .\dofw.mak CFG="dofw - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"dofw - Win32 Debug" : 
   cd "\PPP\Install\FolderProtection\dofw"
   $(MAKE) /$(MAKEFLAGS) /F .\dofw.mak CFG="dofw - Win32 Debug" 
   cd "..\..\..\KAV50"

"dofw - Win32 DebugCLEAN" : 
   cd "\PPP\Install\FolderProtection\dofw"
   $(MAKE) /$(MAKEFLAGS) /F .\dofw.mak CFG="dofw - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"metadata - Win32 Release" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"metadata - Win32 ReleaseCLEAN" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"metadata - Win32 Debug" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"metadata - Win32 DebugCLEAN" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"antispy - Win32 Release" : 
   cd "\PPP\antispy"
   $(MAKE) /$(MAKEFLAGS) /F .\antispy.mak CFG="antispy - Win32 Release" 
   cd "..\KAV50"

"antispy - Win32 ReleaseCLEAN" : 
   cd "\PPP\antispy"
   $(MAKE) /$(MAKEFLAGS) /F .\antispy.mak CFG="antispy - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"antispy - Win32 Debug" : 
   cd "\PPP\antispy"
   $(MAKE) /$(MAKEFLAGS) /F .\antispy.mak CFG="antispy - Win32 Debug" 
   cd "..\KAV50"

"antispy - Win32 DebugCLEAN" : 
   cd "\PPP\antispy"
   $(MAKE) /$(MAKEFLAGS) /F .\antispy.mak CFG="antispy - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"contentflt - Win32 Release" : 
   cd "\PPP\contentflt"
   $(MAKE) /$(MAKEFLAGS) /F .\contentflt.mak CFG="contentflt - Win32 Release" 
   cd "..\KAV50"

"contentflt - Win32 ReleaseCLEAN" : 
   cd "\PPP\contentflt"
   $(MAKE) /$(MAKEFLAGS) /F .\contentflt.mak CFG="contentflt - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"contentflt - Win32 Debug" : 
   cd "\PPP\contentflt"
   $(MAKE) /$(MAKEFLAGS) /F .\contentflt.mak CFG="contentflt - Win32 Debug" 
   cd "..\KAV50"

"contentflt - Win32 DebugCLEAN" : 
   cd "\PPP\contentflt"
   $(MAKE) /$(MAKEFLAGS) /F .\contentflt.mak CFG="contentflt - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ComStreamIO - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin\ComStreamIO"
   $(MAKE) /$(MAKEFLAGS) /F .\ComStreamIO.mak CFG="ComStreamIO - Win32 Release" 
   cd "..\..\..\..\KAV50"

"ComStreamIO - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin\ComStreamIO"
   $(MAKE) /$(MAKEFLAGS) /F .\ComStreamIO.mak CFG="ComStreamIO - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ComStreamIO - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin\ComStreamIO"
   $(MAKE) /$(MAKEFLAGS) /F .\ComStreamIO.mak CFG="ComStreamIO - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"ComStreamIO - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin\ComStreamIO"
   $(MAKE) /$(MAKEFLAGS) /F .\ComStreamIO.mak CFG="ComStreamIO - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"TheBatPlugin - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatPlugin.mak CFG="TheBatPlugin - Win32 Release" 
   cd "..\..\..\KAV50"

"TheBatPlugin - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatPlugin.mak CFG="TheBatPlugin - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"TheBatPlugin - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatPlugin.mak CFG="TheBatPlugin - Win32 Debug" 
   cd "..\..\..\KAV50"

"TheBatPlugin - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatPlugin"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatPlugin.mak CFG="TheBatPlugin - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"iCheckerSA - Win32 Release" : 
   cd "\PPP\iCheckerSA"
   $(MAKE) /$(MAKEFLAGS) /F .\icheckersa.mak CFG="iCheckerSA - Win32 Release" 
   cd "..\KAV50"

"iCheckerSA - Win32 ReleaseCLEAN" : 
   cd "\PPP\iCheckerSA"
   $(MAKE) /$(MAKEFLAGS) /F .\icheckersa.mak CFG="iCheckerSA - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"iCheckerSA - Win32 Debug" : 
   cd "\PPP\iCheckerSA"
   $(MAKE) /$(MAKEFLAGS) /F .\icheckersa.mak CFG="iCheckerSA - Win32 Debug" 
   cd "..\KAV50"

"iCheckerSA - Win32 DebugCLEAN" : 
   cd "\PPP\iCheckerSA"
   $(MAKE) /$(MAKEFLAGS) /F .\icheckersa.mak CFG="iCheckerSA - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"TheBatAntispam - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TheBatAntispam"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatAntispam.mak CFG="TheBatAntispam - Win32 Release" 
   cd "..\..\..\KAV50"

"TheBatAntispam - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatAntispam"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatAntispam.mak CFG="TheBatAntispam - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"TheBatAntispam - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TheBatAntispam"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatAntispam.mak CFG="TheBatAntispam - Win32 Debug" 
   cd "..\..\..\KAV50"

"TheBatAntispam - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TheBatAntispam"
   $(MAKE) /$(MAKEFLAGS) /F .\TheBatAntispam.mak CFG="TheBatAntispam - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"TrafficMonitor2 - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2"
   $(MAKE) /$(MAKEFLAGS) /F .\TrafficMonitor2.mak CFG="TrafficMonitor2 - Win32 Release" 
   cd "..\..\..\KAV50"

"TrafficMonitor2 - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2"
   $(MAKE) /$(MAKEFLAGS) /F .\TrafficMonitor2.mak CFG="TrafficMonitor2 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"TrafficMonitor2 - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2"
   $(MAKE) /$(MAKEFLAGS) /F .\TrafficMonitor2.mak CFG="TrafficMonitor2 - Win32 Debug" 
   cd "..\..\..\KAV50"

"TrafficMonitor2 - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2"
   $(MAKE) /$(MAKEFLAGS) /F .\TrafficMonitor2.mak CFG="TrafficMonitor2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"POP3Protocoller - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\POP3Protocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\POP3Protocoller.mak CFG="POP3Protocoller - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"POP3Protocoller - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\POP3Protocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\POP3Protocoller.mak CFG="POP3Protocoller - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"POP3Protocoller - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\POP3Protocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\POP3Protocoller.mak CFG="POP3Protocoller - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"POP3Protocoller - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\POP3Protocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\POP3Protocoller.mak CFG="POP3Protocoller - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"SMTPProtocoller - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\SMTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\SMTPProtocoller.mak CFG="SMTPProtocoller - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"SMTPProtocoller - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\SMTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\SMTPProtocoller.mak CFG="SMTPProtocoller - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"SMTPProtocoller - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\SMTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\SMTPProtocoller.mak CFG="SMTPProtocoller - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"SMTPProtocoller - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\SMTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\SMTPProtocoller.mak CFG="SMTPProtocoller - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Mail Checker2 - Win32 Release" : 
   cd "\PPP\MailCommon\MC2"
   $(MAKE) /$(MAKEFLAGS) /F .\MC2.mak CFG="Mail Checker2 - Win32 Release" 
   cd "..\..\KAV50"

"Mail Checker2 - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\MC2"
   $(MAKE) /$(MAKEFLAGS) /F .\MC2.mak CFG="Mail Checker2 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Mail Checker2 - Win32 Debug" : 
   cd "\PPP\MailCommon\MC2"
   $(MAKE) /$(MAKEFLAGS) /F .\MC2.mak CFG="Mail Checker2 - Win32 Debug" 
   cd "..\..\KAV50"

"Mail Checker2 - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\MC2"
   $(MAKE) /$(MAKEFLAGS) /F .\MC2.mak CFG="Mail Checker2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"diff - Win32 Release" : 
   cd "\Prague\Diff"
   $(MAKE) /$(MAKEFLAGS) /F .\diff.mak CFG="diff - Win32 Release" 
   cd "..\..\PPP\KAV50"

"diff - Win32 ReleaseCLEAN" : 
   cd "\Prague\Diff"
   $(MAKE) /$(MAKEFLAGS) /F .\diff.mak CFG="diff - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"diff - Win32 Debug" : 
   cd "\Prague\Diff"
   $(MAKE) /$(MAKEFLAGS) /F .\diff.mak CFG="diff - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"diff - Win32 DebugCLEAN" : 
   cd "\Prague\Diff"
   $(MAKE) /$(MAKEFLAGS) /F .\diff.mak CFG="diff - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"GetSystemInfo - Win32 Release" : 
   cd "\PPP\GetSystemInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\GetSystemInfo.mak CFG="GetSystemInfo - Win32 Release" 
   cd "..\KAV50"

"GetSystemInfo - Win32 ReleaseCLEAN" : 
   cd "\PPP\GetSystemInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\GetSystemInfo.mak CFG="GetSystemInfo - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"GetSystemInfo - Win32 Debug" : 
   cd "\PPP\GetSystemInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\GetSystemInfo.mak CFG="GetSystemInfo - Win32 Debug" 
   cd "..\KAV50"

"GetSystemInfo - Win32 DebugCLEAN" : 
   cd "\PPP\GetSystemInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\GetSystemInfo.mak CFG="GetSystemInfo - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"IMAPProtocoller - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\IMAPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\IMAPProtocoller.mak CFG="IMAPProtocoller - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"IMAPProtocoller - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\IMAPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\IMAPProtocoller.mak CFG="IMAPProtocoller - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"IMAPProtocoller - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\IMAPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\IMAPProtocoller.mak CFG="IMAPProtocoller - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"IMAPProtocoller - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\IMAPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\IMAPProtocoller.mak CFG="IMAPProtocoller - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MailWasher - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\MailWasher"
   $(MAKE) /$(MAKEFLAGS) /F .\MailWasher.mak CFG="MailWasher - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"MailWasher - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\MailWasher"
   $(MAKE) /$(MAKEFLAGS) /F .\MailWasher.mak CFG="MailWasher - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MailWasher - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\MailWasher"
   $(MAKE) /$(MAKEFLAGS) /F .\MailWasher.mak CFG="MailWasher - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"MailWasher - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\Protocollers\MailWasher"
   $(MAKE) /$(MAKEFLAGS) /F .\MailWasher.mak CFG="MailWasher - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"StartUpEnum2 - Win32 Release" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Release" 
   cd "..\KAV50"

"StartUpEnum2 - Win32 ReleaseCLEAN" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"StartUpEnum2 - Win32 Debug" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Debug" 
   cd "..\KAV50"

"StartUpEnum2 - Win32 DebugCLEAN" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"IniFile - Win32 Release" : 
   cd "\PPP\IniFile"
   $(MAKE) /$(MAKEFLAGS) /F .\inifile.mak CFG="IniFile - Win32 Release" 
   cd "..\KAV50"

"IniFile - Win32 ReleaseCLEAN" : 
   cd "\PPP\IniFile"
   $(MAKE) /$(MAKEFLAGS) /F .\inifile.mak CFG="IniFile - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"IniFile - Win32 Debug" : 
   cd "\PPP\IniFile"
   $(MAKE) /$(MAKEFLAGS) /F .\inifile.mak CFG="IniFile - Win32 Debug" 
   cd "..\KAV50"

"IniFile - Win32 DebugCLEAN" : 
   cd "\PPP\IniFile"
   $(MAKE) /$(MAKEFLAGS) /F .\inifile.mak CFG="IniFile - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"winfw - Win32 Release" : 
   cd "\PPP\winfw"
   $(MAKE) /$(MAKEFLAGS) /F .\winfw.mak CFG="winfw - Win32 Release" 
   cd "..\KAV50"

"winfw - Win32 ReleaseCLEAN" : 
   cd "\PPP\winfw"
   $(MAKE) /$(MAKEFLAGS) /F .\winfw.mak CFG="winfw - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"winfw - Win32 Debug" : 
   cd "\PPP\winfw"
   $(MAKE) /$(MAKEFLAGS) /F .\winfw.mak CFG="winfw - Win32 Debug" 
   cd "..\KAV50"

"winfw - Win32 DebugCLEAN" : 
   cd "\PPP\winfw"
   $(MAKE) /$(MAKEFLAGS) /F .\winfw.mak CFG="winfw - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"CryptoHelper - Win32 Release" : 
   cd "\PPP\CryptoHelper"
   $(MAKE) /$(MAKEFLAGS) /F .\crpthlpr.mak CFG="CryptoHelper - Win32 Release" 
   cd "..\KAV50"

"CryptoHelper - Win32 ReleaseCLEAN" : 
   cd "\PPP\CryptoHelper"
   $(MAKE) /$(MAKEFLAGS) /F .\crpthlpr.mak CFG="CryptoHelper - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"CryptoHelper - Win32 Debug" : 
   cd "\PPP\CryptoHelper"
   $(MAKE) /$(MAKEFLAGS) /F .\crpthlpr.mak CFG="CryptoHelper - Win32 Debug" 
   cd "..\KAV50"

"CryptoHelper - Win32 DebugCLEAN" : 
   cd "\PPP\CryptoHelper"
   $(MAKE) /$(MAKEFLAGS) /F .\crpthlpr.mak CFG="CryptoHelper - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"NNTPProtocoller - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\NNTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\NNTPProtocoller.mak CFG="NNTPProtocoller - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"NNTPProtocoller - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\NNTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\NNTPProtocoller.mak CFG="NNTPProtocoller - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"NNTPProtocoller - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\NNTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\NNTPProtocoller.mak CFG="NNTPProtocoller - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"NNTPProtocoller - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\NNTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\NNTPProtocoller.mak CFG="NNTPProtocoller - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"PopupChk - Win32 Release" : 
   cd "\PPP\PopupChk"
   $(MAKE) /$(MAKEFLAGS) /F .\popupchk.mak CFG="PopupChk - Win32 Release" 
   cd "..\KAV50"

"PopupChk - Win32 ReleaseCLEAN" : 
   cd "\PPP\PopupChk"
   $(MAKE) /$(MAKEFLAGS) /F .\popupchk.mak CFG="PopupChk - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"PopupChk - Win32 Debug" : 
   cd "\PPP\PopupChk"
   $(MAKE) /$(MAKEFLAGS) /F .\popupchk.mak CFG="PopupChk - Win32 Debug" 
   cd "..\KAV50"

"PopupChk - Win32 DebugCLEAN" : 
   cd "\PPP\PopupChk"
   $(MAKE) /$(MAKEFLAGS) /F .\popupchk.mak CFG="PopupChk - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"httpprotocoller - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\HTTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\httpprotocoller.mak CFG="httpprotocoller - Win32 Release" 
   cd "..\..\..\..\..\KAV50"

"httpprotocoller - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\HTTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\httpprotocoller.mak CFG="httpprotocoller - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"httpprotocoller - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\HTTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\httpprotocoller.mak CFG="httpprotocoller - Win32 Debug" 
   cd "..\..\..\..\..\KAV50"

"httpprotocoller - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\TrafficMonitor2\ProtocollersEx\HTTPProtocoller"
   $(MAKE) /$(MAKEFLAGS) /F .\httpprotocoller.mak CFG="httpprotocoller - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AntiDial - Win32 Release" : 
   cd "\PPP\AntiDial"
   $(MAKE) /$(MAKEFLAGS) /F .\adialtsk.mak CFG="AntiDial - Win32 Release" 
   cd "..\KAV50"

"AntiDial - Win32 ReleaseCLEAN" : 
   cd "\PPP\AntiDial"
   $(MAKE) /$(MAKEFLAGS) /F .\adialtsk.mak CFG="AntiDial - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AntiDial - Win32 Debug" : 
   cd "\PPP\AntiDial"
   $(MAKE) /$(MAKEFLAGS) /F .\adialtsk.mak CFG="AntiDial - Win32 Debug" 
   cd "..\KAV50"

"AntiDial - Win32 DebugCLEAN" : 
   cd "\PPP\AntiDial"
   $(MAKE) /$(MAKEFLAGS) /F .\adialtsk.mak CFG="AntiDial - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AntiDialHook - Win32 Release" : 
   cd "\PPP\AntiDialHook"
   $(MAKE) /$(MAKEFLAGS) /F .\AntiDialHook.mak CFG="AntiDialHook - Win32 Release" 
   cd "..\KAV50"

"AntiDialHook - Win32 ReleaseCLEAN" : 
   cd "\PPP\AntiDialHook"
   $(MAKE) /$(MAKEFLAGS) /F .\AntiDialHook.mak CFG="AntiDialHook - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AntiDialHook - Win32 Debug" : 
   cd "\PPP\AntiDialHook"
   $(MAKE) /$(MAKEFLAGS) /F .\AntiDialHook.mak CFG="AntiDialHook - Win32 Debug" 
   cd "..\KAV50"

"AntiDialHook - Win32 DebugCLEAN" : 
   cd "\PPP\AntiDialHook"
   $(MAKE) /$(MAKEFLAGS) /F .\AntiDialHook.mak CFG="AntiDialHook - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"basegui - Win32 Release" : 
   cd "\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Release" 
   cd "..\KAV50"

"basegui - Win32 ReleaseCLEAN" : 
   cd "\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"basegui - Win32 Debug" : 
   cd "\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Debug" 
   cd "..\KAV50"

"basegui - Win32 DebugCLEAN" : 
   cd "\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"FileSession - Win32 Release" : 
   cd "\Updater60\FileSession"
   $(MAKE) /$(MAKEFLAGS) /F .\filesession.mak CFG="FileSession - Win32 Release" 
   cd "..\..\PPP\KAV50"

"FileSession - Win32 ReleaseCLEAN" : 
   cd "\Updater60\FileSession"
   $(MAKE) /$(MAKEFLAGS) /F .\filesession.mak CFG="FileSession - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"FileSession - Win32 Debug" : 
   cd "\Updater60\FileSession"
   $(MAKE) /$(MAKEFLAGS) /F .\filesession.mak CFG="FileSession - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"FileSession - Win32 DebugCLEAN" : 
   cd "\Updater60\FileSession"
   $(MAKE) /$(MAKEFLAGS) /F .\filesession.mak CFG="FileSession - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"FTPSession - Win32 Release" : 
   cd "\Updater60\FTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\ftpsession.mak CFG="FTPSession - Win32 Release" 
   cd "..\..\PPP\KAV50"

"FTPSession - Win32 ReleaseCLEAN" : 
   cd "\Updater60\FTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\ftpsession.mak CFG="FTPSession - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"FTPSession - Win32 Debug" : 
   cd "\Updater60\FTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\ftpsession.mak CFG="FTPSession - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"FTPSession - Win32 DebugCLEAN" : 
   cd "\Updater60\FTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\ftpsession.mak CFG="FTPSession - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"httpscan - Win32 Release" : 
   cd "\PPP\MailCommon\Filters\HTTPScan"
   $(MAKE) /$(MAKEFLAGS) /F .\httpscan.mak CFG="httpscan - Win32 Release" 
   cd "..\..\..\KAV50"

"httpscan - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Filters\HTTPScan"
   $(MAKE) /$(MAKEFLAGS) /F .\httpscan.mak CFG="httpscan - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"httpscan - Win32 Debug" : 
   cd "\PPP\MailCommon\Filters\HTTPScan"
   $(MAKE) /$(MAKEFLAGS) /F .\httpscan.mak CFG="httpscan - Win32 Debug" 
   cd "..\..\..\KAV50"

"httpscan - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Filters\HTTPScan"
   $(MAKE) /$(MAKEFLAGS) /F .\httpscan.mak CFG="httpscan - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"HTTPSession - Win32 Release" : 
   cd "\Updater60\HTTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\httpsession.mak CFG="HTTPSession - Win32 Release" 
   cd "..\..\PPP\KAV50"

"HTTPSession - Win32 ReleaseCLEAN" : 
   cd "\Updater60\HTTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\httpsession.mak CFG="HTTPSession - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"HTTPSession - Win32 Debug" : 
   cd "\Updater60\HTTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\httpsession.mak CFG="HTTPSession - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"HTTPSession - Win32 DebugCLEAN" : 
   cd "\Updater60\HTTPSession"
   $(MAKE) /$(MAKEFLAGS) /F .\httpsession.mak CFG="HTTPSession - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"NetSession - Win32 Release" : 
   cd "\Updater60\NetSession"
   $(MAKE) /$(MAKEFLAGS) /F .\netsession.mak CFG="NetSession - Win32 Release" 
   cd "..\..\PPP\KAV50"

"NetSession - Win32 ReleaseCLEAN" : 
   cd "\Updater60\NetSession"
   $(MAKE) /$(MAKEFLAGS) /F .\netsession.mak CFG="NetSession - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"NetSession - Win32 Debug" : 
   cd "\Updater60\NetSession"
   $(MAKE) /$(MAKEFLAGS) /F .\netsession.mak CFG="NetSession - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"NetSession - Win32 DebugCLEAN" : 
   cd "\Updater60\NetSession"
   $(MAKE) /$(MAKEFLAGS) /F .\netsession.mak CFG="NetSession - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"NTLM - Win32 Release" : 
   cd "\Updater60\NTLM"
   $(MAKE) /$(MAKEFLAGS) /F .\ntlm.mak CFG="NTLM - Win32 Release" 
   cd "..\..\PPP\KAV50"

"NTLM - Win32 ReleaseCLEAN" : 
   cd "\Updater60\NTLM"
   $(MAKE) /$(MAKEFLAGS) /F .\ntlm.mak CFG="NTLM - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"NTLM - Win32 Debug" : 
   cd "\Updater60\NTLM"
   $(MAKE) /$(MAKEFLAGS) /F .\ntlm.mak CFG="NTLM - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"NTLM - Win32 DebugCLEAN" : 
   cd "\Updater60\NTLM"
   $(MAKE) /$(MAKEFLAGS) /F .\ntlm.mak CFG="NTLM - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ProductInfo - Win32 Release" : 
   cd "\Updater60\ProductInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\productinfo.mak CFG="ProductInfo - Win32 Release" 
   cd "..\..\PPP\KAV50"

"ProductInfo - Win32 ReleaseCLEAN" : 
   cd "\Updater60\ProductInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\productinfo.mak CFG="ProductInfo - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ProductInfo - Win32 Debug" : 
   cd "\Updater60\ProductInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\productinfo.mak CFG="ProductInfo - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"ProductInfo - Win32 DebugCLEAN" : 
   cd "\Updater60\ProductInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\productinfo.mak CFG="ProductInfo - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"Socket - Win32 Release" : 
   cd "\Updater60\Socket"
   $(MAKE) /$(MAKEFLAGS) /F .\socket.mak CFG="Socket - Win32 Release" 
   cd "..\..\PPP\KAV50"

"Socket - Win32 ReleaseCLEAN" : 
   cd "\Updater60\Socket"
   $(MAKE) /$(MAKEFLAGS) /F .\socket.mak CFG="Socket - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"Socket - Win32 Debug" : 
   cd "\Updater60\Socket"
   $(MAKE) /$(MAKEFLAGS) /F .\socket.mak CFG="Socket - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"Socket - Win32 DebugCLEAN" : 
   cd "\Updater60\Socket"
   $(MAKE) /$(MAKEFLAGS) /F .\socket.mak CFG="Socket - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"TrainSupport - Win32 Release" : 
   cd "\PPP\mailcommon\filters\antispam\TrainSupport"
   $(MAKE) /$(MAKEFLAGS) /F .\TrainSupport.mak CFG="TrainSupport - Win32 Release" 
   cd "..\..\..\..\KAV50"

"TrainSupport - Win32 ReleaseCLEAN" : 
   cd "\PPP\mailcommon\filters\antispam\TrainSupport"
   $(MAKE) /$(MAKEFLAGS) /F .\TrainSupport.mak CFG="TrainSupport - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"TrainSupport - Win32 Debug" : 
   cd "\PPP\mailcommon\filters\antispam\TrainSupport"
   $(MAKE) /$(MAKEFLAGS) /F .\TrainSupport.mak CFG="TrainSupport - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"TrainSupport - Win32 DebugCLEAN" : 
   cd "\PPP\mailcommon\filters\antispam\TrainSupport"
   $(MAKE) /$(MAKEFLAGS) /F .\TrainSupport.mak CFG="TrainSupport - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"wingui - Win32 Release" : 
   cd "\PPP\basegui\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Release" 
   cd "..\..\KAV50"

"wingui - Win32 ReleaseCLEAN" : 
   cd "\PPP\basegui\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"wingui - Win32 Debug" : 
   cd "\PPP\basegui\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Debug" 
   cd "..\..\KAV50"

"wingui - Win32 DebugCLEAN" : 
   cd "\PPP\basegui\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AntiPhishing - Win32 Release" : 
   cd "\PPP\AntiPhishing"
   $(MAKE) /$(MAKEFLAGS) /F .\aphish.mak CFG="AntiPhishing - Win32 Release" 
   cd "..\KAV50"

"AntiPhishing - Win32 ReleaseCLEAN" : 
   cd "\PPP\AntiPhishing"
   $(MAKE) /$(MAKEFLAGS) /F .\aphish.mak CFG="AntiPhishing - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AntiPhishing - Win32 Debug" : 
   cd "\PPP\AntiPhishing"
   $(MAKE) /$(MAKEFLAGS) /F .\aphish.mak CFG="AntiPhishing - Win32 Debug" 
   cd "..\KAV50"

"AntiPhishing - Win32 DebugCLEAN" : 
   cd "\PPP\AntiPhishing"
   $(MAKE) /$(MAKEFLAGS) /F .\aphish.mak CFG="AntiPhishing - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"OEAntiSpam - Win32 Release" : 
   cd "\PPP\MailCommon\Interceptors\OutlookExpress\OEAntiSpam"
   $(MAKE) /$(MAKEFLAGS) /F .\OEAntiSpam.mak CFG="OEAntiSpam - Win32 Release" 
   cd "..\..\..\..\KAV50"

"OEAntiSpam - Win32 ReleaseCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookExpress\OEAntiSpam"
   $(MAKE) /$(MAKEFLAGS) /F .\OEAntiSpam.mak CFG="OEAntiSpam - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"OEAntiSpam - Win32 Debug" : 
   cd "\PPP\MailCommon\Interceptors\OutlookExpress\OEAntiSpam"
   $(MAKE) /$(MAKEFLAGS) /F .\OEAntiSpam.mak CFG="OEAntiSpam - Win32 Debug" 
   cd "..\..\..\..\KAV50"

"OEAntiSpam - Win32 DebugCLEAN" : 
   cd "\PPP\MailCommon\Interceptors\OutlookExpress\OEAntiSpam"
   $(MAKE) /$(MAKEFLAGS) /F .\OEAntiSpam.mak CFG="OEAntiSpam - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"AntiPhishingTask - Win32 Release" : 
   cd "\PPP\AntiPhishingTask"
   $(MAKE) /$(MAKEFLAGS) /F .\aphisht.mak CFG="AntiPhishingTask - Win32 Release" 
   cd "..\KAV50"

"AntiPhishingTask - Win32 ReleaseCLEAN" : 
   cd "\PPP\AntiPhishingTask"
   $(MAKE) /$(MAKEFLAGS) /F .\aphisht.mak CFG="AntiPhishingTask - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"AntiPhishingTask - Win32 Debug" : 
   cd "\PPP\AntiPhishingTask"
   $(MAKE) /$(MAKEFLAGS) /F .\aphisht.mak CFG="AntiPhishingTask - Win32 Debug" 
   cd "..\KAV50"

"AntiPhishingTask - Win32 DebugCLEAN" : 
   cd "\PPP\AntiPhishingTask"
   $(MAKE) /$(MAKEFLAGS) /F .\aphisht.mak CFG="AntiPhishingTask - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"tm - Win32 Release" : 
   cd "\PPP\tm2"
   $(MAKE) /$(MAKEFLAGS) /F .\tm.mak CFG="tm - Win32 Release" 
   cd "..\KAV50"

"tm - Win32 ReleaseCLEAN" : 
   cd "\PPP\tm2"
   $(MAKE) /$(MAKEFLAGS) /F .\tm.mak CFG="tm - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"tm - Win32 Debug" : 
   cd "\PPP\tm2"
   $(MAKE) /$(MAKEFLAGS) /F .\tm.mak CFG="tm - Win32 Debug" 
   cd "..\KAV50"

"tm - Win32 DebugCLEAN" : 
   cd "\PPP\tm2"
   $(MAKE) /$(MAKEFLAGS) /F .\tm.mak CFG="tm - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"MKavIO - Win32 Release" : 
   cd "\prague\MKavIO"
   $(MAKE) /$(MAKEFLAGS) /F .\MKavIO.mak CFG="MKavIO - Win32 Release" 
   cd "..\..\PPP\KAV50"

"MKavIO - Win32 ReleaseCLEAN" : 
   cd "\prague\MKavIO"
   $(MAKE) /$(MAKEFLAGS) /F .\MKavIO.mak CFG="MKavIO - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"MKavIO - Win32 Debug" : 
   cd "\prague\MKavIO"
   $(MAKE) /$(MAKEFLAGS) /F .\MKavIO.mak CFG="MKavIO - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"MKavIO - Win32 DebugCLEAN" : 
   cd "\prague\MKavIO"
   $(MAKE) /$(MAKEFLAGS) /F .\MKavIO.mak CFG="MKavIO - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"lha - Win32 Release" : 
   cd "\prague\MiniArchiver\LHA"
   $(MAKE) /$(MAKEFLAGS) /F .\lha.mak CFG="lha - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"lha - Win32 ReleaseCLEAN" : 
   cd "\prague\MiniArchiver\LHA"
   $(MAKE) /$(MAKEFLAGS) /F .\lha.mak CFG="lha - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"lha - Win32 Debug" : 
   cd "\prague\MiniArchiver\LHA"
   $(MAKE) /$(MAKEFLAGS) /F .\lha.mak CFG="lha - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"lha - Win32 DebugCLEAN" : 
   cd "\prague\MiniArchiver\LHA"
   $(MAKE) /$(MAKEFLAGS) /F .\lha.mak CFG="lha - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"connector - Win32 Release" : 
   cd "\PPP\Connector"
   $(MAKE) /$(MAKEFLAGS) /F .\connector.mak CFG="connector - Win32 Release" 
   cd "..\KAV50"

"connector - Win32 ReleaseCLEAN" : 
   cd "\PPP\Connector"
   $(MAKE) /$(MAKEFLAGS) /F .\connector.mak CFG="connector - Win32 Release" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"connector - Win32 Debug" : 
   cd "\PPP\Connector"
   $(MAKE) /$(MAKEFLAGS) /F .\connector.mak CFG="connector - Win32 Debug" 
   cd "..\KAV50"

"connector - Win32 DebugCLEAN" : 
   cd "\PPP\Connector"
   $(MAKE) /$(MAKEFLAGS) /F .\connector.mak CFG="connector - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"ArcSession - Win32 Release" : 
   cd "\Updater60\ArcSession"
   $(MAKE) /$(MAKEFLAGS) /F .\arcsession.mak CFG="ArcSession - Win32 Release" 
   cd "..\..\PPP\KAV50"

"ArcSession - Win32 ReleaseCLEAN" : 
   cd "\Updater60\ArcSession"
   $(MAKE) /$(MAKEFLAGS) /F .\arcsession.mak CFG="ArcSession - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"ArcSession - Win32 Debug" : 
   cd "\Updater60\ArcSession"
   $(MAKE) /$(MAKEFLAGS) /F .\arcsession.mak CFG="ArcSession - Win32 Debug" 
   cd "..\..\PPP\KAV50"

"ArcSession - Win32 DebugCLEAN" : 
   cd "\Updater60\ArcSession"
   $(MAKE) /$(MAKEFLAGS) /F .\arcsession.mak CFG="ArcSession - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\KAV50"

!ENDIF 

!IF  "$(CFG)" == "kav50 - Win32 Release"

"CAB - Win32 Release" : 
   cd "\prague\MiniArchiver\cab"
   $(MAKE) /$(MAKEFLAGS) /F .\CAB.mak CFG="CAB - Win32 Release" 
   cd "..\..\..\PPP\KAV50"

"CAB - Win32 ReleaseCLEAN" : 
   cd "\prague\MiniArchiver\cab"
   $(MAKE) /$(MAKEFLAGS) /F .\CAB.mak CFG="CAB - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

"CAB - Win32 Debug" : 
   cd "\prague\MiniArchiver\cab"
   $(MAKE) /$(MAKEFLAGS) /F .\CAB.mak CFG="CAB - Win32 Debug" 
   cd "..\..\..\PPP\KAV50"

"CAB - Win32 DebugCLEAN" : 
   cd "\prague\MiniArchiver\cab"
   $(MAKE) /$(MAKEFLAGS) /F .\CAB.mak CFG="CAB - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\KAV50"

!ENDIF 


!ENDIF 

