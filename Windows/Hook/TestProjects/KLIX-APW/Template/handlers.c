#include "g_precomp.h"
#pragma hdrstop

#define PLUGIN_DESCRIPTION  "$$PluginDescription$$"

DebugHandler            DbgOut;

$$IF(KLICK)
//  KLICK start 
RcvFragmentedHandler    FragRcv;
unsigned long   KlickPluginID;
PVOID           Klick_Device    = NULL;

PLUGIN_CALLBACK KlickCallbackTable[] = {	
    { CB_RECEIVE,               OnReceive,          PRIORITY_NORMAL },
    { CB_RECEIVE_ON_PASSIVE,    OnReceivePassive,   PRIORITY_NORMAL },
    { CB_REGISTER_KLICK_PLUGIN, OnRegisterKlickPlugin,   PRIORITY_NORMAL }
};
//  KLICK end
$$ENDIF

$$IF(KLIN)
//  KLIN  start
QueryHandleHandler      QueryConn;
QueryHandleHandler      QueryAddr;

unsigned long   KlinPluginID;
PVOID           Klin_Device     = NULL;

PLUGIN_CALLBACK KlinCallbackTable[] = {	
    { CB_CONNECT,               OnConnect,          PRIORITY_NORMAL },
    { CB_REGISTER_KLIN_PLUGIN,  OnRegisterKlinPlugin,   PRIORITY_NORMAL }
};
//  KLIN  end
$$ENDIF

$$IF(KLICK)
//  KLICK start 
KLSTATUS
OnRegisterKlickPlugin(unsigned long ID, KL_PLUGIN_TYPE Type, IoctlHandler   PluginIoctl)
{
    return  KL_SUCCESS;
}

KLSTATUS
__cdecl
OnReceivePassive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize)
{    
    return KL_SUCCESS;
}

KLSTATUS
__cdecl
OnReceive(PVOID EthHeader, PVOID    Packet, ULONG   PacketSize)
{
    return KL_SUCCESS;
}

//  KLICK end
$$ENDIF

$$IF(KLIN)
//  KLIN  start
KLSTATUS
OnRegisterKlinPlugin(unsigned long ID, KL_PLUGIN_TYPE Type, IoctlHandler   PluginIoctl)
{
    return  KL_SUCCESS;
}

/*
 *	 ------ KLIN    Callbacks --------------------------
 */
KLSTATUS
__cdecl
OnConnect (PVOID Handle, BOOLEAN isIncoming)
{   
    return KL_SUCCESS;
}

//  KLIN  end
$$ENDIF

/*
 *	------- Message Handling ---------------------------
 */
KLSTATUS
MyIoctlHandler(ULONG nIoctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned)
{
    *nReturned = 0;

    switch ( nIoctl )
    {
    default:
        break;
    }

    return KL_SUCCESS;
}

/*
 *	------- Plugin Initialization ----------------------
 */
KLSTATUS
__cdecl
PluginInit(PPLUGIN_INIT_INFO   PlugInfo, PBASEDRV_INFO  pBaseDrvInfo)
{      
    ULONG       i               = 0;    
    ULONG       CallbackCount   = 0; 
    PLUGIN_CALLBACK* Table;
    
    switch( pBaseDrvInfo->BaseDrvID ) 
    {
$$IF(KLIN)
// Klin Start
    case KLIN_BASE_DRIVER:
        {
            PKLIN_INFO  KlinInfo  = (PKLIN_INFO)pBaseDrvInfo;
            KlinPluginID          = PlugInfo->PluginID;
            CallbackCount         = sizeof ( KlinCallbackTable  ) / sizeof ( PLUGIN_CALLBACK );
            Table                 = KlinCallbackTable;

            QueryAddr             = KlinInfo->QueryAddr;
            QueryConn             = KlinInfo->QueryConn;
        }
    	break;
// Klin End
$$ENDIF
$$IF(KLICK)
// Klick Start
    case KLICK_BASE_DRIVER:
        {
            PKLICK_INFO KlickInfo = (PKLICK_INFO)pBaseDrvInfo;
            KlickPluginID         = PlugInfo->PluginID;            
            CallbackCount         = sizeof ( KlickCallbackTable ) / sizeof ( PLUGIN_CALLBACK );
            Table                 = KlickCallbackTable;

            FragRcv               = KlickInfo->FragRcv;
        }
    	break;
// KLICK end
$$ENDIF
    default:
        break;
    }    
    
    DbgOut                        = pBaseDrvInfo->DbgOut;

    // Заполняем плагинную структуру
    memcpy ( PlugInfo->PluginDescription, PLUGIN_DESCRIPTION, sizeof(PLUGIN_DESCRIPTION) );
    
    PlugInfo->IOCTL_Handler         = MyIoctlHandler;
    PlugInfo->CallbackCount         = CallbackCount;

    PlugInfo->plCallback    =   KL_MEM_ALLOC ( sizeof ( PLUGIN_CALLBACK ) * CallbackCount );

    if ( PlugInfo->plCallback )
    {
        for ( i = 0; i < CallbackCount; ++i )
        {
            PlugInfo->plCallback[i] = Table[i];
        }
    }

    return KL_SUCCESS;
}



