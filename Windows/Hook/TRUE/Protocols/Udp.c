#include "Udp.h"


BEGIN_PROT_PARSE(UDP, udphdr)
	Message(DL_BRIEF, DM_ICMP, ("UDP"));
	Message(DL_INFO, DM_UDP, (", Src port: %d, Dst port: %d", FIELD2(source), FIELD2(dest) ));
	Message(DL_FULL_INFO, DM_UDP, (", Len: %d, Check: %X", FIELD2(len), FIELD2(check)));
	ADD_PARAM(ID_UDP_SRC_PORT, source)
	ADD_PARAM(ID_UDP_DST_PORT, dest)
END_PROT_PARSE(sizeof(udphdr))
