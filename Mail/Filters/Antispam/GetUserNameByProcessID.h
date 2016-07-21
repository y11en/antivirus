#ifndef _GETUSERNAMEBYPROCESSID_H_
#define _GETUSERNAMEBYPROCESSID_H_

#include <Prague/iface/i_token.h>
#include <Prague/plugin/p_win32loader.h>

inline tERROR GetUserNameByProcessID(IN tULONG ulPID, OUT cStringObj& szUserName)
{
	cERROR err;
	szUserName.clear();

	cAutoObj<cToken> hToken;
	if( PR_FAIL(err = ::g_root->sysCreateObject(hToken, IID_TOKEN)) )
		return err;
	
	hToken->propSetDWord(plPROCESS_ID, ulPID);
	
	if( PR_FAIL(err = hToken->sysCreateObjectDone()) )
		return err;
	
	return szUserName.assign(hToken, pgOBJECT_FULL_NAME);
}

#endif//_GETUSERNAMEBYPROCESSID_H_