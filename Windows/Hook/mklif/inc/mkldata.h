#ifndef __mkldata
#define __mkldata

#include <fltUser.h>

#include <pshpack1.h>

typedef struct _MKLIF_MESSAGE {
	FILTER_MESSAGE_HEADER	m_MessageHeader;
	MKLIF_EVENT				m_Event;
} MKLIF_MESSAGE, *PMKLIF_MESSAGE;

typedef struct _MKLIF_REPLY_MESSAGE {
    FILTER_REPLY_HEADER		m_ReplyHeader;
	MKLIF_REPLY_EVENT		m_Verdict;
} MKLIF_REPLY_MESSAGE, *PMKLIF_REPLY_MESSAGE;

#include <poppack.h>

#endif 