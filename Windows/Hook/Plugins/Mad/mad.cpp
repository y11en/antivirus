#include "Mad.h"
#include "..\ids\attacknotify.h"
#include "ids_common.h"

PLUGIN_CALLBACK MadTable[] = {	    
    { CB_PACKET_RECEIVE, CKl_Mad::ReceiveFrame, PRIORITY_NORMAL   }
};

CKl_Mad::CKl_Mad(char* PluginName /* = NULL */) : CKl_KlickPlugin(PluginName)
{
    AddTable( MadTable, sizeof ( MadTable ) / sizeof ( PLUGIN_CALLBACK ) );
	
    
    Ref();
}

CKl_Mad::~CKl_Mad()
{
    if ( m_Registered )
    {
        Unregister();
    }
    
    Deref ();
}

KLSTATUS    
CKl_Mad::OnReceiveIOCTL(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned )
{
    return OnReceiveMessage( MessageID, pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned );
}

KLSTATUS
CKl_Mad::OnReceiveMessage(ULONG MessageID, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned )
{
    KLSTATUS        klStatus = KL_SUCCESS;
    PPLUGIN_MESSAGE pm = (PPLUGIN_MESSAGE)pInBuffer;

    switch( pm->MessageID ) 
    {
    case MAD_MSGID_ATTACK:
        {
            DbgOut ("Send Attack Notification !\n");
            
            PMAD_ATTACK Mad = (PMAD_ATTACK)pm->Message;
            
            AttackNotifyStruct Par;
            
            Par.m_AttackID     = Mad->AttackID;
            Par.m_AttackerIP   = Mad->AttackerIP;
            Par.m_Proto        = Mad->Proto;
            Par.m_LocalPort    = Mad->LocalPort;
            
            EventNotify(ATTACK_NOTIFY, sizeof(AttackNotifyStruct*), &Par);
            
            break;
        }        
    default:
        break;
    }


    return klStatus;
}

KLSTATUS
KL_CALLBACK
CKl_Mad::ReceiveFrame(PVOID pktContext, void*   plContext)
{       
    return KL_SUCCESS;
}

