// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  17 November 2004,  15:04 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AVP1
// Purpose     -- AVP3 low level IO library (obsolete)
// Author      -- Graf
// File Name   -- plugin_l_llio.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_l_llio )
#define __public_plugin_l_llio
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_L_LLIO  ((tPID)(58900))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_l_llio
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&Read13, PID_L_LLIO, 0x149f655cl },
{ (tDATA)&Write13, PID_L_LLIO, 0x7a1ae915l },
{ (tDATA)&Read25, PID_L_LLIO, 0xd6d193aal },
{ (tDATA)&Write26, PID_L_LLIO, 0x215d4e59l },
{ (tDATA)&GetDiskParam, PID_L_LLIO, 0xb17fffacl },
{ (tDATA)&GetFirstMcbSeg, PID_L_LLIO, 0x6f205269l },
{ (tDATA)&GetDosMemSize, PID_L_LLIO, 0x6d25f920l },
{ (tDATA)&GetIfsApiHookTable, PID_L_LLIO, 0x859c4886l },
{ (tDATA)&GetDosTraceTable, PID_L_LLIO, 0x9c324cd0l },
{ (tDATA)&MemoryRead, PID_L_LLIO, 0x0323b6dal },
{ (tDATA)&MemoryWrite, PID_L_LLIO, 0xc820c199l },
{ (tDATA)&ExecSpecial, PID_L_LLIO, 0x7f2c7b9fl },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tBOOL IMPEX_FUNC(Read13)( tBYTE p_Disk, tWORD p_Sector, tBYTE p_Head, tWORD p_NumSectors, tBYTE* p_Buffer )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(Write13)( tBYTE p_Disk, tWORD p_Sector, tBYTE p_Head, tWORD p_NumSectors, tBYTE* p_Buffer )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(Read25)( tBYTE p_Drive, tDWORD p_Sector, tWORD p_NumSectors, tBYTE* p_Buffer )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(Write26)( tBYTE p_Drive, tDWORD p_Sector, tWORD p_NumSectors, tBYTE* p_Buffer )IMPEX_INIT;
IMPEX_DECL tBOOL IMPEX_FUNC(GetDiskParam)( tBYTE p_disk, tBYTE p_drive, tWORD* p_CX, tBYTE* p_DH )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetFirstMcbSeg)()IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetDosMemSize)()IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetIfsApiHookTable)( tDWORD* p_table, tDWORD p_size )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetDosTraceTable)( tDWORD* p_table, tDWORD p_size )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(MemoryRead)( tDWORD p_dwOffset, tDWORD p_dwSize, tBYTE* p_lpBuffer )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(MemoryWrite)( tDWORD p_dwOffset, tDWORD p_dwSize, tBYTE* p_lpBuffer )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ExecSpecial)( hOBJECT p__this, tDWORD* p_result, tSTRING p_FuncName, tDWORD p_wParam, tDWORD p_lParam )IMPEX_INIT;
#endif
// AVP Prague stamp end



