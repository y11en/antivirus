#////////////////////////////////////////////////////////////////////
#//
#//  INSTALL.MAK
#//  Inno Setup 5.0 Installer makefile
#//  Project: KIS 2006
#//  Author:  Graf
#//  Copyright (c) Kaspersky Labs, 2005
#//
#////////////////////////////////////////////////////////////////////

!include AVP_VERSION.MAK

OUT=..\..\Out\$(BUILD)

TARGET: 
    -@ echo --------------------Configuration: $@ - Win32 $(BUILD)--------------------
	 @ xcopy \prague\release\klif.* $(OUT) /Q /R /Y
	 @ xcopy \windows\hook\release\klop.* $(OUT) /Q /R /Y
    -@ rar a -ep -idp -m1 $(OUT)\PDB.$(PRODUCT_VERSION_STR).rar $(OUT)\*.pdb $(OUT)\*.ppl $(OUT)\*.dll $(OUT)\*.exe $(OUT)\*.com $(OUT)\*.sys $(OUT)\*.vxd $(OUT)\*.tlb

