#ifndef __KLBACKUP_H__
#define __KLBACKUP_H__

/*!
    klbackup cmdline interface
*/

//!common cmdline parameters
#define KLBKP_LOGFILE_PATH                  L"logfile"
#define KLBKP_BACKUP_PATH                   L"path"
#define KLBKP_RESTORE_FLAG                  L"restore"
#define KLBKP_USE_SUBFOLDER_WITH_TS_FLAG    L"use_ts"

//! password
#define KLBKP_PASSWORD                      L"password"

//! deprecated
#define KLBKP_SAVE_CERT_FLAG                L"savecert"


/*
    !task start cmdline parameters
*/
//!This flag means that klbackup was started from task
#define KLBKP_TASK_START                    L"task"

//!encrypted password
#define KLBKP_TASK_SAVE_CERT_PASSWORD       L"pswdenc"

//!result file name
#define KLBKP_TASK_RESULTS_FILE             L"rsltfile"

/*
    Command line example
    -task -rsltfile "c:\klbackup\results.txt" -logfile "c:\klbackup\klbackup.log" -path "c:\klbackup" -pswdenc FA23563BE569484930394949943949
*/

//! Result file data
#define KLBKP_TASK_RES_SECTION	        L"result"

//! Error code (number)
#define KLBKP_TASK_RES_CODE		        L"code"

//! Exception info (string)
#define KLBKP_TASK_RES_EXCPT            L"excpt"

#define KLBKP_RESULT_FROM_AK(x)    \
                (0xE0FF0000L | ((x) & 0x0000FFFF))

#define KLBKP_EMPTY_PASSWORD    L"52079A03-6DAA-46f0-80A4-0F55D44B2FEB"

enum
{
    KLBKP_RC_OK = 0,

    //! Commandline is invalid
    KLBKP_RC_INVALID_CMDLINE = 1,

    //! Cannot decrypt password
    KLBKP_RC_INVALID_PASSWORD = 2,
    
    //! Cannot open SCM
    KLBKP_RC_FAILED_OPEN_SCM = 3,

    //! Cannot stop service
    KLBKP_RC_FAILED_STOP_SERVICE = 4,

    //! Cannot start service
    KLBKP_RC_FAILED_START_SERVICE = 5,

    //! Invalid backup path
    KLBKP_RC_INVALID_DST_PATH = 6,

    //! Destrination folder is not empty
    KLBKP_RC_DST_NOT_EMPTY = 7,

    //! Backing up DB failed: check whether SQL Server has enough permissions to access backup path.
    KLBKP_RC_DB_BACKUP_FAILED = 8,

    //! Unexpected error
    KLBKP_RC_UNEXPECTED = 9,

    //! klbackup is already running
    KLBKP_RC_ALREADY_RUNNING = 10
};

#endif //__KLBACKUP_H__
