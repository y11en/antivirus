#////////////////////////////////////////////////////////////////////
#//
#//  _AVP_BUILD.MAK
#//  Project: AVP6
#//  Author:  Graf
#//  Copyright (c) Kaspersky Labs, 2003
#//
#//  Perforce template: Graf-6-Dev
#//
#////////////////////////////////////////////////////////////////////

!include AVP_VERSION.MAK

VER_PRODUCT_H=\CommonFiles\Version\ver_product.h
VER_WXI=\PPP\Install\WIX\KAV2006_Installer\KIS\ver.wxi 
VER_CMD=\PPP\Install\WIX\KAV2006_Installer\KIS\ver.cmd 
AVP_BUILD=\PPP\Install\_AVP_BUILD.mak
AVP_VERSION=\PPP\Install\avp_version.mak
LABEL_TXT=\out\release\label.txt
HIST_TXT=\out\release\hist.txt

!IFNDEF P4
P4=p4
!ENDIF


!IFNDEF DEPOT_PATH
DEPOT_PATH=//depot/KAV/Products/dev
!ENDIF
        
!IF "$(PRODUCT_STATE)"==""
PRODUCT_STATE_SUFFIX_STR=""
!ELSE
PRODUCT_STATE_SUFFIX_STR=" $(PRODUCT_STATE)"
!ENDIF



all: TIME $(VER_PRODUCT_H)
	@copy label_template.txt $(LABEL_TXT)
	$(P4) counter change >>  $(LABEL_TXT)
	$(P4) -x $(LABEL_TXT) sync $(P4_SYNC_FORCE)
	-@echo ------------------------------------------------------------------------------ > $(HIST_TXT)
	-@echo Build $(PRODUCT_VERSION_STR) changelist number: >> $(HIST_TXT)
	-@type $(LABEL_TXT) >> $(HIST_TXT)
	-@echo ------------------------------------------------------------------------------ >> $(HIST_TXT)
	-@perl.exe o:\ppp\install\get_http_content.pl kav%% $(PRODUCT_VERSION_STR) graf a >> $(HIST_TXT)
	

$(VER_PRODUCT_H): AVP_VERSION.MAK
	-@ del /F $(VER_PRODUCT_H)
	-@ del /F $(VER_WXI)
	-@ del /F $(VER_CMD)

	$(P4) edit <<$(VER_PRODUCT_H)
// This file was auto generated with O:\PPP\Install\_AVP_BUILD.MAK
// $(DATE) $(TIME)
#ifndef __VER_PRODUCT_H
#define __VER_PRODUCT_H
#define VER_PRODUCTNAME_STR                 "$(PRODUCT_NAME_STR)"
#define VER_PRODUCTVERSION_HIGH             $(PRODUCT_VERSION_HIGH)
#define VER_PRODUCTVERSION_LOW              $(PRODUCT_VERSION_LOW)
#define VER_PRODUCTVERSION_BUILD            $(PRODUCT_BUILD)
#define VER_PRODUCTVERSION_COMPILATION      $(PRODUCT_COMPILATION)

// -------------------------------------------------------------------------------------------------
#define VER_COMPANY_REGISTRY_PATH			"Software\\KasperskyLab"
#define VER_PRODUCT_REGISTRY_ROOT			VER_COMPANY_REGISTRY_PATH "\\protected"
#define VER_PRODUCT_REGISTRY_NAME			"AVP8"
#define VER_PRODUCT_REGISTRY_PATH			VER_PRODUCT_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_NAME

#define VER_PRODUCT_APPDATA_NAME            "AVP8"
// -------------------------------------------------------------------------------------------------
#define VER_COMPANY_REGISTRY_PATH_W			L"Software\\KasperskyLab"
#define VER_PRODUCT_REGISTRY_ROOT_W			VER_COMPANY_REGISTRY_PATH_W L"\\protected"
#define VER_PRODUCT_REGISTRY_NAME_W			L"AVP8"
#define VER_PRODUCT_REGISTRY_PATH_W			VER_PRODUCT_REGISTRY_ROOT_W L"\\" VER_PRODUCT_REGISTRY_NAME_W

#define VER_PRODUCT_APPDATA_NAME_W          L"AVP8"

#define VER_PRODUCT_REGISTRY_PROT_W			L"KASPERSKYLAB\\PROTECTED"
// -------------------------------------------------------------------------------------------------

#define VER_PRODUCT_REGISTRY_AK_FS			"KAVFS8\\8.0.0.0"
#define VER_PRODUCT_REGISTRY_AK_WKS			"KAVWKS8\\8.0.0.0"
#define VER_PRODUCT_REGISTRY_AK_SOS			"KAVSOS8\\8.0.0.0"

#define VER_PRODUCT_STATE_STR               "$(PRODUCT_STATE)"
#define VER_PRODUCT_STATE_SUFFIX_STR        $(PRODUCT_STATE_SUFFIX_STR)
#define VER_PRODUCT_WMIH_ID                 _g_ver_WmiHlpProdGuid
#define VER_PRODUCT_WMIH_LEGACYREGKEY       "KasperskyAntiVirus"

const unsigned char _g_ver_WmiHlpProdGuid[16] = {0xC6, 0x4B, 0x4D, 0x2C, 0x93, 0x07, 0x56, 0x49, 0xA9, 0xF9, 0xE2, 0x52, 0x43, 0x54, 0x69, 0xC0};

#include "ver_kl.h"
#endif//__VER_PRODUCT_H
<<KEEP

	$(P4) edit <<$(VER_WXI)
<Include>
	<?define ProductVersion="$(PRODUCT_VERSION_STR)" ?>
	<?define ProductBuild="$(PRODUCT_COMPILATION)" ?>
	<?if $$(var.ProductStatus) = "Beta" ?>
		<?define UpdateRoot="index" ?>
		<?define ProductStatusStr=" Beta" ?>
		<?define ProductComments="Version: $(PRODUCT_VERSION_STR) Beta" ?>
	<?elseif $$(var.ProductStatus) = "Release" ?>
		<?define UpdateRoot="index" ?>
		<?define ProductStatusStr="" ?>
		<?define ProductComments="Version: $(PRODUCT_VERSION_STR)" ?>
	<?endif?>
</Include>
<<KEEP

	$(P4) edit <<$(VER_CMD)
@set PRODUCT_VERSION_STR=$(PRODUCT_VERSION_STR)
<<KEEP

	$(P4) edit $(AVP_BUILD)
	$(P4) edit $(AVP_VERSION)

	$(P4) submit -i < <<
Change:	new
Client:	$(P4CLIENT)
User:	$(P4USER)
Status:	new	
Description:
	kav6: make version header $(PRODUCT_VERSION_STR)
Files:
	$(DEPOT_PATH)/PPP/Version/ver_product.h
	$(DEPOT_PATH)/PPP/Install/WIX/KAV2006_Installer/KIS/ver.cmd
	$(DEPOT_PATH)/PPP/Install/WIX/KAV2006_Installer/KIS/ver.wxi
	$(DEPOT_PATH)/PPP/Install/_AVP_BUILD.mak
	$(DEPOT_PATH)/PPP/Install/avp_version.mak
<<


rebuild: TIME CLEANUP all

TIME: 
	-@ time /T
	-@ echo Build $(PRODUCT_VERSION_STR) Setup $(BUILD)...

CLEANUP:
	-@del /F /S /Q ..\..\temp\$(BUILD)\*.*
	-@del /F /S /Q ..\..\temp\$(BUILD)US\*.* 
	-@del /F /S /Q ..\..\Out\$(BUILD)\*.*
