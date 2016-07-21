#ifndef _MACROS_H_
#define _MACROS_H_

#include <string>

#define THREAD_REGISTER()   FSDrvInterceptorRegisterInvisibleThread();
#define THREAD_UNREGISTER() FSDrvInterceptorUnregisterInvisibleThread();
#define _CATCH_ALL_TRACE    \
} \
catch(_com_error &e) \
{ \
	PR_TRACE((0, prtERROR, "_com_error '%s' at %s, %d", e.ErrorMessage(), __FILE__, __LINE__)); \
} \
catch(std::exception &e) \
{ \
	PR_TRACE((0, prtERROR, "std exception '%s' at %s, %d", e.what(), __FILE__, __LINE__)); \
} \
catch(...) \
{ PR_TRACE((0, prtERROR, "exception at %s, %d", __FILE__, __LINE__));

#define FOREACH_EXPLORER_BEGIN(spApp)	\
{	\
	CComPtr<Outlook::_Explorers> spExplorers;	\
	HRESULT hr = S_OK;	\
	_TRY_BEGIN	\
	hr = spApp->get_Explorers(&spExplorers);	\
	long ulExplCount = 0;	\
	hr = spExplorers->get_Count(&ulExplCount);	\
	for (int ulExplCounter = 1; ulExplCounter <= ulExplCount; ulExplCounter++)	\
	{	\
		CComVariant vExplNum(ulExplCounter);	\
		CComPtr<Outlook::_Explorer> spCurrentExplorer;		\
		_TRY_BEGIN hr = spExplorers->Item(vExplNum, &spCurrentExplorer); _CATCH_ALL_TRACE _CATCH_END	\
		{
#define FOREACH_EXPLORER_END	\
		}	\
	}	\
	_CATCH_ALL_TRACE	\
	_CATCH_END	\
}

#define FOREACH_BAR_BEGIN(spExplorer)	\
{	\
	HRESULT hr = S_OK;	\
	CComPtr<Office::_CommandBars> spCmdBars;	\
	int ulBarCount = 0;	\
	_TRY_BEGIN	\
	if (SUCCEEDED(hr = spExplorer->get_CommandBars(&spCmdBars)) && spCmdBars)	\
		ulBarCount = spCmdBars->GetCount();	\
	for (int ulBarCounter = 1; ulBarCounter <= ulBarCount; ulBarCounter++)	\
	{	\
		CComVariant vBarNum(ulBarCounter);	\
		CComPtr<Office::CommandBar> spCurrentBar = NULL;	\
		_TRY_BEGIN spCurrentBar = spCmdBars->GetItem(vBarNum); _CATCH_ALL_TRACE _CATCH_END	\
		CComPtr<Office::CommandBarControls> spCurrentBarControls = NULL;	\
		_TRY_BEGIN spCurrentBarControls = spCurrentBar->GetControls();	_CATCH_ALL_TRACE _CATCH_END	\
		{

#define FOREACH_BAR_END	\
		}	\
	}	\
	_CATCH_ALL_TRACE	\
	_CATCH_END	\
}

#define FOREACH_CONTROL_BEGIN(spBarControls, nFromPos)	\
{	\
	_TRY_BEGIN	\
	int ulCount = spBarControls->GetCount();	\
	for (int i = nFromPos; i <= ulCount; i++)	\
	{	\
		CComVariant vNum(i);	\
		CComPtr<Office::CommandBarControl> spCurrentControl = NULL;	\
		_TRY_BEGIN spCurrentControl = spBarControls->GetItem(vNum); _CATCH_ALL_TRACE _CATCH_END	\
		{

#define FOREACH_CONTROL_END	\
		}	\
	}	\
	_CATCH_ALL_TRACE	\
	_CATCH_END	\
}

#define PUT_ENABLED(_ctrl, _enbl)	\
if ( _ctrl )	\
{	\
	BSTR text = 0; \
	HRESULT _hr = _ctrl->get_Caption(&text); \
	PR_TRACE((0, prtIMPORTANT, "%s control '%s'", _enbl ? "Enabling" : "Disabling", 	(const char *)_bstr_t(text, false)));\
	_ctrl->PutEnabled(_enbl);	\
}

#define PUT_VISIBLE(_ctrl, _enbl)	\
if ( _ctrl )	\
{	\
	BSTR text = 0; \
	HRESULT _hr = _ctrl->get_Caption(&text); \
	PR_TRACE((0, prtIMPORTANT, "Putting control '%s' %s", (const char *)_bstr_t(text, false), _enbl ? "visible" : "invisible"));\
	_ctrl->PutVisible(_enbl);	\
}

#define PUT_ENABLED_CTRL(_ctrl, _enbl)	\
if ( _ctrl )	\
{	\
	if ( !_enbl )	\
	{	\
		_ctrl->PutVisible(_enbl);	\
		_ctrl->PutEnabled(_enbl);	\
	}	\
	else	\
	{	\
		_ctrl->PutEnabled(_enbl);	\
		_ctrl->PutVisible(_enbl);	\
	}	\
}

#define FUNC_TRACE(x) CFuncTrace trace x
class CFuncTrace
{
public:
	CFuncTrace(char* szFuncName): m_szFuncName(szFuncName), n_num(-1)
	{
		PR_TRACE((0, prtIMPORTANT, "=> %s", m_szFuncName.c_str())); 
	}
	CFuncTrace(char* szFuncName, int num): m_szFuncName(szFuncName), n_num(num)
	{
		PR_TRACE((0, prtIMPORTANT, "=> %s[%x]", m_szFuncName.c_str(), n_num));
	}
	virtual ~CFuncTrace()        
	{
		if ( n_num != -1 )
			PR_TRACE((0, prtIMPORTANT, "<= %s[%x]", m_szFuncName.c_str(), n_num)); 
		else
			PR_TRACE((0, prtIMPORTANT, "<= %s", m_szFuncName.c_str())); 
	}
private:
	std::string m_szFuncName;
	int n_num;
};

#endif//_MACROS_H_