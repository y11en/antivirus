#ifndef KLNETCOMMDEFINES_H
#define KLNETCOMMDEFINES_H

// KL Network communication functions return codes
#define AVPNET_RC_NODATA			 0
#define AVPNET_RC_ERROR				-1
#define AVPNET_RC_TIMEDOUT			-2

// KL Network communication connection functions return codes
#define AVPNET_RCCON_ERROR			NULL
#define AVPNET_RCCON_TIMEDOUT		-1

// KL Network communication authentication defines
#define AUTH_REPLY_OK				0xABCC0000
#define AUTH_REPLY_FAIL				0xABCC0001

#endif