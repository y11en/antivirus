// PragueHelper.h: interface for the CPragueHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRAGUEHELPER_H__D0015018_8D79_4EBD_90DA_6D9CBB457B8A__INCLUDED_)
#define AFX_PRAGUEHELPER_H__D0015018_8D79_4EBD_90DA_6D9CBB457B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

#include <Prague/pr_compl.h>
#include <Prague/pr_types.h>
#include <Prague/pr_err.h>
#include <Prague/pr_pid.h>
#include <Prague/pr_obj.h>
#include <Prague/pr_prop.h>
#include <Prague/pr_iid.h>
#include <Prague/pr_sys.h>
#include <Prague/iface/i_io.h>

#include <stdexcept>
#include <atlbase.h>
#include <boost/utility.hpp>

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

#define PRAGUE_ASSERT(expr) \
	do {					\
		PragueHelper::CError err((expr), __FILE__, __LINE__);	\
	} while(0);

namespace PragueHelper
{
	tstring GetProductPath(bool bAddSlash = true);	// returns the working directory for KAV 6.0
	tstring GetRegStringValue(CRegKey& reg, LPCTSTR szValueName);

	template<typename T>
	class CAutoCloseObj : boost::noncopyable
	{
	public:
		CAutoCloseObj() : m_hObj(NULL) {}
		CAutoCloseObj(T obj) : m_hObj(obj) {}
		~CAutoCloseObj()
		{
			Cleanup();
		}

		CAutoCloseObj& operator=(T obj)
		{
			Cleanup();
			m_hObj = obj;
			return *this;
		}

		operator T()
		{
			return m_hObj;
		}

		T& operator->()
		{
			return m_hObj;
		}

		T* get()
		{
			return &m_hObj;
		}

		void Detach()
		{
			m_hObj = NULL;
		}

	private:
		void Cleanup()
		{
			if (m_hObj)
			{
				m_hObj->sysCloseObject();
				m_hObj = NULL;
			}
		}

		T	m_hObj;
	};
	
	typedef CAutoCloseObj<hIO> CAutoIO;
	typedef CAutoCloseObj<hOBJECT> CAutoObject;
	typedef CAutoCloseObj<hMSG_RECEIVER> CAutoMsgReceiver;

	class CError
	{
	public:
		CError(tERROR nErr, LPCSTR szSourceFile, int nSourceLine) : m_nErr(nErr)
		{
			if (PR_FAIL(nErr))
			{
				std::string strErr("Prague error: ");
				strErr += boost::lexical_cast<std::string>(nErr);

				if (szSourceFile)
				{
					strErr += _T(" (");
					strErr += szSourceFile;
					strErr += _T(", line ");
					strErr += boost::lexical_cast<std::string>(nSourceLine);
					strErr += _T(")");
				}

				throw std::runtime_error(strErr);
			}
		}

		operator tERROR() const
		{
			return m_nErr;
		}

		CError& operator=(tERROR nErr)
		{
			m_nErr = nErr;
			if (PR_FAIL(m_nErr))
			{
				std::string strErr("Prague error: ");
				strErr += boost::lexical_cast<std::string>(nErr);
				throw std::runtime_error(strErr);
			}

			return *this;
		}
		

	private:
		tERROR m_nErr;
	};
}

#endif // !defined(AFX_PRAGUEHELPER_H__D0015018_8D79_4EBD_90DA_6D9CBB457B8A__INCLUDED_)
