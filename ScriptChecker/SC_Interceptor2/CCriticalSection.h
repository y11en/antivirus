/////////////////////////////////////////////////////////////////////////////
// CCriticalSection

class CCriticalSection
{
// Constructor
public:
	inline CCriticalSection()	{ ::InitializeCriticalSection(&m_sect); };

// Attributes
public:
	inline operator CRITICAL_SECTION*() { return (CRITICAL_SECTION*) &m_sect; };
	CRITICAL_SECTION m_sect;

// Operations
public:
	inline VOID Leave() { ::LeaveCriticalSection(&m_sect); };
	inline VOID Enter() { ::EnterCriticalSection(&m_sect); };

// Implementation
public:
	inline ~CCriticalSection() { ::DeleteCriticalSection(&m_sect); };
};
