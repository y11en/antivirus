#////////////////////////////////////////////////////////////////////
#//
#//  README.TXT
#//  KLE 3.0 Engine makefile
#//  Project KLE
#//  Alexey de Mont de Rique [graf@kaspersky.com] 
#//  Copyright (c) Kaspersky Labs. 2001
#//
#////////////////////////////////////////////////////////////////////

DEBUG/RELEASE folders contain modules:
	KLE_APP.EXE - test application for KL Engine (both NT and 9X). Usage: KLE_APP filename.
	KLE_SYS.SYS - KL engine for NT/2k
	KLH_SYS.SYS - test helper driver to access KLE_SYS. It is an example of usage KLE_API.
	KLE_VXD.VXD - KL engine for Win 9X
	KLH_VXD.VXD - test helper driver to access KLE_VXD. It is an example of usage KLE_API.

KLE folder contains main KLEAPI.H header with all function prototypes and definitions.
    it also contains main workspace file for KLE_APP debugging.

KLE_APP folder contains sources for KLE_APP and helper drivers modules. Example of usage KLE_API.
!ATTENTION! this package doesn't contain antiviral databases. These files are on ftp://ftp.kaspersky.com
Bases should be installed to the same folder as executables, or source(KLE_APP.CPP) should be corrected.

SYS Device drivers should be registered. (Use: REGINI kle_sys.ini and REGINI klh_sys.ini)

--------------------------------------------------------------------------------
Build 3321 (API version 0x00010006)
NT/2K - KLE driver with requested functionality implemented except multithread scanning (just multithread-safe). 
9x/ME - KLE driver with requested functionality implemented except multithread scanning (just multithread-safe). 

--------------------------------------------------------------------------------
Build 3314 (API version 0x00010005)
NT/2K - KLE driver with requested functionality implemented except multithread scanning (just multithread-safe). 
9x/ME - not started yet.