#////////////////////////////////////////////////////////////////////
#//
#//  INSTALL.MAK
#//  Inno Setup 4.0 Installer makefile
#//  Project: AVP 3.5
#//  Author:  Graf
#//  Copyright (c) Kaspersky Labs, 2003
#//
#////////////////////////////////////////////////////////////////////
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF

COMPANY_NAME_STR=Kaspersky Labs
COMPANY_SITE_STR=http://www.kaspersky.com
KASPERSKY_ANTIVIRUS_STR=Kaspersky Anti-Virus
HKLM_SETUP_FOLDERS_KEY=SOFTWARE\KasperskyLab\SetupFolders
BASES_NAME_STR=$(KASPERSKY_ANTIVIRUS_STR) Bases

MODIF_STR=AVP
PRODUCT_NAME_STR=$(KASPERSKY_ANTIVIRUS_STR) $(MODIF_STR)
HKLM_PRODUCT_KEY=SOFTWARE\KasperskyLab\$(PRODUCT_NAME_STR)
COPYRIGHT_STR=Copyright © $(COMPANY_NAME_STR), 2003.

VERSION_STR=3.5.0.165

OUT=..\$(BUILD)

TARGET=$(OUT)\$(MODIF_STR).$(BUILD).$(VERSION_STR).exe

AVPABT_OCX=..\..\commonfiles\release\AVPABT.OCX
AVPRVC_OCX=..\..\commonfiles\release\AVPRVC.OCX

M_SPECIAL=\
$(AVPABT_OCX) \
$(AVPRVC_OCX) \
$(OUT)\method.dll \
$(OUT)\avpedit.exe \

M_PRODUCT=\
0004B81E.key \
avp.txt \
avp.ini \
$(OUT)\avp32.exe \
$(OUT)\avpbase.dll \
$(OUT)\avp_io.vxd \
$(OUT)\avp_iont.dll \
$(OUT)\avp_io32.dll \

# file lists
FILES       =$(BUILD)\files.iss
FILES_BASES =$(BUILD)\files_bases.iss
FILES_CLEAN =$(BUILD)\files_clean.iss

REDIST=..\..\ppp\install\redist

DATA_ROOT       ={app}
DATA_BASES      =$(DATA_ROOT)\Bases

$(TARGET): INSTALL.MAK $(M_PRODUCT) $(M_SPECIAL) $(FILES_CLEAN) $(FILES) $(FILES_BASES)
	-@ echo --------------------Configuration: $(PRODUCT_NAME_STR) - Win32 $(BUILD)--------------------
	-@ erase $@
	-@ iscc.exe <<$(MODIF_STR).$(BUILD).iss
[Setup]
AppName=$(PRODUCT_NAME_STR)
AppVersion=$(VERSION_STR)
AppVerName=$(PRODUCT_NAME_STR) version $(VERSION_STR)
AppPublisher=$(COMPANY_NAME_STR)
AppCopyright=$(COPYRIGHT_STR)
AppPublisherURL=$(COMPANY_SITE_STR)
AppSupportURL=$(COMPANY_SITE_STR)
AppUpdatesURL=$(COMPANY_SITE_STR)
VersionInfoCompany=$(COMPANY_NAME_STR)
VersionInfoTextVersion=$(VERSION_STR)
VersionInfoVersion=$(VERSION_STR)
VersionInfoDescription=$(PRODUCT_NAME_STR)
DefaultGroupName=$(PRODUCT_NAME_STR)
DefaultDirName={reg:HKLM\$(HKLM_SETUP_FOLDERS_KEY),$(PRODUCT_NAME_STR)|{pf}\$(PRODUCT_NAME_STR)}
DisableProgramGroupPage=yes
OutputDir=$(OUT)
OutputBaseFilename=$*
PrivilegesRequired=admin
AlwaysShowComponentsList=no
WizardImageFile=ppp.bmp
WizardSmallImageFile=ppp_small.bmp
InfoBeforeFile=avp.txt
Compression=bzip
SolidCompression=yes
DisableReadyPage=yes
;AppMutex=$(PRODUCT_NAME_STR)
;UninstallRestartComputer=yes

[Messages] 
BeveledLabel= $(COMPANY_NAME_STR)

[Code]
function NeedRestart(): Boolean;
begin
	Result := not UsingWinNT();
end;

[Dirs] 
Name: "$(DATA_BASES)";       Flags: uninsalwaysuninstall;

[Tasks] 
Name: run;      Description: "&Start $(PRODUCT_NAME_STR) after setup";
Name: editor;   Description: "&Install $(PRODUCT_NAME_STR) Virus database Editor"; Flags: unchecked;

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
#include "$(FILES)"
#include "$(FILES_BASES)"

Source: "$(AVPABT_OCX)";      DestDir: "{app}";         Flags: regserver uninsrestartdelete;
Source: "$(AVPRVC_OCX)";      DestDir: "{app}";         Flags: regserver uninsrestartdelete;
Source: "$(OUT)\method.dll";  DestDir: "{app}";         Tasks: editor;
Source: "$(OUT)\avpedit.exe"; DestDir: "{app}";        Tasks: editor;

; begin VC system files
; (Note: Scroll to the right to see the full lines!)
;Source: "$(REDIST)\asycfilt.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
;Source: "$(REDIST)\atla.dll";     DestName: "atl.dll"; DestDir: "{sys}"; MinVersion: 4,0; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\atlu.dll";     DestName: "atl.dll"; DestDir: "{sys}"; MinVersion: 0,4; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\comcat.dll";   DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\mfc42.dll";    DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\mfc42u.dll";   DestDir: "{sys}"; MinVersion: 0,4; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\msvcp60.dll";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
;Source: "$(REDIST)\msvcirt.dll";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
;Source: "$(REDIST)\msvcrt.dll";   DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall
;Source: "$(REDIST)\oleaut32.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\olepro32.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regserver
;Source: "$(REDIST)\stdole2.tlb";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall regtypelib
;!IF "$(BUILD)" == "Debug"
;Source: "$(REDIST)\MSVCP60d.DLL"; DestDir: "{sys}"; Flags: uninsneveruninstall restartreplace;
;Source: "$(REDIST)\MSVCRTd.DLL";  DestDir: "{sys}"; Flags: uninsneveruninstall restartreplace;
;!ENDIF
; end VC system files

[Icons]
Name: "{group}\$(PRODUCT_NAME_STR)"; Filename: "{app}\avp32.exe";
Name: "{group}\Uninstall $(PRODUCT_NAME_STR)"; Filename: "{uninstallexe}";

[Run]
Filename: "{app}\avp32.EXE";   WorkingDir: "{app}"; StatusMsg: "Starting $(PRODUCT_NAME_STR) application..."; Tasks: run; Flags: nowait;

[UninstallDelete] 
Type: files; Name: "$(DATA_BASES)\*.*"

<<KEEP


"$(BUILD)" :
    @if not exist "$(BUILD)/$(NULL)" mkdir "$(BUILD)"

$(FILES_CLEAN): $(M_PRODUCT) INSTALL.MAK 
	-@ erase $(FILES)
	-@ echo cleaned > $@

$(FILES): $(M_PRODUCT) 
	-@ echo ;FILES > $@
	!@ echo Source: "$**"; DestDir: "{app}"; >> $@

$(FILES_BASES): c:\bases\avp.set
	-@ echo ;FILES_BASES > $@
	@ if exist c:\bases\avp.set echo Source: "c:\bases\*"; DestDir: "$(DATA_BASES)"; >> $@


CLEAN:
	-@ erase $(TARGET)

