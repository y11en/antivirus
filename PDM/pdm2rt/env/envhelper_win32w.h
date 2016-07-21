#ifndef _cEnvironmentHelperWin32W_H
#define _cEnvironmentHelperWin32W_H

#include <windows.h>
#include "../../pdm2/eventmgr/include/envhelper.h"

#include "../../Windows/Hook/mklif/mklapi/mklapi.h"


typedef void* ( __cdecl *w32_pfMemAlloc )(PVOID pOpaquePtr, size_t, ULONG );
typedef void ( __cdecl *w32_pfMemFree )( PVOID pOpaquePtr, void** );


typedef enum _eHandleType
{
	_ht_none	= 0,
	_ht_drv		= 1,
	_ht_native	= 2,
};

class HandleImp
{
public:

	HandleImp ()
	{
		m_HandleType  = _ht_none;

		m_pClientContext = NULL;
		m_pMessage = NULL;
		m_MessageSize = 0;

		m_hFile = INVALID_HANDLE_VALUE;
	}

	HandleImp (
		PVOID pClientContext,
		PVOID pMessage,
		ULONG MessageSize
		)
	{
		ConnectHandle( pClientContext, pMessage, MessageSize );
	}

	HandleImp (
		PWCHAR pwchFileName
		)
	{
		ConnectHandle( pwchFileName );
	}

	void
	ConnectHandle (
		PVOID pClientContext,
		PVOID pMessage,
		ULONG MessageSize
		);

	void
	ConnectHandle (
		PWCHAR pwchFileName
		);


	~HandleImp();

	bool IsValid()
	{
		bool bValid = false;
		switch (m_HandleType)
		{
		case _ht_native:
			if (INVALID_HANDLE_VALUE != m_hFile)
				bValid = true;
			break;

		case _ht_drv:
			bValid = true;
			break;
		}

		return bValid;
	}

	bool FileRead (
		uint64_t pos,
		void* buff,
		uint32_t buff_size,
		uint32_t* bytes_read
		);

	uint64_t FileSize()
	{
		if (!IsValid())
			return 0;

		switch (m_HandleType)
		{
		case _ht_native:
			{
				LARGE_INTEGER size;
				size.LowPart = GetFileSize( m_hFile, (DWORD*)&size.HighPart );
				
				return (unsigned) size.QuadPart;
			}
			break;
		
		case _ht_drv:
			{
				HRESULT hResult = S_OK;

				uint64_t size = 0;
				
				MKLIF_OBJECT_REQUEST Request;
				ULONG BufSize = sizeof(size);
				
				memset(&Request, 0, sizeof(Request));
				Request.m_RequestType = _object_request_get_size;
				
				hResult = MKL_ObjectRequest( 
					m_pClientContext, 
					m_pMessage,
					&Request,
					sizeof(Request),
					&size,
					&BufSize );
				
				if (SUCCEEDED( hResult ) && BufSize)
					return size;
			}

			break;
		}
		
		return 0;
	}

	_eHandleType GetHandleType ()
	{
		return m_HandleType;
	};

private:
	_eHandleType m_HandleType;

	PVOID m_pClientContext;
	PVOID m_pMessage;
	ULONG m_MessageSize;

	HANDLE m_hFile;
};

class cEnvironmentHelperWin32W : public cEnvironmentHelper 
{
public:
	cEnvironmentHelperWin32W(PVOID pOpaquePtr, w32_pfMemAlloc pfW32Alloc, w32_pfMemFree pfW32Free);
	~cEnvironmentHelperWin32W();

	// memory
	_IMPLEMENT( Alloc );
	_IMPLEMENT( Free );

	// environment
	_IMPLEMENT( ExpandEnvironmentStr );
	
	// Interlocked Variable Access
	_IMPLEMENT( InterlockedInc) ;
	_IMPLEMENT( InterlockedDec );
	_IMPLEMENT( InterlockedExchAdd );

	// Path 
	_IMPLEMENT( PathGetLong );
	_IMPLEMENT( PathIsNetwork );
	_IMPLEMENT( PathFindExecutable );

	// File operations
	_IMPLEMENT( FileAttr );
	_IMPLEMENT( FileOpen );
	_IMPLEMENT( FileClose );
	_IMPLEMENT( FileRead );
	_IMPLEMENT( FileSizeByHandle );
	_IMPLEMENT( FileSizeByName );

	// Registry
	_IMPLEMENT( Reg_OpenKey );
	_IMPLEMENT( Reg_EnumValue );
	_IMPLEMENT( Reg_QueryValue );
	_IMPLEMENT( Reg_QueryValueStr );
	_IMPLEMENT( Reg_CloseKey );

	// Process
	_IMPLEMENT( ProcessVirtualQuery );
	_IMPLEMENT( GetTimeLocal );

	_IMPLEMENT( DriverPending );

private:
	PVOID m_pOpaquePtr;
	w32_pfMemAlloc m_pfAlloc;
	w32_pfMemFree m_pfFree;
};


#endif _cEnvironmentHelper_PDM_H