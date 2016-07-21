/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file tsk/RICommon.h
 * \author јндрей Ћащенков
 * \date 14:30 21.03.2005
 * \brief ќбщие константы, используемые подсистемой удаленной инсталл€ции.
 */

#ifndef __TSK_RI_COMMON__
#define __TSK_RI_COMMON__

#ifndef  _WIN32
#define TEXT(quote) L##quote
#endif

#define KLTSK_RI_WRAPPER_FILE_NAME				TEXT("klriwrap.ex_")
#define KLTSK_RI_WRAPPER_KPD_FILE				TEXT("setup.kpd")
#define KLTSK_RI_SETUP_EXE_FILE					TEXT("setup.exe")
#define KLTSK_RI_SETUP_LOG_FILE					TEXT("setup.log")
#define KLTSK_RI_SETUP_AVP_FILE					TEXT("setup.avp")
#define KLTSK_RI_SETUP_REPORT_FILE				TEXT("avpsetup.rep")
#define KLTSK_RI_SECTION_RESPONSE_RESULT		TEXT("ResponseResult")
#define KLTSK_RI_KEY_FIRST_RESULT_CODE			TEXT("ResultCode")
#define KLTSK_RI_KEY_PROCESS_RESULT_CODE		TEXT("ProcessResultCode")
#define KLTSK_RI_KEY_SCRIPT_RESULT_CODE			TEXT("ScriptResultCode")
#define KLTSK_RI_KEY_REBOOT_ZOMBIE_FILE			TEXT("RebootZombieFile")
#define KLTSK_RI_RESULT_CODE_OK					TEXT("0")
#define KLTSK_RI_RESULT_NCODE_OK				0
#define KLTSK_RI_SCRIPT_RESULT_CODE_WARNING		TEXT("-50")
#define KLTSK_RI_SCRIPT_RESULT_NCODE_WARNING	(-50)
#define KLTSK_RI_SCRIPT_RESULT_CODE_ERROR		TEXT("-100")
#define KLTSK_RI_SCRIPT_RESULT_NCODE_ERROR		(-100)

#define KLTSK_RI_KLRBTAGT_FILE_NAME				TEXT("klrbtagt.exe")
#define KLTSK_RI_KLRBTAGT_INI_FILE_NAME			TEXT("klrbtagt.ini")

#define KLTSK_RI_KLRBTAGT_INI_FILE_BASE			TEXT("klrbtagt")
#define KLTSK_RI_CFG_TASK_ID					TEXT("TaskId")

#define KLRI_PROC_GENERAL_ERROR -1
#define KLRI_PROC_INVALID_CMD_LINE -4

#define KLRI_RSLT_NO_ERROR          0
#define KLRI_RSLT_GENERAL_ERROR     -1
#define KLRI_RSLT_FILE_NOT_EXISTS   -5

#endif // __TSK_RI_COMMON__
