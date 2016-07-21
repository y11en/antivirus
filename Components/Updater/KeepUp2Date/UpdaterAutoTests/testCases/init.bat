if DEFINED endUpdaterInitScript goto endUpdaterInitScript

REM this script performs common initialization part for all test cases

REM set current path
if "%initScriptPath%"=="" (cd ..)
set initScriptPath=testCases\


REM path to Updater executable
set IUpdaterPath=..\..\..\..\out_win32\debug
REM set IUpdaterPath=..\..\..\..\out\debug


REM set path to cygwin utilities if it is not already
set PATH=cygwin;%PATH%


REM Updater return codes
    set CORE_NO_ERROR=0
    set CORE_FailedToCreateFolder=1
    set CORE_NotEnoughPermissions=2
    set CORE_NoSuchFileOrDirectory=3
    set CORE_NO_SOURCE_FILE=6
    set CORE_URLS_EXHAUSTED=9
    set CORE_NOTHING_TO_UPDATE=10
    set CORE_NotAllComponentsAreUpdated=11
    set CORE_ComponentRejectedByProduct=12
    set CORE_WRONG_KEY=14
    set CORE_ADMKIT_FAILURE=15
    set CORE_INVALID_SIGNATURE=17
    set CORE_GenericFileOperationFailure=18
    set CORE_CANCELLED=19
    set CORE_NO_RESERVED_UPDATE_DESCRIPTION=21
    set CORE_UPDATE_DESCRIPTION_DAMAGED=22
    set CORE_INTERNAL_ERROR=27
    set CORE_DOWNLOAD_ERROR=28
    set CORE_REMOTE_HOST_CLOSED_CONNECTION=29
    set CORE_DOWNLOAD_TIMEOUT=30
    set CORE_FTP_AUTH_ERROR=31
    set CORE_PROXY_AUTH_ERROR=32
    set CORE_CANT_RESOLVE_NAME=33
    set CORE_CANT_CONNECT_ADM_SERVER=34
    set CORE_RETRANSLATION_SUCCESSFUL=35
    set CORE_NO_OPERATION_REQUESTED=36
    set CORE_UpdateSuccessfulRetranslationFailed=37
    set CORE_CANT_CONNECT_INET_SERVER=38
    set CORE_BASE_CHECK_FAILED=39
    set CORE_SERVER_AUTH_ERROR=40
    set CORE_CANT_CONNECT_TO_PROXY=41
    set CORE_CANT_RESOLVE_PROXY=42
    set CORE_AK_WrongReceiverId=43
    set CORE_AK_ServerBusy=44
    set CORE_AK_ConnectionError=45
    set CORE_AK_ConnectionNagentError=46
    set CORE_AK_ConnectionServerError=47
    set CORE_AK_CannotConnectToServer=48
    set CORE_AK_WrongArguments=49
    set CORE_AK_ErrorInOperation=50
    set CORE_AK_UnknownError=51

set endUpdaterInitScript=1

:endUpdaterInitScript

REM clean previous result
call clean.bat
