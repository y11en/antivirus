#include <windows.h>
#include <winsock2.h>
#include <Iphlpapi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
typedef DWORD (__stdcall * TGetAdaptersAddresses)(ULONG Family, DWORD Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES pAdapterAddresses, PULONG pOutBufLen);


template< class TIPInfo_type > class CIPInfo
{
public:
	DWORD error()
	{
		return Error;
	}
	void dont_FREE()
	{
		m_dont_free=1;
	}
	CIPInfo()
	{
		RetVal = ERROR;
		Error = 0;
		m_dont_free=0;
		Pointer_to_IP_data=0;
		Pointer_to_next_IP_data=0;
		Pointer_to_current_data=0;
	}
	~CIPInfo()
	{
		if(!m_dont_free && Pointer_to_IP_data!=0)
			FREE(Pointer_to_IP_data);
	}
	TIPInfo_type Get_data_pointer()
	{
		if(Error || (Pointer_to_IP_data==0 && reload()))
			return 0;
		return Pointer_to_IP_data;
	}
	void Set_data_pointer(TIPInfo_type x)
	{
		if(!x)
			return;
		Pointer_to_IP_data=x;
		dont_FREE();
	}
	TIPInfo_type Get_Next()
	{
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		Pointer_to_current_data=Pointer_to_next_IP_data;
		if(Pointer_to_next_IP_data!=0)
			Pointer_to_next_IP_data=Pointer_to_next_IP_data->Next;
		return Pointer_to_current_data;
	}
	TIPInfo_type Get_First()
	{
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		Pointer_to_current_data=Pointer_to_IP_data;
		if(Pointer_to_IP_data!=0)
			Pointer_to_next_IP_data=Pointer_to_IP_data->Next;
		return Pointer_to_IP_data;
	}
	DWORD Number_of_Entries_n()
	{
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		if (Pointer_to_IP_data!=0)
				return Pointer_to_IP_data->dwNumEntries;
		return 0;
	}
	DWORD Number_of_Entries()
	{
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		TIPInfo_type x=Pointer_to_IP_data;
		DWORD n=0;
		while(x!=0)
		{
			n++;
			x=x->Next;
		}
		return n;
	}
	TIPInfo_type Find_by_Index(DWORD index)
	{
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		TIPInfo_type x=Pointer_to_IP_data;
		while(x!=0)
		{
			if(compare_index(x,index)==1)
				break;
			x=x->Next;
		}
		return x;
	}
	TIPInfo_type Find_by_AdapterName(PCHAR str1)
	{	
		if(Pointer_to_IP_data==0 && reload())
			return 0;
		TIPInfo_type x=Pointer_to_IP_data;
		while(x!=0)
		{
			if(compare_adapter_name(x,str1)==1)
				break;
			x=x->Next;
		}
		return x;
	}
	DWORD reload()
	{
		if(Error)
			return Error;
		if(Pointer_to_IP_data!=0)
			FREE(Pointer_to_IP_data);
		Pointer_to_next_IP_data=0;
		Pointer_to_IP_data=0;
		Pointer_to_current_data=0;
		OutBufferLength=0;
		ULONG i;
		for (i = 0; i < 5; i++) 
		{
	
			RetVal = get_IP_data();
			if (RetVal == NO_ERROR && OutBufferLength==0)
			{
				break;
			}
			if (RetVal == NO_ERROR && OutBufferLength>0 && Pointer_to_IP_data)
			{
				Pointer_to_next_IP_data=Pointer_to_IP_data;
				Pointer_to_current_data=Pointer_to_IP_data;
				Error=0;
				return 0;
			}
			if (Pointer_to_IP_data != NULL)
				FREE(Pointer_to_IP_data);
			if (RetVal != ERROR_BUFFER_OVERFLOW && RetVal !=ERROR_INSUFFICIENT_BUFFER)
			{
				break;
			} 
			Pointer_to_IP_data = (TIPInfo_type) MALLOC(OutBufferLength);
			if (Pointer_to_IP_data == NULL)
			{
				break;
			}
		}
		Error=1;
		Pointer_to_IP_data=0;
		return 1;
	}
	
	virtual DWORD compare_index(TIPInfo_type,DWORD) { return 0; };
	virtual DWORD compare_adapter_name(TIPInfo_type,PCHAR) { return 0; };
	virtual DWORD get_IP_data() {return ERROR_INVALID_FUNCTION;}
	TIPInfo_type Pointer_to_current_data;
	ULONG	RetVal;
protected:
	DWORD m_dont_free;
	ULONG OutBufferLength;
	TIPInfo_type Pointer_to_IP_data;
	TIPInfo_type Pointer_to_next_IP_data;
private:
	DWORD	Error;
};

class C_GetAdaptersInfo : public CIPInfo<PIP_ADAPTER_INFO>
{
public:
	DWORD compare_index(PIP_ADAPTER_INFO x,DWORD index);
	DWORD compare_adapter_name(PIP_ADAPTER_INFO x,PCHAR str1);
	DWORD get_IP_data();
};
class C_GetAdaptersAddresses : public CIPInfo<PIP_ADAPTER_ADDRESSES>
{
public:
	C_GetAdaptersAddresses(TGetAdaptersAddresses f,ULONG x,ULONG y)
	{
		Family=x;
		Flags=y;
		func=f;
	
	}
	C_GetAdaptersAddresses(ULONG x,ULONG y)
	{
		Family=x;
		Flags=y;
		HMODULE hIPLHLPAPI = GetModuleHandle("iphlpapi.dll");
		if (hIPLHLPAPI)
			func=(TGetAdaptersAddresses)GetProcAddress(hIPLHLPAPI, "GetAdaptersAddresses");
		else
			func=0;
	
	}
	DWORD compare_index(PIP_ADAPTER_ADDRESSES x,DWORD index);
	DWORD compare_adapter_name(PIP_ADAPTER_ADDRESSES x,PCHAR str1);
	DWORD get_IP_data();
private:
	ULONG Family,Flags;
	TGetAdaptersAddresses func;
};

class C_GetIpForwardTable : public CIPInfo<PMIB_IPFORWARDTABLE>
{
public:
	DWORD get_IP_data();
	IPAddr get_IP4_gateway(DWORD *);
	BOOL  ROUTEcmp(PMIB_IPFORWARDTABLE);
};
class C_ArpTable : public CIPInfo<PMIB_IPNETTABLE>
{
public:
	DWORD get_IP_data();
	PMIB_IPNETROW find_by_ip(IPAddr);
	DWORD del_by_ip(IPAddr);
	DWORD arpspoof_check_by_ip(IPAddr,PMIB_IPNETROW);
	BOOL compare_tables_for_spoof(PMIB_IPNETTABLE);
	BOOL ARPmemcmp(PMIB_IPNETTABLE);
};
