#ifndef WOWREDIR_H__
#define WOWREDIR_H__

#include <windows.h>

class CWow64FsRedirectorDisabler
{
        typedef BOOL (WINAPI *tWow64DisableWow64FsRedirection)(PVOID *OldValue);
        typedef BOOL (WINAPI *tWow64RevertWow64FsRedirection)(PVOID OlValue);

        tWow64DisableWow64FsRedirection m_fnWow64DisableWow64FsRedirection;
        tWow64RevertWow64FsRedirection m_fnWow64RevertWow64FsRedirection;

        PVOID m_pOldValue;
        BOOL m_bWow64FsRedirectorDisabled;

public:

        CWow64FsRedirectorDisabler() :
		m_fnWow64DisableWow64FsRedirection(NULL),
        m_fnWow64RevertWow64FsRedirection(NULL),

        m_pOldValue(NULL),

         m_bWow64FsRedirectorDisabled(FALSE)

        {

			HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
			if(hKernel32)
			{
				m_fnWow64DisableWow64FsRedirection = (tWow64DisableWow64FsRedirection)GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
				m_fnWow64RevertWow64FsRedirection = (tWow64RevertWow64FsRedirection)GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");

				if(m_fnWow64DisableWow64FsRedirection)
					m_bWow64FsRedirectorDisabled = m_fnWow64DisableWow64FsRedirection(&m_pOldValue);
			}
        }

        ~CWow64FsRedirectorDisabler()
        {
			if(m_fnWow64RevertWow64FsRedirection && m_bWow64FsRedirectorDisabled)
			m_fnWow64RevertWow64FsRedirection(m_pOldValue);
        }

};


#endif //WOWREDIR_H__