#ifndef _MC_MAIL_TASK_H__
#define _MC_MAIL_TASK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <CKAH/CKAHUM.h>
#include <CKAH/ckahcr.h>

#include <ProductCore/iface/i_task.h>

#include <Mail/plugin/p_trafficmonitor.h>
// AVP Prague stamp end

#include <map>

#include "../SMTPUnitTests/TestManager.h"

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CTrafficMonitorTask : public cTask {
private:
    // Internal function declarations
    tERROR pr_call ObjectInitDone();
    tERROR pr_call ObjectPreClose();
    tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

    // Property function declarations

public:
    // External function declarations
    tERROR pr_call SetSettings( const cSerializable* p_settings );
    tERROR pr_call GetSettings( cSerializable* p_settings );
    tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
    tERROR pr_call SetState( tTaskRequestState p_state );
    tERROR pr_call GetStatistics( cSerializable* p_statistics );
    tERROR pr_call Process( hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut );

    // Data declaration
    //hREGISTRY                 m_config;     // Настройки (не используются). Не путать с настройками, вдвигаемыми через SetSettings и предоставляемыми через GetSettings
    tDWORD                    m_state;      // Текущее состояние задачи
    hTASKMANAGER              m_tm;         // Указатель на объект TaskManager
    //tDWORD                    m_session_id; // Идентификатор сессии (не используется) при работе в терминальном режиме Windows
    //cTrafficMonitorSettings   m_settings;   // Настройки траффик-монитора
    //cTrafficMonitorStatistics m_statistics; // Статистика трафик-монитора
    // AVP Prague stamp end
    // AVP Prague stamp begin( C++ class declaration end,  )
	TestManager*	m_testManager;
public:
    mDECLARE_INITIALIZATION(CTrafficMonitorTask)
};
// AVP Prague stamp end


#endif