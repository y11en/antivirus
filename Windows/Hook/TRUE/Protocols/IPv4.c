#include "IPv4.h"
	
BEGIN_PROT_PARSE(IP, iphdr)
	WRONG_IF_TRUE(FIELD1(version)!=4)
	ADD_PARAM(ID_IP_PROTOCOL, protocol)
	ADD_PARAM(ID_IP_SRC_ADDRESS, saddr)
	ADD_PARAM(ID_IP_DST_ADDRESS, daddr)
	if(pField->ihl > 5){
		ADD_VALUE(ID_IP_OPCODE,  *((BYTE*)pField + sizeof(iphdr))) 
	}

ADD_MKEY(protocol)
END_PROT_PARSE(sizeof(iphdr))