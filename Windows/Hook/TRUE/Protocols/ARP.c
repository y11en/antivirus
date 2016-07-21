#include "ARP.h"


BEGIN_PROT_PARSE(ARP, arphdr)
	Message(DL_BRIEF, DM_ARP,  ("TRUE:  --------ARP--------\n"));
	Message(DL_BRIEF, DM_ARP, ("TRUE: |ARP command: %x\n", FIELD2(ar_op)));
	Message(DL_FULL_INFO, DM_ARP, ("TRUE: |ARP Hardware address type: %x\n", FIELD2(ar_hrd)));
	Message(DL_FULL_INFO, DM_ARP, ("TRUE: |ARP Protocol address type: %x\n", FIELD2(ar_pro)));
	Message(DL_FULL_INFO, DM_ARP, ("TRUE: |ARP Hdr addr. size: %x\n", FIELD1(ar_hln)));
	Message(DL_FULL_INFO, DM_ARP, ("TRUE: |ARP Proto addr. size: %x\n", FIELD1(ar_pln)));
	ADD_PARAM(ID_ARP_HRD_TYPE, ar_hrd)
	ADD_PARAM(ID_ARP_PRO_TYPE, ar_pro)
	ADD_PARAM(ID_ARP_HRD_SIZE, ar_hln)
	ADD_PARAM(ID_ARP_PRO_SIZE, ar_pln)
	ADD_PARAM(ID_ARP_COMMAND, ar_op)
	ADD_KEY((FIELD2(ar_hrd)<<16)|(FIELD2(ar_pro)))
END_PROT_PARSE(sizeof(arphdr))


BEGIN_PROT_PARSE(ARP_IP, arp_ip_hdr)
	Message(DL_INFO, DM_ARP, ("TRUE: |ARP Sender:%.2X %.2X %.2X %.2X %.2X %.2X(%d.%d.%d.%d), Target:%.2X %.2X %.2X %.2X %.2X %.2X(%d.%d.%d.%d)\n", 
		pField->ar_sha[0], pField->ar_sha[1], 
		pField->ar_sha[2], pField->ar_sha[3], 
		pField->ar_sha[4], pField->ar_sha[5], 
		(ULONG)(*(BYTE*)&pField->ar_sip), 
		(ULONG)(*((BYTE*)&pField->ar_sip + 1)), 
		(ULONG)(*((BYTE*)&pField->ar_sip + 2)), 
		(ULONG)(*((BYTE*)&pField->ar_sip + 3)), 
		pField->ar_tha[0], pField->ar_tha[1], 
		pField->ar_tha[2], pField->ar_tha[3], 
		pField->ar_tha[4], pField->ar_tha[5], 
		(ULONG)(*(BYTE*)&pField->ar_tip), 
		(ULONG)(*((BYTE*)&pField->ar_tip + 1)), 
		(ULONG)(*((BYTE*)&pField->ar_tip + 2)), 
		(ULONG)(*((BYTE*)&pField->ar_tip + 3))))
	ADD_PARAM_PTR(ID_ARP_SENDER_MAC_ADDRESS, 6, ar_sha)
	ADD_PARAM_PTR(ID_ARP_TARGET_MAC_ADDRESS, 6, ar_tha)
	ADD_PARAM(ID_ARP_SENDER_IP_ADDRESS, ar_sip)
	ADD_PARAM(ID_ARP_TARGET_IP_ADDRESS, ar_tip)
END_PROT_PARSE(sizeof(arp_ip_hdr))

