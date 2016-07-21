
#include <winsock2.h>
#include "adapters.h"


DWORD C_GetAdaptersInfo::compare_index(PIP_ADAPTER_INFO x,DWORD index)
{
	if(x->Index==index)
		return 1;
	return 0;
}
DWORD C_GetAdaptersInfo::compare_adapter_name(PIP_ADAPTER_INFO x,PCHAR str1)
{
	if(strcmp(x->AdapterName,str1)==0)
		return 1;
	return 0;
}

DWORD C_GetAdaptersInfo::get_IP_data()
{

	DWORD RetVal=ERROR_INVALID_FUNCTION;
	__try
	{
		RetVal=GetAdaptersInfo(Pointer_to_IP_data, &OutBufferLength);
	}
	__except(1)
	{
		RetVal=ERROR_INVALID_FUNCTION;
	}
	return RetVal;
}

DWORD C_GetAdaptersAddresses::compare_index(PIP_ADAPTER_ADDRESSES x,DWORD index)
{
	if(x->IfIndex==index)
		return 1;
	return 0;
}
DWORD C_GetAdaptersAddresses::compare_adapter_name(PIP_ADAPTER_ADDRESSES x,PCHAR str1)
{
	if(strcmp(x->AdapterName,str1)==0)
		return 1;
	return 0;
}

DWORD C_GetAdaptersAddresses::get_IP_data()
{
	if(!func)
		return 1;
	DWORD RetVal=ERROR_INVALID_FUNCTION;
	__try
	{
		RetVal=func(Family,Flags,0,Pointer_to_IP_data, &OutBufferLength);
	}
	__except(1)
	{
		RetVal=ERROR_INVALID_FUNCTION;
	}
	return RetVal;
}
DWORD C_GetIpForwardTable::get_IP_data()
{
	DWORD RetVal=ERROR_INVALID_FUNCTION;
	__try
	{
		RetVal=GetIpForwardTable(Pointer_to_IP_data,&OutBufferLength,0);
	}
	__except(1)
	{
		RetVal=ERROR_INVALID_FUNCTION;
	}
	return RetVal;

}
DWORD C_GetIpForwardTable::get_IP4_gateway(DWORD * Index)
{
	if(error() || (Pointer_to_IP_data==0 && reload()))
		return 0;
	DWORD default_gw=0;
	DWORD dwNumEntries=Pointer_to_IP_data->dwNumEntries;
	DWORD metric=100;
	for(DWORD x=0;x<dwNumEntries;x++)
	{
		if(Pointer_to_IP_data->table[x].dwForwardDest==0 && 
			Pointer_to_IP_data->table[x].dwForwardType!=MIB_IPROUTE_TYPE_INVALID &&
			metric>Pointer_to_IP_data->table[x].dwForwardMetric1)
		{
			metric=Pointer_to_IP_data->table[x].dwForwardMetric1;
			default_gw=ntohl(Pointer_to_IP_data->table[x].dwForwardNextHop);
			if(Index)
				*Index=Pointer_to_IP_data->table[x].dwForwardIfIndex;
		}
	}
	return default_gw;
}
BOOL C_GetIpForwardTable::ROUTEcmp(PMIB_IPFORWARDTABLE route_table)
{
	if(Pointer_to_IP_data==0)
		reload();
	if(error() || !route_table || !Pointer_to_IP_data)
		return true;
	if(route_table->dwNumEntries!=Pointer_to_IP_data->dwNumEntries)
		return true;
	PMIB_IPFORWARDROW table1,table2;
	for(DWORD x=0;x<Pointer_to_IP_data->dwNumEntries;x++)
	{
		table1=&route_table->table[x];
		table2=&Pointer_to_IP_data->table[x];

		if(table1->dwForwardIfIndex!=table2->dwForwardIfIndex)
			return true;
		if(table1->dwForwardDest!=table2->dwForwardDest)
			return true;
		if(table1->dwForwardNextHop!=table2->dwForwardNextHop)
			return true;
		if(table1->dwForwardMask!=table2->dwForwardMask)
			return true;
	}
	
	return false;
}