////////////////////////////////////////////////////////////////////
//
//  XFlags.h
//  IO flags definitions
//  Generic purpose IO stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __XFLAGS_H__
#define __XFLAGS_H__

//--
typedef enum {
  FIOSO_START   = 0,
  FIOSO_CURRENT = 1,
  FIOSO_END     = 2
} IOS_SEEK_ORIGIN;

//---
#define FIOAM_READ       0x00000008l        // Открывать файл только на чтение
#define FIOAM_WRITE      0x00000004l        // Открывать файл на чтение и запись

#define FIOAM_TEMPORARY  0x00008000l        // Создавать временный файл (уничтожается после записи)

#define FIOAM_RANDOM     0x00000100l        // Случайный доступ в устройстве
#define FIOAM_SEQUENTIAL 0x00000000l        // Последовательный доступ в устройстве

#define FIOCD_CREATE_NEW    		0x00010000l	// Создавать новый файл. Возникает ошибка, если файл уже существует.
#define FIOCD_CREATE_ALWAYS			0x00020000l	// Создавать новый файл. Старый файл уничтожается
#define FIOCD_OPEN_EXISTING			0x00030000l	// Открывать файл. Возникает ошибка, если файла нет
#define FIOCD_OPEN_ALWAYS	  		0x00040000l	// Открывать файл. Если файла нет, то он создается
#define FIOCD_TRUNCATE_EXISTING	0x00050000l	// Открывать файл. После открытия файл обрезается до нулевой длины.  Файла должен открываться с атрибутом FIOAM_WRITE. Возникает ошибка, если файла нет.

#define FIOSM_SHARE_NONE        0x01000000l // Запретить доступ к файлу
#define FIOSM_SHARE_READ        0x02000000l // Разрешить доступ на чтение
#define FIOSM_SHARE_WRITE       0x04000000l // Разрешить доступ на запись


void IOFlagsToWin32Flags( DWORD dwIOFlags, 
												  DWORD *pdwDesiredAccess, 
													DWORD *pdwShareMode, 
													DWORD *pdwCreationDisposition, 
													DWORD *pdwFlagsAndAttributes );

void IOSeekCodeToWin32SeekCode( IOS_SEEK_ORIGIN dwIOSeek, DWORD *pdwWin32Seek );

#endif