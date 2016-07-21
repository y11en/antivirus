#ifndef _SIMPLEBUFFERWINDOW_H_
#define _SIMPLEBUFFERWINDOW_H_

#pragma warning( disable : 4786 )
#include <Prague/prague.h>
#include <map>
#include <Prague/pr_wrappers.h>

class CSimpleBufferWindow
{
public:
	typedef void (* ClearCallback_t)(CSimpleBufferWindow* _this, tDWORD context, tVOID* data);
	CSimpleBufferWindow(tINT buff_size, ClearCallback_t ClearCallback, tDWORD context, tVOID* data);
	~CSimpleBufferWindow();
	tINT Insert(tBYTE* data, tINT data_size, tBOOL bClearIfNeeded = cTRUE, tBOOL bCallCallback = cFALSE);
	void Clear(tBOOL bCallClearCallback = cTRUE, tDWORD shift = 0);
	void Lock();
	void Unlock();
	tBYTE* View() const;
	tINT GetSize() const;

private:
	tBYTE* m_buff;		// Буфер
	tINT m_buff_size;	// Размер буфера
	tINT m_pos;			// Текущая позиция
	PRLocker m_cs;
	
	ClearCallback_t m_ClearCallback;
	tDWORD m_callback_context;
	tVOID* m_callback_data;
};

#endif//_SIMPLEBUFFERWINDOW_H_