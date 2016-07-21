/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sync_items.h
 * \author	Andrew Kazachkov
 * \date	12.08.2003 12:02:33
 * \brief	
 * 
 */

#ifndef __KL_SYNC_ITEMS_H__
#define __KL_SYNC_ITEMS_H__

#include <set>

namespace KLSRV
{    
    class CSyncItems
    {
    public:
        CSyncItems();
        ~CSyncItems();
        bool TryEnter(const std::wstring& wstrItem);
        void Leave(const std::wstring& wstrItem);
    protected:
        std::set<std::wstring>                  m_setItems;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    };

    class CLockItem
    {
    public:
        CLockItem(CSyncItems& storage, const std::wstring& wstrItem)
            :   m_wstrItem(wstrItem)
            ,   m_Storage(storage)
        {
            m_bResult=m_Storage.TryEnter(m_wstrItem);
        };

        ~CLockItem()
        {
            if(m_bResult)
                m_Storage.Leave(m_wstrItem);
        };
        operator bool()
        {
            return m_bResult;
        }
    protected:
        CSyncItems&         m_Storage;
        const std::wstring  m_wstrItem;
        bool                m_bResult;
    };
}

#endif //__KL_SYNC_ITEMS_H__