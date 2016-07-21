#ifndef _LOCALIZE_H_06D78A03_ACDA_40a4_8422_B1B74C428AB4
#define _LOCALIZE_H_06D78A03_ACDA_40a4_8422_B1B74C428AB4

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

namespace KLUPD {

// Definition of error codes returning by interface functions
// Warning: code numbers should not be changed to make documentation consistent
// Note: code numbers are explicity used for convenience,
//   because support team uses this file for trace file analysis.
enum CoreError
{
    // Success
    CORE_NO_ERROR = 0,

    // Failed to create folder
    CORE_FailedToCreateFolder = 1,

    // Not enough permissions
    CORE_NotEnoughPermissions = 2,

    // No such file or directory
    CORE_NoSuchFileOrDirectory = 3,

    // File does not exist on update source
    CORE_NO_SOURCE_FILE = 6,

    // deprecate code for Administration Kit
    DEPRECATE_7 = 7,

    // deprecate code for Administration Kit
    DEPRECATE_8 = 8,

    // No source contains valid update files or source list is not configured
    CORE_URLS_EXHAUSTED = 9,

    // All files are up-to-date
    CORE_NOTHING_TO_UPDATE = 10,

    // Not all components are updated
    CORE_NotAllComponentsAreUpdated = 11,

    // Component installation is rejected by product
    CORE_ComponentRejectedByProduct = 12,


    // Black list check failed
    CORE_WRONG_KEY = 14,

    // Incorrect product configuration
    CORE_ADMKIT_FAILURE = 15,

    // Invalid file signature
    CORE_INVALID_SIGNATURE = 17,

    // Generic file operation failure
    CORE_GenericFileOperationFailure = 18,

    // Operation canceled
    CORE_CANCELLED = 19,

    // deprecate code for Administration Kit
    DEPRECATE_20 = 20,

    // Files to perform rollback operation absent or damaged
    CORE_NO_RESERVED_UPDATE_DESCRIPTION = 21,

    // Index file damaged. File is not valid XML structure or does not exist
    CORE_UPDATE_DESCRIPTION_DAMAGED = 22,

    // Updater logic error
    CORE_INTERNAL_ERROR = 27,

    // Download error
    CORE_DOWNLOAD_ERROR = 28,

    // Connection has been closed by remote host
    CORE_REMOTE_HOST_CLOSED_CONNECTION = 29,

    // Network operation timeout expired
    CORE_DOWNLOAD_TIMEOUT = 30,

    // Failed to authorize on FTP server
    CORE_FTP_AUTH_ERROR = 31,

    // Failed to authorize on proxy server
    CORE_PROXY_AUTH_ERROR = 32,

    // Failed to resolve source DNS name
    CORE_CANT_RESOLVE_NAME = 33,

    // Failed to connect to Administration Server
    CORE_CANT_CONNECT_ADM_SERVER = 34,

    // Retranslation successful and update is not requested
    CORE_RETRANSLATION_SUCCESSFUL = 35,

    // Neither update nor retranslation is requested
    CORE_NO_OPERATION_REQUESTED = 36,

    // Update successful, but retranslation failed
    CORE_UpdateSuccessfulRetranslationFailed = 37,

    // Connection to source can not be established
    CORE_CANT_CONNECT_INET_SERVER = 38,

    // Application aborted update after downloaded bases check
    CORE_BASE_CHECK_FAILED = 39,

    // Authorization on update source failed
    CORE_SERVER_AUTH_ERROR = 40,

    // Failed to establish connection to proxy server
    CORE_CANT_CONNECT_TO_PROXY = 41,

    // Proxy server DNS name resolution error
    CORE_CANT_RESOLVE_PROXY = 42,

    // Connection to Administration Server failed. Invalid Administration Kit Transport identifier: either receiver was deleted or conneñt was already called for this receiver
    CORE_AK_WrongReceiverId = 43,
    // Connection to failed, Administration Server is busy and can not handle requests now
    CORE_AK_ServerBusy = 44,
    // Connection failed. Physical connection to Administration Server error
    CORE_AK_ConnectionError = 45,
    // Connection to Administration Kit network agent physical error
    CORE_AK_ConnectionNagentError = 46,
    // Connection to Master Administration Server physical error
    CORE_AK_ConnectionServerError = 47,
    // Connection to Administration Server failed, unknown error
    CORE_AK_CannotConnectToServer = 48,
    // Failed to receive file from Administration Server, invalid arguments for transport
    CORE_AK_WrongArguments = 49,
    // Failed to receive file, Administration Kit transport file receive error
    CORE_AK_ErrorInOperation = 50,
    // Failed to receive file, unknown Administration Kit transport receive operation error
    CORE_AK_UnknownError = 51,


    // Update source is selected
    EVENT_SOURCE_SELECTED = 100,

    // Administration Server source is selected
    EVENT_ADMIN_KIT_SOURCE_SELECTED = 101,

    // Proxy server is selected
    EVENT_PROXY_SERVER_SELECTED = 102,

    // File download started
    EVENT_DOWNLOAD_FILE_STARTED = 103,

    // File downloaded
    EVENT_FILE_DOWNLOADED = 104,

    // File installed
    EVENT_NEW_FILE_INSTALLED = 105,

    // File updated
    EVENT_FILE_UPDATED = 107,

    // File rolled back
    EVENT_FILE_ROLLED_BACK = 108,

    // deprecate code for Administration Kit
    DEPRECATE_109 = 109,

    // Task started event
    EVENT_TaskStarted = 110,

    // Started installation files for update
    EVENT_StartInstallFilesForUpdate = 112,

    // Started installation files for retranslation
    EVENT_StartInstallFilesForRetranslation = 113,

    // Started coping files for rollback
    EVENT_StartCopyFilesForRollback = 114,

    // deprecate code for Administration Kit
    DEPRECATE_115 = 115,

    // DNS name resolved
    EVENT_DNS_NAME_RESOLVED = 116,

    // Component is not updated
    EVENT_ComponentIsNotUpdated = 117,

    // Generate list of files to download
    EVENT_GeneratingFileListToDownload = 118,

	EVENT_ComponentIsNotRetranslated = 119
};


struct SErrorCode_LocalizedDescription
{
    CoreError m_code;
    const char *m_description;
};

static SErrorCode_LocalizedDescription g_errorCodeLocalizedDescriptions[] = {
    { CORE_NO_ERROR,                  "Success" },
    { CORE_FailedToCreateFolder,      "Failed to create folder" },
    { CORE_NotEnoughPermissions,      "Not enough permissions" },
    { CORE_NoSuchFileOrDirectory,     "No such file or directory" },
    { CORE_NO_SOURCE_FILE,            "File does not exist on update source" },
    { CORE_URLS_EXHAUSTED,            "No source contains valid update files or source list is not configured" },
    { CORE_NOTHING_TO_UPDATE,         "All files are up-to-date" },
    { CORE_NotAllComponentsAreUpdated,"Not all components are updated" },
    { CORE_ComponentRejectedByProduct,"Component installation is rejected by product" },
    { CORE_WRONG_KEY,                 "Black list check failed" },
    { CORE_ADMKIT_FAILURE,            "Incorrect product configuration" },
    { CORE_INVALID_SIGNATURE,         "Invalid file signature" },
    { CORE_GenericFileOperationFailure, "File operation failure" },
    { CORE_CANCELLED,                 "Operation canceled" },
    { CORE_NO_RESERVED_UPDATE_DESCRIPTION, "Files to perform rollback operation absent or damaged" },
    { CORE_UPDATE_DESCRIPTION_DAMAGED,     "Index file damaged. File is not valid XML structure or does not exist" },
    { CORE_INTERNAL_ERROR,            "Updater logic error" },
    { CORE_DOWNLOAD_ERROR,            "Download error" },
    { CORE_REMOTE_HOST_CLOSED_CONNECTION,   "Connection has been closed by remote host" },
    { CORE_DOWNLOAD_TIMEOUT,          "Network operation timeout expired" },
    { CORE_FTP_AUTH_ERROR,            "Failed to authorize on FTP server" },
    { CORE_PROXY_AUTH_ERROR,          "Failed to authorize on proxy server" },
    { CORE_CANT_RESOLVE_NAME,         "Failed to resolve source DNS name" },
    { CORE_CANT_CONNECT_ADM_SERVER,   "Failed to connect to Administration Server" },
    { CORE_RETRANSLATION_SUCCESSFUL,  "Retranslation successful and update is not requested" },
    { CORE_NO_OPERATION_REQUESTED,    "Neither update nor retranslation is requested" },
    { CORE_UpdateSuccessfulRetranslationFailed, "Update successful, but retranslation failed" },
    { CORE_CANT_CONNECT_INET_SERVER,  "Connection to source can not be established" },
    { CORE_BASE_CHECK_FAILED,         "Application aborted update after downloaded bases check" },
    { CORE_SERVER_AUTH_ERROR,         "Authorization on update source failed" },
    { CORE_CANT_CONNECT_TO_PROXY,     "Failed to establish connection to proxy server" },
    { CORE_CANT_RESOLVE_PROXY,        "Proxy server DNS name resolution error" },
    { CORE_AK_WrongReceiverId,        "Connection to Administration Server failed. Invalid Administration Kit Transport identifier: either receiver was deleted or conneñt was already called for this receiver" },
    { CORE_AK_ServerBusy,             "Connection to failed, Administration Server is busy and can not handle requests now" },
    { CORE_AK_ConnectionError,        "Connection failed. Physical connection to Administration Server error" },
    { CORE_AK_ConnectionNagentError,  "Connection to Administration Kit network agent physical error" },
    { CORE_AK_ConnectionServerError,  "Connection to Master Administration Server physical error" },
    { CORE_AK_CannotConnectToServer,  "Connection to Administration Server failed, unknown error" },
    { CORE_AK_WrongArguments,         "Failed to receive file from Administration Server, invalid arguments for transport" },
    { CORE_AK_ErrorInOperation,       "Failed to receive file, Administration Kit transport file receive error" },
    { CORE_AK_UnknownError,           "Failed to receive file, unknown Administration Kit transport receive operation error" },


    
    { EVENT_SOURCE_SELECTED,            "Update source is selected" },
    { EVENT_ADMIN_KIT_SOURCE_SELECTED,  "Administration Server source is selected" },
    { EVENT_PROXY_SERVER_SELECTED,      "Proxy server is selected" },
    { EVENT_DOWNLOAD_FILE_STARTED,      "Download file started" },
    { EVENT_FILE_DOWNLOADED,            "File downloaded" },
    { EVENT_NEW_FILE_INSTALLED,         "New file installed" },
    { EVENT_FILE_UPDATED,               "File updated" },
    { EVENT_FILE_ROLLED_BACK,           "File rolled back" },
    { EVENT_TaskStarted,                "Task started event" },
    { EVENT_StartInstallFilesForUpdate, "Started installation files for update" },
    { EVENT_StartInstallFilesForRetranslation, "Started installation files for retranslation" },
    { EVENT_StartCopyFilesForRollback,  "Started coping files for rollback" },
    { EVENT_DNS_NAME_RESOLVED,          "DNS name resolved" },
    { EVENT_ComponentIsNotUpdated,      "Component is not updated" },
    { EVENT_GeneratingFileListToDownload, "Generate list of files to download" },
	{ EVENT_ComponentIsNotRetranslated,  "Component is not retranslated" },
};

// return true if error related to network problem, otherwise false
inline bool isNetworkError(const CoreError &result)
{
    return result == CORE_PROXY_AUTH_ERROR
        || result == CORE_SERVER_AUTH_ERROR
        || result == CORE_CANT_RESOLVE_NAME
        || result == CORE_CANT_RESOLVE_PROXY
        || result == CORE_CANT_CONNECT_INET_SERVER
        || result == CORE_CANT_CONNECT_TO_PROXY
        || result == CORE_DOWNLOAD_TIMEOUT
        || result == CORE_DOWNLOAD_ERROR
        || result == CORE_REMOTE_HOST_CLOSED_CONNECTION
        || result == CORE_CANT_CONNECT_ADM_SERVER
        || result == CORE_AK_WrongReceiverId
        || result == CORE_AK_ServerBusy
        || result == CORE_AK_ConnectionError
        || result == CORE_AK_ConnectionNagentError
        || result == CORE_AK_ConnectionServerError
        || result == CORE_AK_CannotConnectToServer
        || result == CORE_AK_WrongArguments
        || result == CORE_AK_ErrorInOperation
        || result == CORE_AK_UnknownError;
}

// return true if switch to next source is needed
inline bool criticalErrorForDifferenceFile(const CoreError &result)
{
    // user requested immediate stop
    if(result == CORE_CANCELLED)
        return true;

    // other then network error are not fatal for download, it can be internal file operation error or whatever
    if(!isNetworkError(result))
        return false;

    // all network errors are fatal, but few (stay on current source):
    return
        // 1) difference file may have extension "*.mp3, *.exe" and being non-compatible with user firewall policy
            result != CORE_PROXY_AUTH_ERROR
            && result != CORE_SERVER_AUTH_ERROR
            && result != CORE_REMOTE_HOST_CLOSED_CONNECTION
        // 2) internal download errors are not critical, try download file with no difference mechanism
            && result != CORE_DOWNLOAD_ERROR
            && result != CORE_AK_UnknownError;
}

inline bool badFile(const CoreError &result)
{
    return result == CORE_INVALID_SIGNATURE || result == CORE_UPDATE_DESCRIPTION_DAMAGED;
}

inline bool absentFile(const CoreError &result)
{
    return result == CORE_NO_SOURCE_FILE
        || result == CORE_NotEnoughPermissions
        || result == CORE_NoSuchFileOrDirectory;
}


// return true if result does not mean error
inline bool isSuccess(const CoreError &result)
{
    return result == CORE_NO_ERROR
        || result == CORE_NOTHING_TO_UPDATE
        || result == CORE_RETRANSLATION_SUCCESSFUL
        || result == CORE_UpdateSuccessfulRetranslationFailed
        || result == CORE_NotAllComponentsAreUpdated
        || result == CORE_ComponentRejectedByProduct;
}

// there is no 'success' type codes in Prague, that is why 'success' and 'warning'
//  are treated similarly. For error codes the function returns false
inline bool isSuccessOrWarningForPrague(const CoreError &code)
{
    if(100 <= code)
        return true;

    return isSuccess(code)
        || code == CORE_CANCELLED;
}

// return indicates that code is not error, but operation was not performed
inline bool isNoOperationRequestedCode(const CoreError &code)
{
    return code == CORE_NOTHING_TO_UPDATE
        || code == CORE_NO_OPERATION_REQUESTED;
}

}   // namespace KLUPD


// deprecete definition for Administraiton Kit (TODO: move to Administraion Kit headers)
#define CORE_CURRENT_VERSION_NEWER 20


#endif  // #ifndef _LOCALIZE_H_06D78A03_ACDA_40a4_8422_B1B74C428AB4
