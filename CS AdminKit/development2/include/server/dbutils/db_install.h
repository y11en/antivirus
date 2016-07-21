/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	db_install.h
 * \author	Andrew Kazachkov
 * \date	21.02.2006 12:23:01
 * \brief	
 * 
 */

#include <server/srv_common.h>

#ifndef __DB_INSTALL_H__
#define __DB_INSTALL_H__

typedef enum
{
    DbiPt_Proportional = 0,
    DbiPt_Exponential = 1    
}DbInst_ProgressType;

// BEGIN API DECLARATIONS


/*!
  \brief	Creates DB connection

  \param	szwServerType   IN  "MSSQLSRV" for SQL Server or "MYSQL" for MySQL
  \param	szwLogin        IN  username ("" if windows authentication)
  \param	szwPassword     IN  password ("" if windows authentication)
  \param	szwServer       IN  server instance
  \param	nPort           IN  port (MySQL only)
  \param	szwDb           IN  database name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_Create(   
                        const wchar_t*  szwServerType,
                        const wchar_t*  szwLogin,
                        const wchar_t*  szwPassword,
                        const wchar_t*  szwServer,
                        unsigned        nPort,
                        const wchar_t*  szwDb);

/*!
  \brief	Save db settings to settings storage

  \param	szwServerType   IN  "MSSQLSRV" for SQL Server or "MYSQL" for MySQL
  \param	szwLogin        IN  username ("" if windows authentication)
  \param	szwPassword     IN  password ("" if windows authentication)
  \param	szwServer       IN  server instance
  \param	nPort           IN  port (MySQL only)
  \param	szwDb           IN  database name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_SaveSettings(
                        const wchar_t*  szwServerType,
                        const wchar_t*  szwLogin,
                        const wchar_t*  szwPassword,
                        const wchar_t*  szwServer,
                        unsigned        nPort,
                        const wchar_t*  szwDb);


/*!
  \brief	Creates DB connection using settings from settings storage

  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_CreateFromSettings();

/*!
  \brief	Destroys connection to DB server
*/
KLCSSRV_DECL void __stdcall KLDBINST_Destroy();


/*!
  \brief	KLDBINST_GetErrorMessage

  \return	error message
*/
KLCSSRV_DECL const char* __stdcall KLDBINST_GetErrorMessage();


/*!
  \brief	Executes SQL-script specified by nID

  \param	nID IN resourceID
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_ExecScriptFromResource(
                UINT        nID, 
                void*       hProgressWnd, 
                unsigned    lFlags);


/*!
  \brief	Executes SQL-script specified in szString

  \param	szwString  IN script
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/

KLCSSRV_DECL int __stdcall KLDBINST_ExecScriptFromString(const char* szString);

/*!
  \brief	Creates DATABASE

  \param	szDB IN database name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_CreateDB(const wchar_t* szDB);

/*!
  \brief	Drops database

  \param	szDB IN database name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_DropDB(const wchar_t* szDB);

/*!
  \brief	Makes specified database current

  \param	szDB IN database name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_UseDB(const wchar_t* szDB);


/*!
  \brief	Stores localization strings into database

  \param	szwDllName IN localization DLL name
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_LoadLocStringsToDB(const wchar_t* szwDllName);


/*!
  \brief	Returns db type string -- "MSSQLSRV" for SQL Server or "MYSQL" for MySQL

  \return	returns type string or empty string if no DB connection exists
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL const wchar_t* __stdcall KLDBINST_GetDbType();

/*!
  \brief	Returns used db name

  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_GetCurrentDb(VARIANT* OUT pDbName);

/*!
  \brief	Returns SQL Server version info

  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_GetSqlServerVersionInfo(
                                VARIANT* OUT pvarVersion,
                                VARIANT* OUT pvarLevel,
                                VARIANT* OUT pvarEdition);


/*!
  \brief	Checks specified database existence. 

  \param	szDB in database name
  \param	pnIfExists OUT pointer to result. Zero if exists, non-zero otherwise
  \return	0 if success, error code otherwise
            Use KLDBINST_GetErrorMessage() to acquire full error info. 
*/
KLCSSRV_DECL int __stdcall KLDBINST_IfDatabaseExists(
                                const wchar_t*  szDB, 
                                long*           pnIfExists);


/*!
  \brief	Backs up administration server data

  \param	szwInstallDir IN Administration Server installation directory
  \param	szwBackupPath IN    directory where to save backup files
  \param	bUseCurrentTime IN  By specifying this flag destination directory will
                                be in form "szwBackupPath\klbackup YYYY-MM-DD # HH-MM-SS". 
  \param	szwPassword IN      password, cannot be empty.
  \return	0 if success, error code otherwise. 
            Use KLDBINST_GetErrorMessage() to acquire full error info. 

            Following codes are possible:
            -3  Cannot open SCM
            -4  Cannot stop services
            -5  Cannot start services
            -6  Invalid backup path
            -7  Destrination folder is not empty
            -8  Backing up DB failed: check whether SQL Server has enough permissions to access backup path.
            -10 backup is already running
*/
KLCSSRV_DECL int __stdcall KLDBINST_BackupAdmServer(
                                const wchar_t*  szwInstallDir,
                                const wchar_t*  szwBackupPath,
                                LONG            bUseCurrentTime,
                                const wchar_t*  szwPassword,
                                void*           hProgressWnd);

/*!
  \brief	For internal use only
*/
KLCSSRV_DECL KLDB::DbConnectionPtr __stdcall KLDBINST_GetConnectionI();

/*!
  \brief	For internal use only
*/
KLCSSRV_DECL int __stdcall KLDBINST_SetResourceInstance(const wchar_t* szwDll);

/*!
  \brief	For internal use only
*/
KLCSSRV_DECL KLPAR::ParamsPtr __stdcall KLDBINST_CreateSettingsI(
                        const wchar_t*  szwServerType,
                        const wchar_t*  szwLogin,
                        const wchar_t*  szwPassword,
                        const wchar_t*  szwServer,
                        unsigned        nPort,
                        const wchar_t*  szwDb);

/*!
  \brief	For internal use only
*/
KLCSSRV_DECL KLSTD::CAutoPtr<KLERR::Error> __stdcall KLDBINST_GetLastErrorI();

#endif //__DB_INSTALL_H__
