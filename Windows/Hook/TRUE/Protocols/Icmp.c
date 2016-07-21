#include "icmp.h"

BEGIN_PROT_PARSE(ICMP, icmphdr)
	Message(DL_BRIEF, DM_ICMP, ("ICMP"));
	Message(DL_INFO, DM_ICMP, (", Type: %d, Code: %d", FIELD1(type), FIELD1(code)));
	ADD_PARAM(ID_ICMP_TYPE, type)
	ADD_PARAM(ID_ICMP_CODE, code)
	ADD_VALUE(ID_ICMP_UNI_CODE, (ULONG)(((USHORT)pField->type<<8)|((USHORT)pField->code)))
END_PROT_PARSE(sizeof(icmphdr))
