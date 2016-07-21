/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_remote.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Реализация межпроцесного взаимодействия.
 * 
 */
//************************************************************************

#include "pr_common.h"

//************************************************************************

struct PRLibrary
{
	PRLibrary(tPROCESS process, tSTRING name) : m_process(process), m_name(name), m_ref(0){}
	bool operator < (const PRLibrary& c) const { return m_process < c.m_process ? true : m_name < c.m_name; }

	tPROCESS	m_process;
	std::string	m_name;
	int			m_ref;
	tDWORD		m_library;
	handle_t	m_handle;
};

class TLibraryList : private std::set<PRLibrary>
{
public:
	PRLibrary * GetLibrary(tPROCESS process, tSTRING name)
	{
		std::pair<iterator, bool> l_add = insert(value_type(PRLibrary(process, name)));
                PRLibrary& l_library = const_cast<PRLibrary&> ( *l_add.first );
		return &l_library;
	}
	void DelLibrary(tPROCESS process, tSTRING name)
	{
		erase(value_type(PRLibrary(process, name)));
	}
	
} g_library_list;

//************************************************************************

tERROR RLGetLibrary(tPROCESS process, tSTRING name, tLIBRARY *library)
{
	if( process == PR_PROCESS_LOCAL )
		process = g_mng.m_ProccessId;

	PRLibrary *l_library = g_library_list.GetLibrary(process, name);
	if( !l_library->m_ref )
	{
		tERROR l_err = errUNEXPECTED;
		if( process != g_mng.m_ProccessId )
		{
			PRConnection * l_connect = g_mng.FindConnection(PRRemoteHost(process));
			if( l_connect )
			{
				l_library->m_handle = l_connect->m_handle;
				l_err = RPC_LoadLibrary(l_library->m_handle, name, &l_library->m_library);
			}
		}
		else
		{
			l_library->m_handle = 0;
			l_err = PRRPC_LoadLibrary(0, name, &l_library->m_library);
		}

		if( PR_FAIL(l_err) )
			return l_err;
	}
	l_library->m_ref++;
	*library = (tLIBRARY)l_library;
	return errOK;
}

tERROR RLInvokeMethod(tLIBRARY library, tDWORD method, void *param, tDWORD param_size)
{
	PRLibrary *l_library = (PRLibrary*)library;
	if( l_library->m_handle )
		return RPC_InvokeMethod(l_library->m_handle, l_library->m_library, method, (tCHAR*)param, param_size);
	else
		return PRRPC_InvokeMethod(0, l_library->m_library, method, (tCHAR*)param, param_size);
}

tERROR RLReleaseLibrary(tLIBRARY library)
{
	PRLibrary *l_library = (PRLibrary*)library;
	if( --l_library->m_ref )
		return errOK;

	tERROR l_err;
	if( l_library->m_handle )
		l_err = RPC_FreeLibrary(l_library->m_handle, l_library->m_library);
	else
		l_err = PRRPC_FreeLibrary(0, l_library->m_library);
	g_library_list.DelLibrary(l_library->m_process, (tSTRING)l_library->m_name.c_str());
	return l_err;
}


//************************************************************************

tERROR PRRPC_LoadLibrary(handle_t IDL_handle, tSTRING name, tDWORD *hnd)
{
	*hnd = (tDWORD)LOAD_LIBRARY(name);
	return errOK;
}

tERROR PRRPC_InvokeMethod(handle_t IDL_handle, tDWORD hnd, tDWORD method, tCHAR *param, tDWORD size)
{
	PRInvokeHandler l_invoke_handler = (PRInvokeHandler)GET_SYMBOL(hnd, INVOKE_HANDLER);
	if( !l_invoke_handler )
		return errHANDLE_INVALID;

	l_invoke_handler(method, (void*)param, size);
	return errOK;
}

tERROR PRRPC_FreeLibrary(handle_t IDL_handle, tDWORD hnd)
{
	UNLOAD_LIBRARY(hnd);
	return errOK;
}

//************************************************************************
