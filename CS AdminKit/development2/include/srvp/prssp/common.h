/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file    prssp/common.h
 * \author  Mikhail Karmazine
 * \date    15.04.2003 14:29:49
 * \brief   Общие декларации для клинетской и серверной частей модуля KLPRSSP
 * 
 */


#ifndef __PRSSP_COMMON_H__
#define __PRSSP_COMMON_H__

// !brief операции над SettingStorage, которые имеют общий интерфейс:
//   Update, Add, Replace, Clear, Delete
#define PRSSP_WO_UPDATE    1
#define PRSSP_WO_ADD       2
#define PRSSP_WO_REPLACE   3
#define PRSSP_WO_CLEAR     4    
#define PRSSP_WO_DELETE    5

// !brief операции над SettingStorage, которые имеют общий интерфейс:
//   CreateSection, DeleteSection
#define PRSSP_SO_CREATE    1
#define PRSSP_SO_DELETE    2

#endif //__PRSSP_COMMON_H__