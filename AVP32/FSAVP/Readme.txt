////////////////////////////////////////////////////////////////////
//
//  REARME.TXT
//  NT KMD AVP Engine (FSAVP.SYS) history 
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1997
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
build 132b10: 16.09.99
FIXED:   Handle registration/unregitration

-----------------------------------------------------------------------
build 132b7: 01.09.99
CHANGED: recompiled with new FSAVOID.H (30.6.1999 15:57:09)

-----------------------------------------------------------------------
build 132b6: 01.09.99
FIXED: action code report in case of sector owerwriting(disinfection)

-----------------------------------------------------------------------
build 132b5: 31.08.99

1. Compiler was changed to VC6sp3.(132B1)
2. _all_ modules was recompiled with Maximum speed option.(132B4)
3. VtoolD changed from 2.05b to 3.01(132B4)
4. DriverWorks changed from 1.2 to 2.2(132B4)
5. I've insert SEH handling in AVPBASE.DLL and partial SEH handling in FSAVP.SYS (132B5)


...
CHANGED: Versioninfo revised
...
ADDED: Detection of PP viruses
...
FIXED: BOOT sector disinfection
...
-----------------------------------------------------------------------
build 131: ??.06.99
ADDED: Database Integrity checking
...

...
-----------------------------------------------------------------------
build 126: 25.11.98
CHANGED: Sector access repeat in case of STATUS_VERIFY_REQUIRED.
CHANGED: GetDiskGeometry now never return zeros.

-----------------------------------------------------------------------
build 125: 25.09.98
CHANGED: build No is equal to the AVP engine now.
ADDED:   GK_API 2.0 modules with ODS_ support (BETA, for nt4.0).
         GK_API 1.0 also supported in separate modules.
CHANGED: plugin call SendMessage once at the end of processing, if v. found

build 026a: 23.07.98
CHANGED: Sector I/O changed, Scan ENGINE enhanced.

-----------------------------------------------------------------------
build 026: 01.07.98
CHANGED: Loading Messages from registry. look MESSAGES.INI
ADDED:   Reloading Bases after changing AVP.SET file.
ADDED:   WINDOWS NT3.50 driver

-----------------------------------------------------------------------
build 025: 17.06.98
BUGFIX:  Next Bug that cause PF while SettingsModified proc was fixed.

-----------------------------------------------------------------------
build 024: 16.06.98
ADDED:   Loading Messages from registry. look FSAVP.INI
BUGFIX:  Bug that cause PF while SettingsModified proc was fixed.

-----------------------------------------------------------------------
build 023: 15.06.98
ADDED:   Windows NT 3.51 support.
CHANGED: Minor changes and fixes

-----------------------------------------------------------------------
build 022: 09.06.98
CHANGED: Reload all registry parameters while SttingsModified() call
ADDED:   check backslash in BasePath parameter
ADDED:   additional parameters to GkSendMessage

-----------------------------------------------------------------------
build 021: 28.05.98
ADDED:   LimitCompoundSize registry parameter (in bytes). FSAVP will
         process archives and other compound files only if size less
         then this parameter. if ==0 means process always.
CHANGED: If FilterRegister() fails, FSAVP exits.(FSAVP calls
         FilterRegister() only if GKRegister registry parameter !=0 )

-----------------------------------------------------------------------
build 020: 08.04.98
CHANGED: Checking for STATUS_OPLOCK_BREAK_IN_PROGRESS was added.

-----------------------------------------------------------------------
build 019: 07.04.98
ADDED: GKAPI SendMessage() called in several cases.
CHANGED: Checking on colosing/modifying remote files disabled.
We have a lockups on ZwCreateFile in this situation.


-----------------------------------------------------------------------
build 014: 28.03.98
CHANGED: Recompiled with new HKAPI.H header.
ADDED: PLUGINMESSAGE structure.
ADDED: Registry parameters: Priority, Action, InclusionList, ExclusionList.

-----------------------------------------------------------------------
build 013: 12.03.98
CHANGED: Recompiled with new VERSION header and resources.
CHANGED: Mail & Plain Mail separated from Archives.
CHANGED: Several internal AVP Engine changes.

-----------------------------------------------------------------------
build 012: 10.11.97
CHANGED: AVP_ID = 10002 now
ADDED:   registry StopProcessArchivesAfterDetection key in the "Parameters"

-----------------------------------------------------------------------
build 011: 26.11.97
ADDED:   MBR scan loop at start
ADDED:   cache for last checked files

-----------------------------------------------------------------------
build 010: 20.11.97
recompiled with gk971120
ADDED:   DeviceIoControl interface
ADDED:   Support for international charsets
BUGFIX:  Syncronyzation for multithreading was added.
         ( In previous builds object wasn't checked if engine was busy )
         (QueueMax parameter added to registry: see FSAVP.INI)
BUGFIX:  Some nonsignificant bugs was fixed
CHANGED: Internal processing algorithms
CHANGED: Message Formatting (plain names)
BUGFIX:  Reporting dor compound objects
---
In TO_DO list :
Cache for last checked files
Scanning Absolute sectors on HDD; initial scanning of MBRs.
Detecting TEMP path
Detecting free space on TEMP HD.

-----------------------------------------------------------------------
build 009: 14.11.97
BUGFIX:    Extension check fixed
BUGFIX:    return STATUS_ACCESS_DENIED fixed
BUGFIX:    Limit for unpacking extended to 32M(from 16K :-O ).
ADDED:     reporting about Suspicions and Warinings inside archives

-----------------------------------------------------------------------
build 008: 13.11.97
YES!!!!!! IT WORKS STABLE!!!!
BUGFIX:    Bug with buffer size detection fixed.

-----------------------------------------------------------------------
build 007: 13.11.97
CHANGED:   Recompiled to work with GK971112.
BUGFIX:    Deallocation of CALLBACK structure fixed
KNOWN PROBLEM: BUG in registration/unregistration still present.

-----------------------------------------------------------------------
build 006: 10.11.97
KNOWN PROBLEM: GK registration/unregistration cause serious problems:
after calling regisration/unregistration system became unstable.
It can happens after first registration, but can after third or more.

If in the FSAVP.ini set GKRegister = 0; FSAVP doesn't perform registration,
but loads/unloads databases, and perform test scan of BOOT and command.com.
And it works PERFECTLY.

-----------------------------------------------------------------------
build 005: 04.11.97
BUGFIX:  Problems with several packers was fixed
CHANGED: MountVolume... now returns STATUS_DENY_ACCES if infected.

-----------------------------------------------------------------------
build 004: 03.11.97
ADDED:   Sector scanning (w/o disinfection)
BUGFIX:  Some nonprincipal bugs fixed

-----------------------------------------------------------------------
build 003: 24.10.97
CHANGED: Support for new registration

-----------------------------------------------------------------------
build 002: 02.10.97
BUGFIX:  Wrong virus names in the Warning and Suspic report
BUGFIX:  Corrected "CloseWrongFileHandle" problem (maybe)
CHANGED: Registration Name

-----------------------------------------------------------------------
build 001: 30.09.97
CHANGED: Registration/deregistration calls.
ADDED:   Messaging at the end of object processing.

-----------------------------------------------------------------------
build 000: 26.09.97
COMMENT: Place FSAVP.SYS in any directory,
         Unpack databases in any directory,
         Make corresponded changes of vlues in FSAVP.INI:
              ImagePath
              BasePath
         command: REGINI FSAVP.INI;
         command: net start "F-SECURE AVP".
         Enjoy :-)

