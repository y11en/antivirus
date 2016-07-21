/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvinst_db_con_str.h
 * \author	Andrew Kazachkov
 * \date	14.10.2005 15:49:15
 * \brief	
 * 
 */

#ifndef __KL_SRVINST_DB_CON_STR_H__
#define __KL_SRVINST_DB_CON_STR_H__

namespace KLSRV
{
    std::wstring GetDbConnectionString(KLPAR::ParamsPtr pSettings);
};

#endif //__KL_SRVINST_DB_CON_STR_H__
