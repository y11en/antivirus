#include "Tcp.h"

BEGIN_PROT_PARSE(TCP, tcphdr)
	Message(DL_BRIEF, DM_TCP, ("TCP"));
	Message(DL_INFO, DM_TCP, (", Src port: %d, Dst port: %d", FIELD2(source), FIELD2(dest) ));
	Message(DL_FULL_INFO, DM_TCP, (", fin %d, syn %d, rst %d, psh %d, ack %d, urg %d, window %d, seq %d, ack_seq: %d, check: %d", 
		pField->fin, 
		pField->syn, 
		pField->rst, 
		pField->psh, 
		pField->ack, 
		pField->urg, 
		pField->window, 
		pField->seq, 
		pField->ack_seq, 
		pField->check));
	ADD_PARAM(ID_TCP_SRC_PORT, source)
	ADD_PARAM(ID_TCP_DST_PORT, dest)
END_PROT_PARSE(sizeof(tcphdr))

