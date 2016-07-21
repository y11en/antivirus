# Microsoft Developer Studio Generated NMAKE File, Based on AVP3 LLIO.dsp
!IF "$(CFG)" == ""
CFG=AVP3 LLIO - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AVP3 LLIO - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AVP3 LLIO - Win32 Release" && "$(CFG)" != "AVP3 LLIO - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVP3 LLIO.mak" CFG="AVP3 LLIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVP3 LLIO - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "AVP3 LLIO - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

OUTDIR=.\AVP3_LLIO___Win32_Release
INTDIR=.\AVP3_LLIO___Win32_Release

!IF "$(RECURSE)" == "0" 

ALL : 

!ELSE 

ALL : "Avp_ioNT - Win32 Release" "Avp_io32 - Win32 Release" "Avp_io VxD - Win32 Release" 

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Avp_io VxD - Win32 ReleaseCLEAN" "Avp_io32 - Win32 ReleaseCLEAN" "Avp_ioNT - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe

!IF  "$(CFG)" == "AVP3 LLIO - Win32 Release"

!ELSEIF  "$(CFG)" == "AVP3 LLIO - Win32 Debug"

!ENDIF 

MTL_PROJ=

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AVP3 LLIO.dep")
!INCLUDE "AVP3 LLIO.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVP3 LLIO.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVP3 LLIO - Win32 Release" || "$(CFG)" == "AVP3 LLIO - Win32 Debug"

!IF  "$(CFG)" == "AVP3 LLIO - Win32 Release"

"Avp_io VxD - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Release" 
   cd "."

"Avp_io VxD - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "AVP3 LLIO - Win32 Debug"

"Avp_io VxD - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug" 
   cd "."

"Avp_io VxD - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "AVP3 LLIO - Win32 Release"

"Avp_io32 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Release" 
   cd "."

"Avp_io32 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "AVP3 LLIO - Win32 Debug"

"Avp_io32 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Debug" 
   cd "."

"Avp_io32 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "AVP3 LLIO - Win32 Release"

"Avp_ioNT - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Release" 
   cd "."

"Avp_ioNT - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "AVP3 LLIO - Win32 Debug"

"Avp_ioNT - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Debug" 
   cd "."

"Avp_ioNT - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

