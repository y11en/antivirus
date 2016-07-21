#define __kernel_h
#include "custview.h"
#include "../../loader/win32/wheap.h"

typedef struct tag_HEAP {
	struct tag_hi_Heap obj;
	HeapData           data;
} tHEAP;

// ---
HRESULT WINAPI phHeap( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {

	__try {
		tHEAP   heap;
		DWORD   dwErr;
		HRESULT hr;

		memset( pOutputStr, 0, dwMaxSize );

		if ( ((DWORD)address) == 0 ) {
			lstrcpyn( pOutputStr, "heap:null pointer", dwMaxSize );
			return S_OK;
		}
		if ( ((DWORD)address) == 0xccccccccl ) {
			lstrcpyn( pOutputStr, "heap:not initialized", dwMaxSize );
			return S_OK;
		}
		
		hr = pHelper->ReadDebuggeeMemory( pHelper, address, sizeof(tHEAP), &heap, &dwErr );
		if ( FAILED(hr) ) {
			lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
			return S_OK;
		}

		wsprintf( pOutputStr, "heap(c:%04u,s:%u)", heap.data.m_allocated_count, heap.data.m_requested_size );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpyn( pOutputStr, "cannot parse address", dwMaxSize );
	}
	return S_OK;
}



