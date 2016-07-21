#include "Eth.h"


BEGIN_PROT_PARSE(Eth, EthHdr)
	Message(DL_BRIEF, DM_ETH,  ("TRUE:  --------ETH--------\n"));
	Message(DL_INFO, DM_ETH,  ("TRUE: |SRC:%.2X %.2X %.2X %.2X %.2X %.2X, DST:%.2X %.2X %.2X %.2X %.2X %.2X\n", 
		pField->h_source[0], 
		pField->h_source[1], 
		pField->h_source[2], 
		pField->h_source[3], 
		pField->h_source[4], 
		pField->h_source[5], 
		pField->h_dest[0], 
		pField->h_dest[1], 
		pField->h_dest[2], 
		pField->h_dest[3], 
		pField->h_dest[4], 
		pField->h_dest[5]))
	Message(DL_FULL_INFO, DM_ETH, ("TRUE: |PROTO(Size): %d\n", FIELD2(h_proto)))
	WRONG_IF_TRUE((ps->Frame.dwSize<ETH_ZLEN))
	if(FIELD2(h_proto) < 0x05DD){
		ADD_KEY(MJ_ID_ETH_802_3)
	}else{
		ADD_MKEY(h_proto)
	}
END_PROT_PARSE(sizeof(EthHdr))


BEGIN_PROT_PARSE(Eth_802_3, unsigned short)
	if(*pField == 0xFFFF){
		Message(DL_BRIEF, DM_ETH, ("TRUE: |Eth type: Novell 802.3\n"));
		ADD_KEY(MJ_ID_ETH_RAW_802_3)
		END_OF_PARSE(0, maybe)
	}else if(*pField == 0xAAAA){
		Message(DL_BRIEF, DM_ETH, ("TRUE: |Eth type: Ethernet SNAP\n"));
		ADD_KEY(MJ_ID_ETH_SNAP)
//cut Control
		END_OF_PARSE(3, maybe)
	}else{
		ADD_KEY(MJ_ID_ETH_LLC_802_3)
		END_OF_PARSE(0, maybe)
	}
END_PROT_PARSE(0)

BEGIN_PROT_PARSE(LLC, LLCHdr)
	Message(DL_FULL_INFO, DM_ETH, ("TRUE: |DSAP: %.2X, SSAP: %.2X\n", FIELD1(DSAP), FIELD1(SSAP)));
	ADD_MKEY(SSAP)
	if(pField->Ctrl[0] == 0xC0){
		Message(DL_BRIEF, DM_ETH, ("TRUE: |LLC Type: LLC UNNUMBERED\n"));
		END_OF_PARSE(3, maybe)
	}else{
		if(pField->Ctrl[0] & 0xC0){
			Message(DL_BRIEF, DM_ETH, ("TRUE: |LLC Type: LLC SUPERVISORY\n"));
		}else{
			Message(DL_BRIEF, DM_ETH, ("TRUE: |LLC Type: LLC INFORMATION\n"));
		}
		END_OF_PARSE(4, maybe)
	}
END_PROT_PARSE(0)

BEGIN_PROT_PARSE(IPX, int)
	Message(DL_BRIEF, DM_ETH, ("TRUE: |IPX...\n" ));
END_PROT_PARSE(0)

BEGIN_PROT_PARSE(SNAP, SNAPHdr)
	Message(DL_FULL_INFO, DM_ETH, ("TRUE: |SNAP OUI: %.2X%.2X%.2X, Type: %#.4X\n",pField->OUI[0], pField->OUI[1], pField->OUI[2], pField->Type ));
	ADD_MKEY(Type)
END_PROT_PARSE(sizeof(SNAPHdr))

BEGIN_PROT_PARSE(Trap, EthHdr)
	Message(DL_ERROR, DM_UNWIND, ("TRUE: ERROR. Unrecognize MAC proto: %#x\n", FIELD2(h_proto)));
END_PROT_PARSE(0)
//BEGIN_PROT_PARSE(SNAP , SNAPHdr)



