// -------- Wednesday,  19 July 2000,  14:33 --------
// Project -- Prague
// Sub project -- Temporary File IO
// Purpose -- Temp Files
// ...........................................
// Author -- Andy Nikishin
// File Name -- tempfile.h
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------



#if !defined( __tempfile_h__06c36d21_be04_4dc8_ad3e_ee4a8a539c0f )
#define __tempfile_h__06c36d21_be04_4dc8_ad3e_ee4a8a539c0f



#include <Prague/iface/i_io.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Plugin identifier )
#define TEMPIO_ID  ((tPID)(PID_TMPFILE))
#define PID_TEMPIO ((tPID)(PID_TMPFILE))
// AVP Prague stamp end( Plugin, Plugin identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin, Vendor identifier )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end( Plugin, Vendor identifier )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Interface comment )
// --------------------------------------------
// --- 74d331e1_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// IO interface implementation
// Short comments -- input/output interface
// Extended comment
//   Defines behavior of input/output of an object
//   Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position. 
// AVP Prague stamp end( IO, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Register call )
#if defined( __cplusplus )
extern "C" 
{
#endif
  tERROR Tempfile_IO_Register( hROOT root);
#if defined( __cplusplus )
}
#endif
// AVP Prague stamp end( IO, Register call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------

/*
#define FIOAM_READ                  0x00000008l // Открывать файл только на чтение
#define FIOAM_WRITE                 0x00000004l // Открывать файл на чтение и запись

#define FIOAM_TEMPORARY             0x00008000l // Создавать временный файл (уничтожается после записи)

#define FIOAM_RANDOM                0x00000100l // Случайный доступ в устройстве
#define FIOAM_SEQUENTIAL            0x00000000l // Последовательный доступ в устройстве

#define FIOCD_CREATE_NEW    		0x00010000l	// Создавать новый файл. Возникает ошибка, если файл уже существует.
#define FIOCD_CREATE_ALWAYS			0x00020000l	// Создавать новый файл. Старый файл уничтожается
#define FIOCD_OPEN_EXISTING			0x00030000l	// Открывать файл. Возникает ошибка, если файла нет
#define FIOCD_OPEN_ALWAYS	  		0x00040000l	// Открывать файл. Если файла нет, то он создается
#define FIOCD_TRUNCATE_EXISTING	    0x00050000l	// Открывать файл. После открытия файл обрезается до нулевой длины.  Файла должен открываться с атрибутом FIOAM_WRITE. Возникает ошибка, если файла нет.

#define FIOSM_SHARE_NONE            0x01000000l // Запретить доступ к файлу
#define FIOSM_SHARE_READ            0x02000000l // Разрешить доступ на чтение
#define FIOSM_SHARE_WRITE           0x04000000l // Разрешить доступ на запись
*/


#define errCREATE_TEMP                 PR_MAKE_IMP_ERR( PID_TEMPIO, 0x000 ) 
#define errDELETE_TEMP                 PR_MAKE_IMP_ERR( PID_TEMPIO, 0x001 )
#define errUPDATE_MAP_VIEW             PR_MAKE_IMP_ERR( PID_TEMPIO, 0x002 )
#define errCOPY_MEM                    PR_MAKE_IMP_ERR( PID_TEMPIO, 0x003 )

#define fOS_TMPFILE_ACCESS_SEQUENTIAL_SCAN     ((tDWORD)(0x00000100L))
#define mTmpACCESS                             ((tDWORD)(0x00000FFFL))

// Messages

#define pmc_TEMPFILE    0xe64494e4
#define pm_TEMPFILE_GETPATH 1  // hSTRING in ctx. Receiver can fill it with TempPath with terminating \\!
#define pm_TEMPFILE_GETNAME 2  // hSTRING in ctx. contains TempPath with terminating \\!

#endif // tempfile_h



