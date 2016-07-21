#ifndef _EXCEPT_H__INCLUDE_
#define _EXCEPT_H__INCLUDE_

#include "defs.h"
#include <exception>
#include <string>

namespace KLUTIL
{

//! Информация о месте возникновения исключения в исходном файле.
struct KLUTIL_API CExSrcInfo
{
	std::string file;
	unsigned line;
	std::string func;
	CExSrcInfo() throw(): line(0) {}
	CExSrcInfo(const std::string& _file, unsigned _line = 0, const char* _func = NULL) throw()
		: file(_file), line(_line), func(_func?_func:"") {}
	CExSrcInfo(const CExSrcInfo& ex) throw()
		: file(ex.file), line(ex.line), func(ex.func) {}
};

typedef __int64 TLibID;
#define DEFINE_LIBID(name, n1, n2, n3, n4) const KLUTIL::TLibID name = ((__int64)(n1) << 48) | ((__int64)(n2) << 32) | ((__int64)(n3) << 16) | (__int64)(n4);

DEFINE_LIBID(EID_Except, 0x225B, 0x283A, 0x8DF0, 0x4b5c)

//! Класс исключение.
class KLUTIL_API CExcept: public std::exception
{
public:
	//! Конструктор.
	CExcept(
		const char* pszMsg = NULL,	//!< [in] Описание ошибки.
		unsigned nCode = 0,			//!< [in] Код ошибки.
		const CExSrcInfo& srcInfo = CExSrcInfo(),	//!< [in] Место ошибки.
		const TLibID idLibCode = EID_Except		//!< [in] Идентификатор библиотеки.
		) throw()
		: m_strMsg(pszMsg?pszMsg:""), m_srcInfo(srcInfo), m_nCode(nCode), m_idLibCode(idLibCode)
	{
	}
	virtual ~CExcept() {}

	virtual const char* what() const throw() { return m_strMsg.c_str(); }
	virtual unsigned GetErrCode() const throw() { return m_nCode; }
	virtual const TLibID& GetLibCode() const throw() { return m_idLibCode; }
	virtual const CExSrcInfo& GetSrcInfo() const throw() { return m_srcInfo; }
	std::string GetSrcInfoString() const throw();

private:
	std::string m_strMsg;
	unsigned m_nCode;
	TLibID m_idLibCode;
	CExSrcInfo m_srcInfo;
};


// шаблон для упрощения создания тривиальных наследников для различных библиотек
template<TLibID nLibCode>
class CLibExcept: public CExcept
{
public:
	CLibExcept(
		const char* pszMsg = NULL,
		unsigned nCode = 0,
		const CExSrcInfo& m_srcInfo = CExSrcInfo()
		) throw()
		: CExcept(pszMsg, nCode, m_srcInfo, nLibCode) {}
	CLibExcept(
		const char* pszMsg,
		const CExSrcInfo& m_srcInfo = CExSrcInfo()
		) throw()
		: CExcept(pszMsg, m_srcInfo, nLibCode) {}
};

// использование шаблона:
//	DEFINE_LIBID(EID_SomeLib, 0x????, 0x????, 0x????, 0x????)
//	typedef CLibExcept<EID_SomeLib> CSomeLibExcept;

#if _MSC_VER >= 1300
#	define SRC_INFO() CExSrcInfo(__FILE__, __LINE__, __FUNCTION__)
#else
#	define SRC_INFO() CExSrcInfo(__FILE__, __LINE__)
#endif

#define THROW_TEXT(exType, m_strMsg) throw exType(m_strMsg, 0, SRC_INFO())
#define THROW(exType, m_nCode, m_strMsg) throw exType(m_strMsg, m_nCode, SRC_INFO())
//#define RETHROW(exType, m_nCode, m_strMsg, ex) throw exType(m_strMsg, m_nCode, ex, SRC_INFO())

} // namespace KLUTIL

#endif