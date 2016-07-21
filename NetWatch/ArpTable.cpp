#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "adapters.h"

DWORD C_ArpTable::get_IP_data()
{
	return GetIpNetTable(Pointer_to_IP_data,&OutBufferLength,0);
}
PMIB_IPNETROW C_ArpTable::find_by_ip(IPAddr ip)
{
	if(Pointer_to_IP_data==0 && reload())
		return 0;
	DWORD dwNumEntries=Pointer_to_IP_data->dwNumEntries;
	PMIB_IPNETROW table=Pointer_to_IP_data->table;
	for(DWORD x=0;x<dwNumEntries;x++)
	{
		if (ip==table[x].dwAddr)
			return &table[x];
	}
	return 0;
}

DWORD C_ArpTable::del_by_ip(IPAddr ip)
{
	PMIB_IPNETROW x=C_ArpTable::find_by_ip(ip);
	if(x)
	{
		MIB_IPNETROW y;
		memset(&y,0,sizeof(y));
		y.dwAddr=ip;
		y.dwIndex=x->dwIndex;
		return DeleteIpNetEntry(&y);
	}
	return 0;
}
BOOL C_ArpTable::ARPmemcmp(PMIB_IPNETTABLE arp_table)
{
	if(error() || !arp_table || !Pointer_to_IP_data)
		return true;
	if(arp_table->dwNumEntries!=Pointer_to_IP_data->dwNumEntries)
		return true;
	if(memcmp(&Pointer_to_IP_data->table,&arp_table->table,sizeof(MIB_IPNETROW)*Pointer_to_IP_data->dwNumEntries)==0)
		return false;
	return true;  
}
BOOL C_ArpTable::compare_tables_for_spoof(PMIB_IPNETTABLE arp_table)
{
	if(Pointer_to_IP_data==0 && reload())
		return 0;
	
	DWORD new_dwNumEntries=Pointer_to_IP_data->dwNumEntries;
	PMIB_IPNETROW new_table=Pointer_to_IP_data->table;

	DWORD old_dwNumEntries;
	PMIB_IPNETROW old_table;
	if(arp_table)
	{
		old_dwNumEntries=arp_table->dwNumEntries;
		old_table=arp_table->table;
		if(old_dwNumEntries==new_dwNumEntries)
		{
			if(memcmp(new_table,old_table,sizeof(MIB_IPNETROW)*new_dwNumEntries)==0)
				return 0;
		}
	}
	PMIB_IPNETROW current_new;
	PMIB_IPNETROW current_old;
	for(DWORD x=0;x<new_dwNumEntries;x++)
	{
		current_new=&new_table[x];
		DWORD new_ip=current_new->dwAddr;
		
		if(!arp_table)
		{
			C_ArpTable::arpspoof_check_by_ip(new_ip,0);
		}
		else
		{
			for(DWORD y=0;y<old_dwNumEntries;y++)
			{	
				if(current_new->dwType==4)
				{
					new_ip=0;
					break;
				}
				current_old=&old_table[y];
				if(current_old->dwAddr==new_ip &&
					current_old->dwIndex==current_new->dwIndex)
				{
					if(current_old->dwPhysAddrLen!=current_new->dwPhysAddrLen ||
					memcmp(current_old->bPhysAddr,current_new->bPhysAddr,current_old->dwPhysAddrLen)!=0)
					{
						printf("1.\n");
						if (C_ArpTable::arpspoof_check_by_ip(new_ip,current_old))
							return 1;
					}
					new_ip=0;
					break;
				}
			}
			if(new_ip)
			{
				printf("2.\n");
 				C_ArpTable::arpspoof_check_by_ip(new_ip,0);
			}
		}
	}
	return 0;
}
DWORD C_ArpTable::arpspoof_check_by_ip(IPAddr ip,PMIB_IPNETROW old_row)
{
	
	char *str1;
	in_addr ip_in_addr;
	ip_in_addr.S_un.S_addr=ip;
	str1=inet_ntoa(ip_in_addr);
	printf("Checking %s for arp spoofing...\n",str1);

	ULONG MacAddr[3],MacAddr2[3],MacAddr3[3];
	ULONG PhysAddrLen = 12,PhysAddrLen2=12,PhysAddrLen3=12;
	PMIB_IPNETROW x=C_ArpTable::find_by_ip(ip);
	if(!x || x->dwType!=3) 
		return 0; //if no entry or static entry.
	C_ArpTable::del_by_ip(ip);
	if(SendARP(ip,0, MacAddr, &PhysAddrLen)!=0)
		PhysAddrLen=0;
	C_ArpTable::del_by_ip(ip);
	if(SendARP(ip,0, MacAddr2, &PhysAddrLen2)!=0)
	{
		if(PhysAddrLen==0)
			return 0; //cant sendarp two times, exit;
		PhysAddrLen2=0;
	}
	C_ArpTable::del_by_ip(ip);
	if(SendARP(ip,0, MacAddr3, &PhysAddrLen3)!=0)
		PhysAddrLen3=0;
	if(
		(PhysAddrLen>0 &&  (PhysAddrLen!=x->dwPhysAddrLen  || memcmp(MacAddr,x->bPhysAddr,PhysAddrLen)!=0))
		||
		(PhysAddrLen2>0 && (PhysAddrLen2!=x->dwPhysAddrLen || memcmp(MacAddr2,x->bPhysAddr,PhysAddrLen2)!=0))
		||
		(PhysAddrLen3>0 && (PhysAddrLen3!=x->dwPhysAddrLen || memcmp(MacAddr3,x->bPhysAddr,PhysAddrLen3)!=0))
		)
	{

		if(
			((PhysAddrLen==0 || PhysAddrLen2==0) || (PhysAddrLen==PhysAddrLen2 && memcmp(MacAddr,MacAddr2,PhysAddrLen2)==0))
			&&
			((PhysAddrLen==0 || PhysAddrLen3==0) || (PhysAddrLen==PhysAddrLen3 && memcmp(MacAddr,MacAddr3,PhysAddrLen3)==0))
			&&
			((PhysAddrLen2==0 || PhysAddrLen3==0) || (PhysAddrLen2==PhysAddrLen3 && memcmp(MacAddr2,MacAddr3,PhysAddrLen3)==0))

			)
		{
			if(PhysAddrLen!=0)
				memcpy(x->bPhysAddr,MacAddr,PhysAddrLen);
			else
				memcpy(x->bPhysAddr,MacAddr2,PhysAddrLen2);
		}
		if(old_row)
		{
			if(
				(PhysAddrLen!=0 && old_row->dwPhysAddrLen==PhysAddrLen && memcmp(MacAddr,old_row->bPhysAddr,PhysAddrLen)==0)
				||
				(PhysAddrLen2!=0 && old_row->dwPhysAddrLen==PhysAddrLen2 && memcmp(MacAddr2,old_row->bPhysAddr,PhysAddrLen2)==0)
				||
				(PhysAddrLen3!=0 && old_row->dwPhysAddrLen==PhysAddrLen3 && memcmp(MacAddr3,old_row->bPhysAddr,PhysAddrLen3)==0)
				)
			{
				memcpy(x->bPhysAddr,old_row->bPhysAddr,old_row->dwPhysAddrLen);
				printf("use old address.\n");
			}
		}
		else
			printf("Spoofed new address.\n");
		x->dwType=4;
		C_ArpTable::del_by_ip(ip);
		if(CreateIpNetEntry(x)!=0)
		{
			C_ArpTable::del_by_ip(ip);
			CreateIpNetEntry(x);
		}

		printf("Arp spoof of %s detected. setting arp entry to static.\nfor delete static entry run \"arp -d %s\"\n\n",str1,str1);
		return 1;
		//spoofing detected!
	}
	printf("Spoofing not detected.\n\n");
	CreateIpNetEntry(x);
///	C_ArpTable::reload();
	return 0;
}
