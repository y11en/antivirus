#define PR_IMPEX_DEF

#include <winsock2.h>
#include <Ws2tcpip.h>

#include "resip_imp.h"

tERROR pr_call ipres_ThreadCallback(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext)
{
	tERROR err_ret = errOK;

	sThreadContext* pCommonContext = (sThreadContext*) pCommonThreadContext;
	
	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		{
			WSADATA WSAData;
			WORD wVersionRequested = MAKEWORD( 2, 2 );
	
			if (WSAStartup(wVersionRequested, &WSAData))
				err_ret = errNOT_SUPPORTED;
			else
			{
				sThreadContext* pContext = new sThreadContext;
				*ppThreadContext = pContext;
				if (!pContext)
					err_ret = errUNEXPECTED;
				else
					*pContext  = *pCommonContext;
			}
		}
		break;
	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		if (*ppThreadContext)
		{
			sThreadContext* pContext = (sThreadContext*) *ppThreadContext;
			*ppThreadContext = 0;
			
			delete pContext;
		}

		WSACleanup();
		break;
	case TP_CBTYPE_THREAD_YIELD:
		if (*ppThreadContext)
		{
			sThreadContext* pContext = (sThreadContext*) *ppThreadContext;
			//ProcessDelayed();
		}
		break;
	}

	return err_ret;
}

void ResolveOneHost(cIPResolveResult* pResult)
{
	cStrBuff strHostName = pResult->m_HostName.c_str(cCP_ANSI);

	pResult->m_IPs.clear();

	int count = strHostName.count();
	if (count)
	{
		char* nodename = (char*) strHostName.m_ptr;
		char* servname = NULL;

		for (int cou = count; cou > 0 && !servname; cou--)
		{
			if (nodename[cou] == ':')
			{
				servname = nodename + cou + 1;
				nodename[cou] = 0;
			}
		}

		addrinfo* res;
		
		int nres = getaddrinfo(nodename, servname, NULL, &res);
		if ((WSATRY_AGAIN == nres) || (WSA_NOT_ENOUGH_MEMORY == nres))
			nres = getaddrinfo(nodename, servname, NULL, &res);
		if (!nres)
		{
			addrinfo* restmp = res;
			while (restmp)
			{
                if (restmp->ai_family == PF_INET)
                {
				    sockaddr_in *paddr;
				    paddr = (sockaddr_in*) restmp->ai_addr;

                    cIP ip;
                    ip.SetV4(htonl(paddr->sin_addr.S_un.S_addr));
				    pResult->m_IPs.push_back(ip);
                }
                else if (restmp->ai_family == PF_INET6)
                {
                    sockaddr_in6 *paddr;
                    paddr = (sockaddr_in6*) restmp->ai_addr;

                    cIPv6 ipv6;
                    for (int i = 0; i < 16; ++i)
                        ipv6.m_Bytes[i] = paddr->sin6_addr.u.Byte[15-i];

                    cIP ip;
                    ip.SetV6(ipv6);
                    pResult->m_IPs.push_back(ip);
                }

				restmp = restmp->ai_next;
			}
			
			freeaddrinfo(res);
		}

	}
}

void ResolveOneIP(cIPResolveResult* pResult)
{
	char hostname[1025];		// NI_MAXHOST
	hostname[0] = 0;

	cIP ip = pResult->m_IPs.at(0);
	
    int result = 0;
    if (ip.IsV4())
    {
	    sockaddr_in sa;
	    memset(&sa, 0, sizeof(sa));

	    sa.sin_family = AF_INET;
	    sa.sin_port = 0;
        sa.sin_addr.S_un.S_addr = htonl(ip.m_V4);

    	result = getnameinfo((sockaddr*) &sa, sizeof(sa), hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);
    }
    else if (ip.IsV6())
    {
	    sockaddr_in6 sa;
	    memset(&sa, 0, sizeof(sa));

        sa.sin6_family = AF_INET6;
        sa.sin6_port = 0;
        sa.sin6_flowinfo = 0;
        sa.sin6_scope_id = 0;

        for (int i = 0; i < 16; ++i)
            sa.sin6_addr.u.Byte[i] = ip.m_V6.m_Bytes[16-i];

    	result = getnameinfo((sockaddr*) &sa, sizeof(sa), hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);
    }

	if (!result)
		pResult->m_HostName.assign(hostname, cCP_ANSI);
}

void ProcessSingleObject(sThreadContext* pContext)
{
	PIPRESOLVE_LISTITEM pItem = NULL;

	pContext->m_hSync->Lock(cSYNC_INFINITE);

	if (!IsListEmpty(pContext->m_pList))
		pItem = (PIPRESOLVE_LISTITEM) RemoveHeadList(pContext->m_pList);

	pContext->m_hSync->Release();

	if (!pItem)
		return;
	
	if (pItem->m_ResolveDirection)
		ResolveOneHost(&pItem->m_ResolveData);
	else
		ResolveOneIP(&pItem->m_ResolveData);

	pContext->m_Task->sysSendMsg(pmc_IPRESOLVER_RESULT, pm_RESOLVE_INFO, 0, &pItem->m_ResolveData, SER_SENDMSG_PSIZE);

	delete pItem;
}

tERROR pr_call ipres_ThreadTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
{
	sThreadContext* pContext = (sThreadContext*) pThreadContext;
	
	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		{
			ProcessSingleObject(pContext);
		}
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		break;
	}

	return errOK;
}

