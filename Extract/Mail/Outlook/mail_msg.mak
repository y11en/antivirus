# Microsoft Developer Studio Generated NMAKE File, Based on mail_msg.dsp
!IF "$(CFG)" == ""
CFG=mail_msg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mail_msg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mail_msg - Win32 Release" && "$(CFG)" != "mail_msg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mail_msg.mak" CFG="mail_msg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mail_msg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mail_msg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "mail_msg - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/Outlook
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\MailMsg.ppl"


CLEAN :
	-@erase "$(INTDIR)\cmn_mapi.obj"
	-@erase "$(INTDIR)\mail_msg.res"
	-@erase "$(INTDIR)\msg_io.obj"
	-@erase "$(INTDIR)\msg_mapi.obj"
	-@erase "$(INTDIR)\msg_objptr.obj"
	-@erase "$(INTDIR)\msg_os.obj"
	-@erase "$(INTDIR)\msg_tr.obj"
	-@erase "$(INTDIR)\plugin_mailmsg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MailMsg.exp"
	-@erase "$(OUTDIR)\MailMsg.pdb"
	-@erase "$(OUTDIR)\MailMsg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "..\include\iface" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mail_msg_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mail_msg.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mail_msg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib ole32.lib /nologo /base:"0x64f00000" /dll /incremental:no /pdb:"$(OUTDIR)\MailMsg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MailMsg.ppl" /implib:"$(OUTDIR)\MailMsg.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\cmn_mapi.obj" \
	"$(INTDIR)\msg_io.obj" \
	"$(INTDIR)\msg_mapi.obj" \
	"$(INTDIR)\msg_objptr.obj" \
	"$(INTDIR)\msg_os.obj" \
	"$(INTDIR)\msg_tr.obj" \
	"$(INTDIR)\plugin_mailmsg.obj" \
	"$(INTDIR)\mail_msg.res"

"$(OUTDIR)\MailMsg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\MailMsg.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MailMsg.ppl"
   tsigner "\out\Release\MailMsg.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/Outlook
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\MailMsg.ppl" "$(OUTDIR)\mail_msg.bsc"


CLEAN :
	-@erase "$(INTDIR)\cmn_mapi.obj"
	-@erase "$(INTDIR)\cmn_mapi.sbr"
	-@erase "$(INTDIR)\mail_msg.res"
	-@erase "$(INTDIR)\msg_io.obj"
	-@erase "$(INTDIR)\msg_io.sbr"
	-@erase "$(INTDIR)\msg_mapi.obj"
	-@erase "$(INTDIR)\msg_mapi.sbr"
	-@erase "$(INTDIR)\msg_objptr.obj"
	-@erase "$(INTDIR)\msg_objptr.sbr"
	-@erase "$(INTDIR)\msg_os.obj"
	-@erase "$(INTDIR)\msg_os.sbr"
	-@erase "$(INTDIR)\msg_tr.obj"
	-@erase "$(INTDIR)\msg_tr.sbr"
	-@erase "$(INTDIR)\plugin_mailmsg.obj"
	-@erase "$(INTDIR)\plugin_mailmsg.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mail_msg.bsc"
	-@erase "$(OUTDIR)\mail_msg.map"
	-@erase "$(OUTDIR)\MailMsg.exp"
	-@erase "$(OUTDIR)\MailMsg.ilk"
	-@erase "$(OUTDIR)\MailMsg.pdb"
	-@erase "$(OUTDIR)\MailMsg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "..\include\iface" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mail_msg_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mail_msg.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mail_msg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cmn_mapi.sbr" \
	"$(INTDIR)\msg_io.sbr" \
	"$(INTDIR)\msg_mapi.sbr" \
	"$(INTDIR)\msg_objptr.sbr" \
	"$(INTDIR)\msg_os.sbr" \
	"$(INTDIR)\msg_tr.sbr" \
	"$(INTDIR)\plugin_mailmsg.sbr"

"$(OUTDIR)\mail_msg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib ole32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MailMsg.pdb" /map:"../../out/Debug/mail_msg.map" /debug /machine:I386 /out:"$(OUTDIR)\MailMsg.ppl" /implib:"$(OUTDIR)\MailMsg.lib" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\cmn_mapi.obj" \
	"$(INTDIR)\msg_io.obj" \
	"$(INTDIR)\msg_mapi.obj" \
	"$(INTDIR)\msg_objptr.obj" \
	"$(INTDIR)\msg_os.obj" \
	"$(INTDIR)\msg_tr.obj" \
	"$(INTDIR)\plugin_mailmsg.obj" \
	"$(INTDIR)\mail_msg.res"

"$(OUTDIR)\MailMsg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mail_msg.dep")
!INCLUDE "mail_msg.dep"
!ELSE 
!MESSAGE Warning: cannot find "mail_msg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mail_msg - Win32 Release" || "$(CFG)" == "mail_msg - Win32 Debug"
SOURCE=.\cmn_mapi.cpp

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\cmn_mapi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\cmn_mapi.obj"	"$(INTDIR)\cmn_mapi.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg_io.c

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\msg_io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\msg_io.obj"	"$(INTDIR)\msg_io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg_mapi.cpp

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\msg_mapi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\msg_mapi.obj"	"$(INTDIR)\msg_mapi.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg_objptr.c

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\msg_objptr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\msg_objptr.obj"	"$(INTDIR)\msg_objptr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg_os.c

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\msg_os.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\msg_os.obj"	"$(INTDIR)\msg_os.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msg_tr.c

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\msg_tr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\msg_tr.obj"	"$(INTDIR)\msg_tr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_mailmsg.c

!IF  "$(CFG)" == "mail_msg - Win32 Release"


"$(INTDIR)\plugin_mailmsg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mail_msg - Win32 Debug"


"$(INTDIR)\plugin_mailmsg.obj"	"$(INTDIR)\plugin_mailmsg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mail_msg.rc

"$(INTDIR)\mail_msg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

