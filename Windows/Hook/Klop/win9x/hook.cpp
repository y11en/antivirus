#include "hook.h"

HDSC_Thunk	NdisRegisterProtocolThunk;
HDSC_Thunk	NdisDeregisterProtocolThunk;
HDSC_Thunk	NdisOpenAdapterThunk;
HDSC_Thunk	NdisCloseAdapterThunk;

HDSC_Thunk	NdisRequestThunk;
HDSC_Thunk	NdisResetThunk;

HDSC_Thunk  AddExportTableThunk;
HDSC_Thunk  RemoveExportTableThunk;

t_PELDR_AddExportTable   rPELDR_AddExportTable;

PVOID   FuncAddr = NULL;

// кстати сигнатура не полная, а только начало функции ...
unsigned char _f_AddExpTab[10] = { 
    0x60, 0x8b, 0x7c, 0x24, 0x28, 0xff, 0x74, 0x24, 0x44, 0xff 
};

PVOID   GetFuncPTR()
{
    if ( 0 == memcmp( _f_AddExpTab, rPELDR_AddExportTable, sizeof (_f_AddExpTab) ) )
    {
        PVOID   Func = (char*)rPELDR_AddExportTable + 0x2B;

        Func = *(PVOID*)Func;
        
        return Func;
    }

    return NULL;
}



ULONG   HookNdis()
{
    ULONG	WinVer = Get_VMM_Version(NULL);
    
    if (WinVer == 0x45A)
    {
        if ( rPELDR_AddExportTable = (t_PELDR_AddExportTable)LOCK_Hook_Device_Service_C(___PELDR_AddExportTable, HPELDR_AddExportTable, &AddExportTableThunk) )
        {
     //       Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "AddExportTable hooked\n");            
        }
        else
        {
//            Dbg->Print(BIT_INIT, LEVEL_ERROR, "AddExportTable NOT hooked !!! \n");
        }
    }
    else
    {	
        // Win9x detected 
//         Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "Windows 9x detected\n");
        
        if ( rNdisRegisterProtocol   = (REGISTER_PROTOCOL)LOCK_Hook_Device_Service_C(__NDISRegisterProtocol, HNdisRegisterProtocol, &NdisRegisterProtocolThunk) )
        {
//            Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "NdisRegisterProtocol hooked\n");
        }
        else
        {
//            Dbg->Print(BIT_INIT, LEVEL_ERROR, "NdisRegisterProtocol NOT hooked !!!\n");
        }
        
        
        if ( rNdisDeregisterProtocol = (DEREGISTER_PROTOCOL)LOCK_Hook_Device_Service_C(__NDISDeregisterProtocol, HNdisDeregisterProtocol, &NdisDeregisterProtocolThunk) )
        {
            //Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "NdisDeregisterProtocol hooked\n");
        }
        else
        {
            //Dbg->Print(BIT_INIT, LEVEL_ERROR, "NdisDeregisterProtocol NOT hooked !!!\n");
        }
        
        
        if ( rNdisOpenAdapter        = (OPEN_ADAPTER)LOCK_Hook_Device_Service_C(__NDISOpenAdapter, HNdisOpenAdapter, &NdisOpenAdapterThunk) )
        {
         //   Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "NdisOpenAdapter hooked\n");
        }
        else
        {
           // Dbg->Print(BIT_INIT, LEVEL_ERROR, "NdisOpenAdapter NOT hooked !!!\n");
        }
        
        
        if ( rNdisCloseAdapter       = (CLOSE_ADAPTER)LOCK_Hook_Device_Service_C(__NDISCloseAdapter, HNdisCloseAdapter, &NdisCloseAdapterThunk) )
        {
        //    Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "NdisCloseAdapter hooked\n");
        }
        else
        {
         //   Dbg->Print(BIT_INIT, LEVEL_ERROR, "NdisCloseAdapter NOT hooked !!!\n");
        }
    }
    return TRUE;
}

// Windows Millenium Edition .....................
LRESULT 
__cdecl
HPELDR_AddExportTable(
					  PHPEEXPORTTABLE phExportTable,
					  PSTR pszModuleName,
					  ULONG cExportedFunctions,
					  ULONG cExportedNames,
					  ULONG ulOrdinalBase,
					  PVOID *pExportNameList,
					  PUSHORT pExportOrdinals,
					  PVOID *pExportAddrs,
					  PHLIST phList
					  )
{
	PCHAR	NdisName						= "NDIS.SYS";
	PCHAR	NdisSendName					= "NdisSend";
	PCHAR	NdisRegisterProtocolName	    = "NdisRegisterProtocol";
	PCHAR	NdisDeregisterProtocolName	    = "NdisDeregisterProtocol";
	PCHAR	NdisOpenAdapterName	    		= "NdisOpenAdapter";
	PCHAR	NdisCloseAdapterName    		= "NdisCloseAdapter";
	PCHAR	NdisTransterDataName    		= "NdisTransferData";
    PCHAR	NdisRequestName    		        = "NdisRequest";
    PCHAR	NdisResetName    		        = "NdisReset";

	LRESULT	Res;
	ULONG	Ordinal;

    PVOID   func = NULL;

    if ( FuncAddr == NULL )
    {        
        if ( func = GetFuncPTR() )
        {
            FuncAddr = *(PVOID*)func;
        }
    }
    else
    {
        if ( func = GetFuncPTR() )
        {
            if ( *(PVOID*)func == NULL )
            {
                *(PVOID*)func = FuncAddr;
            }
        }
    }
	
	if (memcmp(NdisName, pszModuleName, 8) == 0)
	{
		ULONG	i;
		PCHAR	FuncName;

        isWin98 = FALSE;

		for ( i = ulOrdinalBase; i < cExportedNames; ++i )
		{
			FuncName = (PCHAR)pExportNameList[i];			
			
			if ( memicmp(FuncName, NdisRegisterProtocolName, 20) == 0   && strlen(FuncName) == 20 )
			{
				// We've found NdisRegisterProtocol								
				Ordinal = pExportOrdinals[i];
				rNdisRegisterProtocol = (REGISTER_PROTOCOL)pExportAddrs[Ordinal];
				pExportAddrs[Ordinal] = HNdisRegisterProtocol;
				
			}


			if ( memicmp(FuncName, NdisDeregisterProtocolName, 22) == 0 && strlen(FuncName) == 22 )
			{					
				// We've found NdisDeregisterProtocol
				Ordinal = pExportOrdinals[i];
				rNdisDeregisterProtocol = (DEREGISTER_PROTOCOL)pExportAddrs[Ordinal];
				pExportAddrs[Ordinal] = HNdisDeregisterProtocol;
				
			}
			
			if ( memicmp(FuncName, NdisOpenAdapterName, 15) == 0        && strlen(FuncName) == 15 )
			{	
				// We've found NdisOpenAdapter				
				Ordinal = pExportOrdinals[i];
				rNdisOpenAdapter = (OPEN_ADAPTER)pExportAddrs[Ordinal];
				pExportAddrs[Ordinal] = HNdisOpenAdapter;
				
			}

			if ( memicmp(FuncName, NdisCloseAdapterName, 16) == 0       && strlen(FuncName) == 16 )
			{					
				Ordinal = pExportOrdinals[i];
				rNdisCloseAdapter = (CLOSE_ADAPTER)pExportAddrs[Ordinal];
				pExportAddrs[Ordinal] = HNdisCloseAdapter;				
			}
		}
	}

    
    Res = rPELDR_AddExportTable
										(
										phExportTable,
										pszModuleName,
										cExportedFunctions,
										cExportedNames,
										ulOrdinalBase,
										pExportNameList,
										pExportOrdinals,
										pExportAddrs,
										phList
										);
    
	
	return Res;
}