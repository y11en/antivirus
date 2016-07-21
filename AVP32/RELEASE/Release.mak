#////////////////////////////////////////////////////////////////////
#//
#//  RELEASE.MAK
#//  AVP 3.0 Release helper makefile
#//  Project AVP
#//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
#//  Copyright (c) Kaspersky Labs.
#//
#////////////////////////////////////////////////////////////////////

# debug - non execute
#!CMDSWITCHES +N

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

COPY=xcopy /y /r /f /z /d

OUT_SCN=\\AVP_SERVER2\TESTING\AVP Scanner\3.5
OUT_MON=\\AVP_SERVER2\TESTING\AVP Monitor\3.5
RESH_SCN=\\AVP_SERVER\Release\Loc\AVP SCANNER\LOC_RES
RESH_MON=\\AVP_SERVER\Release\Loc\AVP MONITOR\LOC_RES
S_CURRENT=\\VINTIK\Test\!current\Editor

OUT_MON_351 =$(OUT_MON)\351
OUT_MON_400 =$(OUT_MON)\400
OUT_MON_2000=$(OUT_MON)\2000
OUT_MON_95  =$(OUT_MON)\95
OUT_SCN_NT  =$(OUT_SCN)\NT
OUT_SCN_95  =$(OUT_SCN)\95


IN351=GKNT\351
IN400=GKNT2\40
IN2000=GKNT2\2000


OUT_OBJ=\
"$(OUT_SCN)\scarabey.exe" \
"$(OUT_SCN)\avp32.exe" \
"$(OUT_SCN)\avpedit.exe" \
"$(OUT_SCN)\method.dll" \
"$(OUT_SCN)\avpbase.dll" \
"$(OUT_SCN_95)\avp_io32.dll" \
"$(OUT_SCN_95)\avp_io.vxd" \
"$(OUT_SCN_NT)\avp_ioNT.dll" \
"$(OUT_MON)\avpm.exe" \
"$(OUT_MON_95)\gk95.vxd" \
"$(OUT_MON_95)\avp95.vxd" \
"$(OUT_MON_95)\avp_io.vxd" 

OUT_MON_351_OBJ=\
"$(OUT_MON_351)\fsgk.ws" \
"$(OUT_MON_351)\fsgk.srv" \
"$(OUT_MON_351)\fsrec.sys" \
"$(OUT_MON_351)\fsfilter.sys"

OUT_MON_400_OBJ=\
"$(OUT_MON_400)\fsgk.sys" \
"$(OUT_MON_400)\fsrec.sys" \
"$(OUT_MON_400)\fsfilter.sys"

OUT_MON_2000_OBJ=\
"$(OUT_MON_2000)\fsgk.sys" \
"$(OUT_MON_2000)\fsrec.sys" \
"$(OUT_MON_2000)\fsfilter.sys"

RESH_MON_OBJ=\
"$(RESH_MON)\resource.h" \
"$(RESH_MON)\avpm.rc"

RESH_SCN_OBJ=\
"$(RESH_SCN)\resource.h" \
"$(RESH_SCN)\avp32.rc"

#//////////////////////////////////////////////////
#// Main target
#//
TARGET=$(OUT_SCN)\Release.txt

"$(TARGET)": \
"$(S_CURRENT)" \
"$(OUT_SCN)" \
"$(OUT_SCN_NT)" \
"$(OUT_SCN_95)" \
"$(OUT_MON)" \
"$(OUT_MON_351)" \
"$(OUT_MON_400)" \
"$(OUT_MON_2000)" \
"$(OUT_MON_95)" \
$(OUT_OBJ) \
$(OUT_MON_351_OBJ) \
$(OUT_MON_400_OBJ) \
$(OUT_MON_2000_OBJ) \
$(RESH_MON_OBJ) \
$(RESH_SCN_OBJ) \
"$(OUT_MON_351)\fsavp.sys" \
"$(OUT_MON_400)\fsavp.sys" \
"$(OUT_MON_2000)\fsavp.sys" \
$(@F)
	@$(COPY) $(@F) $(@D)
	@echo . >> $@
	hacksaw Release_Mail.hsc 
	@cd ..
	ss Checkin $$/Avp32/*.* -K -C- -I-Y
	@cd ..\CommonFiles
	ss Checkin $$/CommonFiles/*.* -K -C- -I-Y

$(OUT_OBJ): $(@F)
	@$(COPY) $(@F) $(@D)
	@$(COPY) $(@F) $(S_CURRENT)

$(OUT_MON_351_OBJ):   ..\$(IN351)\$(@F) 
	@$(COPY) ..\$(IN351)\$(@F) $(@D)

$(OUT_MON_400_OBJ):   ..\$(IN400)\$(@F) 
	@$(COPY) ..\$(IN400)\$(@F) $(@D)

$(OUT_MON_2000_OBJ):  ..\$(IN2000)\$(@F) 
	@$(COPY) ..\$(IN2000)\$(@F) $(@D)

$(RESH_MON_OBJ):  ..\AVPM\$(@F) 
	@$(COPY) ..\AVPM\$(@F) $(@D)
	
$(RESH_SCN_OBJ):  ..\AVP_32\$(@F) 
	@$(COPY) ..\AVP_32\$(@F) $(@D)

"$(OUT_MON_351)\fsavp.sys" : FSAVP351.SYS
	@$(COPY) $** $(@D)

"$(OUT_MON_400)\fsavp.sys" : FSAVP402.SYS
	@$(COPY) $** $(@D)

"$(OUT_MON_2000)\fsavp.sys": FSAVP402.SYS
	@$(COPY) $** $(@D)

"$(OUT_SCN)" :
    if not exist "$(OUT_SCN)/$(NULL)" mkdir "$(OUT_SCN)"
"$(OUT_SCN_95)" : "$(OUT_SCN)" 
    if not exist "$(OUT_SCN_95)/$(NULL)" mkdir "$(OUT_SCN_95)"
"$(OUT_SCN_NT)" : "$(OUT_SCN)" 
    if not exist "$(OUT_SCN_NT)/$(NULL)" mkdir "$(OUT_SCN_NT)"
"$(OUT_MON)" :
    if not exist "$(OUT_MON)/$(NULL)" mkdir "$(OUT_MON)"
"$(OUT_MON_95)" : "$(OUT_MON)" 
    if not exist "$(OUT_MON_95)/$(NULL)" mkdir "$(OUT_MON_95)"
"$(OUT_MON_351)" : "$(OUT_MON)" 
    if not exist "$(OUT_MON_351)/$(NULL)" mkdir "$(OUT_MON_351)"
"$(OUT_MON_400)" : "$(OUT_MON)" 
    if not exist "$(OUT_MON_400)/$(NULL)" mkdir "$(OUT_MON_400)"
"$(OUT_MON_2000)" : "$(OUT_MON)" 
    if not exist "$(OUT_MON_2000)/$(NULL)" mkdir "$(OUT_MON_2000)"
"$(S_CURRENT)":
    if not exist "$(S_CURRENT)/$(NULL)" mkdir "$(S_CURRENT)"


CLEAN :
	-@erase "$(OUT_SCN)\*.*"
	-@erase "$(OUT_SCN_95)\*.*"
	-@erase "$(OUT_SCN_NT)\*.*"
	-@erase "$(OUT_MON)\*.*"
	-@erase "$(OUT_MON_95)\*.*"
	-@erase "$(OUT_MON_351)\*.*"
	-@erase "$(OUT_MON_400)\*.*"
	-@erase "$(OUT_MON_2000)\*.*"

