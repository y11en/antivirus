/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	sssoapcommon.h
 * \author	Mikhail Karmazine
 * \date	14:30 20.11.2002
 * \brief	common declarations for soap calls if SettingStorage interface.
 */

#ifndef __SSSOAPCOMMON_H__6C7B23EC_8E25_4eb3_8410_58667A82E48D__
#define __SSSOAPCOMMON_H__6C7B23EC_8E25_4eb3_8410_58667A82E48D__

// operations for klpkg_SS_Write:
#define SS_WO_UPDATE	1
#define SS_WO_ADD		2
#define SS_WO_REPLACE	3
#define SS_WO_CLEAR		4	
#define SS_WO_DELETE	5

// operations for klpkg_SS_SectionOpearation:
#define SS_SO_CREATE	1
#define SS_SO_DELETE	2

#endif // __SSSOAPCOMMON_H__6C7B23EC_8E25_4eb3_8410_58667A82E48D__
