#ifndef _FLT_LIST
#define _FLT_LIST

#include "GuardFilter.h"
#include "PFFilterID.h"

struct FLTLIST : public std::list<CGuardFilter>
{
	FLTLIST operator* (const CGuardFilter& flt) const
	{
		FLTLIST ret_list = *this;
		
		if (ret_list.empty ())
		{
			ret_list.insert (ret_list.end (), flt);
		}
		else
		{
			const PARAMLIST& params = flt.GetParamList ();
			
			for (FLTLIST::iterator i = ret_list.begin(); i != ret_list.end(); i++)
			{
				assert ((i->m_HookID == flt.m_HookID) || (flt.m_HookID == 0));
				assert ((i->m_FunctionMj == flt.m_FunctionMj) || (flt.m_FunctionMj == 0));
				assert ((i->m_FunctionMi == flt.m_FunctionMi) || (flt.m_FunctionMi == 0));
				assert ((i->m_ProcessingType == flt.m_ProcessingType) || (flt.m_ProcessingType == PreProcessing));
				assert ((i->m_Flags == flt.m_Flags) || (flt.m_Flags == FLT_A_DEFAULT));
				assert ((i->m_Timeout == flt.m_Timeout) || (flt.m_Timeout == 0));
				assert ((i->m_Expiration == flt.m_Expiration) || (flt.m_Expiration == 0));
				assert ((i->m_szProcName == flt.m_szProcName) || (flt.m_szProcName == "*"));
				

				for (PARAMLIST::const_iterator j = params.begin (); j != params.end (); j++)
				{
					i->AddParam (*j);
				}
			}
		}

		return ret_list;
	}

	FLTLIST& operator*= (const CGuardFilter& flt)
	{
		if (empty ())
		{
			insert (end (), flt);
		}
		else
		{
			const PARAMLIST& params = flt.GetParamList ();
			
			for (FLTLIST::iterator i = begin(); i != end(); i++)
			{
				assert ((i->m_HookID == flt.m_HookID) || (flt.m_HookID == 0));
				assert ((i->m_FunctionMj == flt.m_FunctionMj) || (flt.m_FunctionMj == 0));
				assert ((i->m_FunctionMi == flt.m_FunctionMi) || (flt.m_FunctionMi == 0));
				assert ((i->m_ProcessingType == flt.m_ProcessingType) || (flt.m_ProcessingType == PreProcessing));
				assert ((i->m_Flags == flt.m_Flags) || (flt.m_Flags == FLT_A_DEFAULT));
				assert ((i->m_Timeout == flt.m_Timeout) || (flt.m_Timeout == 0));
				assert ((i->m_Expiration == flt.m_Expiration) || (flt.m_Expiration == 0));
				assert ((i->m_szProcName == flt.m_szProcName) || (flt.m_szProcName == "*"));
								
				for (PARAMLIST::const_iterator j = params.begin (); j != params.end (); j++)
				{
					i->AddParam (*j);
				}
				
			}
		}
		
		return *this;
	}
		

	FLTLIST operator* (const FLTLIST &lst) const
	{
		FLTLIST lst_new, lst_layer, lst_layer_new;

		if (lst.empty ())
		{
			lst_new = *this;
		}
		else
		{
			for (FLTLIST::const_iterator j = lst.begin(); j != lst.end(); j++)
			{
				lst_layer = *this;
				lst_layer_new = lst_layer * *j;
				lst_new += lst_layer_new;
			}			
		}
					
		return lst_new;
	}

	FLTLIST& operator= (const FLTLIST& lst)
	{
		clear ();

		for (FLTLIST::const_iterator i = lst.begin(); i != lst.end(); i++)
		{
			insert (end (), *i);
		}
		
		return *this;
	}

	FLTLIST& operator+= (const FLTLIST& lst)
	{
		for (FLTLIST::const_iterator i = lst.begin(); i != lst.end(); i++)
		{
			insert(end (), *i);
		}
		
		return *this;
	}
	
	void SetOptions(ULONG HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD dwFlags, LPCSTR szProcName = "*")
	{
		for (FLTLIST::iterator i = begin(); i != end(); i++)
		{
			i->m_HookID = HookID;
			i->m_FunctionMj = FunctionMj;
			i->m_FunctionMi = FunctionMi;
			i->m_Flags = dwFlags;
			i->m_szProcName = szProcName;
		}
			
	}
	
	FLTLIST::iterator First ()
	{

		FLTLIST::iterator i = begin ();

		if (i == end ())
		{
			return end ();
		} 
			
		PFILTER_PARAM pParam = i->FindParam (PF_FILTER_ORDER_ID);

		if (!pParam)
		{
			return end ();
		}

		DWORD dwFilterIDMin = *(DWORD*) pParam->m_ParamValue;

		FLTLIST::iterator ret = i;

		while (i != end ())
		{

			pParam = i->FindParam (PF_FILTER_ORDER_ID);
			
			if (!pParam)
			{
				return end ();
			}

			if (*(DWORD*) pParam->m_ParamValue < dwFilterIDMin)
			{
				dwFilterIDMin = *(DWORD*) pParam->m_ParamValue;
				
				ret = i;
			}

			++ i;
		}

		return ret;

	}
};

typedef FLTLIST::iterator GuardFilterIterator;

FLTLIST operator* (const CGuardFilter & flt, const FLTLIST & lst);

#endif //_FLT_LIST