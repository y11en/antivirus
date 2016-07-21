// AutoPTCHAR.h: interface for the CAutoPTCHAR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOPTCHAR_H__6B12B83C_1290_41C0_B483_4932AA46B952__INCLUDED_)
#define AFX_AUTOPTCHAR_H__6B12B83C_1290_41C0_B483_4932AA46B952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AutoPTCHAR
{
public:
	AutoPTCHAR(CString* pstr, size_t len = 0);
	~AutoPTCHAR();

	inline operator unsigned short*() const
	{
		return (unsigned short*) m_ptch;
	}

	inline operator unsigned char*() const
	{
		return (unsigned char*) m_ptch;
	}

	inline bool IsValid()
	{
		if (m_ptch != NULL)
			return true;
		return false;
	};

	inline size_t GetBufferLength(){return m_BufferLen;};
	inline PTCHAR GetPTCH(){return m_ptch;};

private:
	PTCHAR m_ptch;
	CString* m_pstrBase;
	size_t m_BufferLen;
};

#endif // !defined(AFX_AUTOPTCHAR_H__6B12B83C_1290_41C0_B483_4932AA46B952__INCLUDED_)
