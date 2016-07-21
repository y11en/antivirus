#include <windows.h>
#include "SimpleBufferWindow.h"

CSimpleBufferWindow::CSimpleBufferWindow(tINT buff_size, ClearCallback_t ClearCallback, tDWORD context, tVOID* data):
	m_buff_size(buff_size),
	m_pos(0),
	m_ClearCallback(ClearCallback),
	m_callback_context(context),
	m_callback_data(data)
{
	try
	{
		m_buff = new tBYTE[m_buff_size];
	}
	catch(std::bad_alloc &)
	{
		PR_RPT(("Failed to alloc buffer for CSimpleBufferWindow"));
		m_buff = NULL;
	}
	if(!m_buff)
		m_buff_size = 0;
	else
		memset(m_buff, 0, m_buff_size);
};

CSimpleBufferWindow::~CSimpleBufferWindow()
{
	delete []m_buff;
}

tINT CSimpleBufferWindow::Insert(tBYTE* data, tINT data_size, tBOOL bClearIfNeeded, tBOOL bCallCallback)
{
	PRAutoLocker cs(m_cs);
	if(m_buff)
	{
		if(m_pos + data_size <= m_buff_size)
		{
			// Копируем данные в позицию m_pos
			memcpy(m_buff + m_pos, data, data_size);
			m_pos += data_size;
			return m_pos;
		}
		else if(data_size <= m_buff_size && bClearIfNeeded)
		{
			// Если данные целиком поместятся в буфере
			// Обнуляем буфер
			Clear(bCallCallback);
			memcpy(m_buff, data, data_size);
			m_pos += data_size;
			return m_pos;
		}
		else
		{
			// Операция невозможна
//			PR_RPT(("Failed to insert data to CSimpleBufferWindow"));
			return 0;
		}
	}
	return 0;
}

void CSimpleBufferWindow::Clear(tBOOL bCallClearCallback, tDWORD shift)
{
	PRAutoLocker cs(m_cs);
	
	if(m_ClearCallback && bCallClearCallback)
		m_ClearCallback(this, m_callback_context, m_callback_data);

	if(m_buff && m_buff_size && shift)
	{
		memmove(m_buff, m_buff + shift, m_pos - shift); 
		memset(m_buff + shift, 0, m_pos - shift);
		m_pos -= shift;
		return;
	}
	if ( m_buff && m_buff_size )
		memset(m_buff, 0, m_buff_size);
	m_pos = 0;
}

void CSimpleBufferWindow::Lock()
{   
	m_cs.lock(); 
}

void CSimpleBufferWindow::Unlock()
{ 
	m_cs.unlock(); 
}

tBYTE* CSimpleBufferWindow::View() const
{
	return m_buff;
}

tINT CSimpleBufferWindow::GetSize() const
{
	return m_pos;
}