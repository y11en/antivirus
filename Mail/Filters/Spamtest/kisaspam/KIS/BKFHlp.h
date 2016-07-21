#ifndef __BKFHLP_H__
#define __BKFHLP_H__

#include "MessageInfo/MessageInfo.h"
#include "DataBase.h"

unsigned int BKF_CheckMAILMSG(MESSAGE_INFO *pMsg, CBKFBase &BKFBase);
unsigned int BKF_CheckMSOE(MESSAGE_INFO *pMsg, CBKFBase &BKFBase);

#endif