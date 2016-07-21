/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sss_actions.h
 * \author	Andrew Kazachkov
 * \date	30.09.2003 10:47:13
 * \brief	Права доступа модуля SS_SRV.
 * 
 */

#ifndef __KLSSS_ACTIONS_H__
#define __KLSSS_ACTIONS_H__

#define KLAUTH_SS_CONNECT           0x0001  //! Connecting to sss
#define KLAUTH_SS_READ_SECTION      0x0002  //! Method Read
#define KLAUTH_SS_ENUM_SECTIONS     0x0004  //! Method GetNames
#define KLAUTH_SS_CREATE_SECTION    0x0008  //! Method CreateSection
#define KLAUTH_SS_DELETE_SECTION    0x0010  //! Method DeleteSection
#define KLAUTH_SS_APPEND_SECTION    0x0020  //! Method Add
#define KLAUTH_SS_MODIFY_SECTION    0x0040  //! Methods Update, Replace, Clear, Delete

#endif //__KLSSS_ACTIONS_H__
