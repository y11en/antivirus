// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  03 September 2007,  15:39 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__616c8a8a_136f_4a56_a571_f73a61cb1957 )
#define __task_cpp__616c8a8a_136f_4a56_a571_f73a61cb1957
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <iface/i_reg.h>
#include <plugin/p_klstatistics.h>
// AVP Prague stamp end



#include <Updater/updater.h>

#include <iface/i_io.h>
#include <iface/i_threadpool.h>
#include <iface/i_csect.h>
#include <iface/i_token.h>
#include <i_updaterconfigurator.h>

#include <structs/s_avs.h>
#include <ahtasks/structs/s_ids.h>

#include <structs/s_statistics.h>

#include "../../Components/Updater/Transport/PragueTransport/i_transport.h"



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CKLStatistics : public cTask, public cThreadPoolBase, public cThreadTaskBase
{
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

// Data declaration
    hTASKMANAGER m_hTM;         // --
    tDWORD       m_dwTaskState; // --
// AVP Prague stamp end



    CKLStatistics() ;

    tERROR InitPostTransport() ;

    tERROR PostFile(const char*          p_pszServerFileName, // example - statistics/12ef.txt
                    const unsigned char* p_pBuffer,
                    int                  p_nBufferSize) ;

    tERROR OnUpdateEnd(cUpdaterEvent* p_pEvent) ;

    tERROR OnDetectObject(cDetectObjectInfo* p_pInfo, 
                          hOBJECT p_pIO) ;

    tERROR OnAskAction(cAskObjectAction* p_pInfo) ;

    tERROR OnIDSEventReport(cIDSEventReport* p_pInfo) ;

    tERROR UpdateSettings() ;

    tERROR StartSubscription() ;

    tERROR StopSubscription() ;

    tERROR AddComponent(cStrObj& p_strComponent) ;

    cStrObj GetInfoString() ;

    tERROR SendAVZFile() ;

    virtual void do_work() ;



    cKLStatisticsSettings       m_objSettings ;
    hTRANSPORT                  m_hTransport ;
    cStrObj                     m_strBestUrl ;
    hTOKEN                      m_hToken ;
    cVector<hIO>                m_aIO ;
    cVector<cStrObj>            m_aComponents ;
    hCRITICAL_SECTION           m_hCSIORWFile ;
    hCRITICAL_SECTION           m_hCSIOXml ;
    hCRITICAL_SECTION           m_hCSIOEvent ;



// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(CKLStatistics)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end



