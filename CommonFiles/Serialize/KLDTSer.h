#if !defined (__KLDTSER_H__)
#define __KLDTSER_H__

#include "../AVP_data.h"
#include "../AVPPort.h"
#include "../Property/Property.h"
#include "../SWManager/Swm.h"
#include "../AVPComID.h"
#include "KLDTDefs.h"

#if defined (__unix__) || defined(__MWERKS__)
	#define KLDT_PARAM
#else
	#define KLDT_PARAM __cdecl
	#if !defined(__KLDT_NO_UNICODE__)
		#define KLDT_REALISE_UNICODE
// Graf [7/8/2002]
	#include <tchar.h>
#endif
#endif


#ifdef __cplusplus
#    define KLDT_PROC extern "C" 
#else
#    define KLDT_PROC
#endif


// Init library
// pfMemAlloc - pointer to memory-alloc function
// pfMemFree  - pointer to memory-free function
KLDT_PROC AVP_bool  KLDT_PARAM KLDT_Init_Library( void* (*pfMemAlloc)(AVP_size_t), 
																								  void (*pfMemFree)(void*) );

// Init library
// pfMemAlloc   - pointer to memory-alloc function
// pfMemFree    - pointer to memory-free function
// pfMemLock	  - pointer to inter-thread synchronization lock function
// pfMemUnlock	- pointer to inter-thread synchronization unlock function
// pContext     - pointer to inter-thread synchronization functions context
KLDT_PROC AVP_bool  KLDT_PARAM KLDT_Init_LibraryEx( void* (*pfMemAlloc)(AVP_size_t), 
																								    void (*pfMemFree)(void*),
																										void (*pfMemLock)(void *), 
																										void (*pfMemUnlock)(void *), 
																										void* pContext );
// Deinit library
KLDT_PROC AVP_bool KLDT_PARAM  KLDT_Deinit_Library( AVP_bool bDeinitDATALib );

/////////////////////  Serialize to file part ///////////////////////////////////////////////

// Serialize data tree
// pszFileName    - pointer to out file name
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Serialize  ( const TCHAR *pszFileName, 
																							  HDATA hRootData, 
																								const TCHAR *pszDescription );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeW ( const wchar_t *pszFileName, 
																								HDATA hRootData, 
																								const wchar_t *pszDescription );
#endif

// Serialize data tree using SWM 
// pszFileName    - pointer to out file name
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingSWM( const TCHAR *pszFileName, 
																											HDATA hRootData, 
																											const TCHAR *pszDescription );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingSWMW( const wchar_t *pszFileName, 
																											 HDATA hRootData, 
																										   const wchar_t *pszDescription );
#endif

// Serialize data tree using customized SWM 
// pszFileName    - pointer to out file name
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
// pszCodeStr     - pointer to string with custumising codes
//                  NULL or empty - use KLDT_Serialize 
//                  'e'           - use Encrypter
//                  'c'           - use CRCChecker
//                  'p'           - use Packer
// Example - "ecp" - use Encrypter, CRCChecker and Packer in just this sequence 
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingCustomizedSWM( const TCHAR *pszFileName, 
																															  HDATA hRootData, 
																															  const TCHAR *pszDescription, 
																															  const TCHAR *pszCodeStr );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeUsingCustomizedSWMW( const wchar_t *pszFileName, 
																															   HDATA hRootData, 
																															   const wchar_t *pszDescription, 
																															   const wchar_t *pszCodeStr );
#endif

// Low level function serialize data tree
// pszFileName    - pointer to out file name
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
// pSWMInfo				- pointer to SWM_Info struct
// pSWMStreamInfo - pointer to SWM_StreamInfo struct
// bWriteRawDataHeader - whether standard header "RAW_DATA" must be written
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeWrite( const TCHAR *pszFileName, 
																									 HDATA hRootData, 
																									 const TCHAR *pszDescription,
																									 SWM_Info *pSWMInfo, 
																									 SWM_StreamInfo *pSWMStreamInfo,
																									 AVP_bool bWriteRawDataHeader );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeWriteW( const wchar_t *pszFileName, 
																									  HDATA hRootData, 
																									  const wchar_t *pszDescription,
																									  SWM_Info *pSWMInfo, 
																									  SWM_StreamInfo *pSWMStreamInfo,
																									  AVP_bool bWriteRawDataHeader );
#endif

// Low level function serialize standard headers
// hFile							 - handle of opened file
// pszDescription			 - pointer to file description
// bWriteRawDataHeader - whether standard header "RAW_DATA" must be written
KLDT_PROC AVP_bool KLDT_PARAM KLDT_SerializeHeaders( HANDLE hFile,
																									   const TCHAR *pszDescription,
																									   AVP_bool bWriteRawDataHeader );

/////////////////////  Serialize to memory part /////////////////////////////////////////////

// Serialize data tree to memory
// pBuffer        - pointer to serialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - handle of serialization tree root
// pszDescription - pointer to description
// If "pBuffer" or "nBufferSize" equal 0, amount of bytes necessary for serialization
// will be returned
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemory( AVP_char *pBuffer, 
																											 AVP_dword nBufferSize,
																											 HDATA hRootData, 
																											 const TCHAR *pszDescription );


// Serialize data tree to memory using SWM 
// pBuffer        - pointer to serialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
// If "pBuffer" or "nBufferSize" equal 0, amount of bytes necessary for serialization
// will be returned
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryUsingSWM( AVP_char *pBuffer, 
																															 AVP_dword nBufferSize,
																															 HDATA hRootData, 
																															 const TCHAR *pszDescription );

// Serialize data tree to memory using customized SWM 
// pBuffer        - pointer to serialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - handle of serialization tree root
// pszDescription - pointer to file description
// pszCodeStr     - pointer to string with custumising codes
//                  NULL or empty - use KLDT_Serialize 
//                  'e'           - use Encrypter
//                  'c'           - use CRCChecker
//                  'p'           - use Packer
// Example - "ecp" - use Encrypter, CRCChecker and Packer in just this sequence 
// If "pBuffer" or "nBufferSize" equal 0, amount of bytes necessary for serialization
// will be returned
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryUsingCustomizedSWM( AVP_char *pBuffer, 
																																			   AVP_dword nBufferSize,
																																				 HDATA hRootData, 
																																				 const TCHAR *pszDescription, 
																																				 const TCHAR *pszCodeStr );
// Low level function serialize data tree to memory
// pBuffer        - pointer to serialization buffer
// nBufferSize		- amount of bytes in buffer
// pszDescription - pointer to file description
// pSWMInfo				- pointer to SWM_Info struct
// pSWMStreamInfo - pointer to SWM_StreamInfo struct
// bWriteRawDataHeader - whether standard header "RAW_DATA" must be written
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeToMemoryWrite( AVP_char *pBuffer, 
																														AVP_dword nBufferSize,
																														HDATA hRootData,
																														const TCHAR *pszDescription,
																														SWM_Info *pSWMInfo, 
																														SWM_StreamInfo *pSWMStreamInfo, 
																														AVP_bool bWriteRawDataHeader );

// Low level function deserialize standard headers to memory
// pBuffer        - pointer to serialization buffer
// nBufferSize		- amount of bytes in buffer
// pszDescription - pointer to description
// bReadRawDataHeader - whether standard header "RAW_DATA" must be read
// If "pBuffer" or "nBufferSize" equal 0, amount of bytes necessary for serialization
// will be returned
KLDT_PROC AVP_dword KLDT_PARAM KLDT_SerializeHeadersToMemory( AVP_char *pBuffer, 
																														  AVP_dword nBufferSize,
																														  const TCHAR *pszDescription,
																															AVP_bool bWriteRawDataHeader );

/////////////////////  Deserialize from file part ////////////////////////////////////////////

// Deserialize data tree
// pszFileName    - pointer to in file name
// hRootData      - pointer to handle of tree root
//                  0  - handle of deserialized tree will be puted into it
//                  !0 - DATA_Attach to this handle will be completed 
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Deserialize( const TCHAR *pszFileName, 
																							  HDATA *hRootData );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeW( const wchar_t *pszFileName, 
																							   HDATA *hRootData );
#endif

// Deserialize data header
// pszFileName    - pointer to in file name
// pKLHeader      - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
//                   Should be freed by "free" function
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeader( const TCHAR *pszFileName, 
																											AVP_KL_Header *pKLHeader,
																											TCHAR **ppszDescription );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaderW( const wchar_t *pszFileName, 
																										   AVP_KL_Header *pKLHeader,
																										   wchar_t **ppszDescription );
#endif

// Deserialize data tree using SWM
// pszFileName    - pointer to in file name
// hRootData      - pointer to handle of tree root
//                  0  - handle of deserialized tree will be puted into it
//                  !0 - DATA_Attach to this handle will be completed 
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeUsingSWM( const TCHAR *pszFileName, 
																											  HDATA *hRootData );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeUsingSWMW( const wchar_t *pszFileName, 
																											   HDATA *hRootData );
#endif

// Low level function deserialize data tree
// pszFileName    - pointer to out file name
// hRootData      - handle of serialization tree root
// pSWMInfo				- pointer to SWM_Info struct
// pSWMStreamInfo - pointer to SWM_StreamInfo struct
// bReadRawDataHeader - whether standard header "RAW_DATA" must be read
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeRead( const TCHAR *pszFileName, 
																										HDATA *hRootData,
																										SWM_Info *pSWMInfo, 
																										SWM_StreamInfo *pSWMStreamInfo,
																										AVP_bool bReadRawDataHeader);
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeReadW( const wchar_t *pszFileName, 
																									   HDATA *hRootData,
																									   SWM_Info *pSWMInfo, 
																									   SWM_StreamInfo *pSWMStreamInfo,
																									   AVP_bool bReadRawDataHeader);
#endif

// Low level function deserialize standard headers
// hFile							 - handle of opened file
// bReadRawDataHeader - whether standard header "RAW_DATA" must be read
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaders( HANDLE hFile,
																											AVP_bool bReadRawDataHeader );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeadersW( HANDLE hFile,
																											AVP_bool bReadRawDataHeader );
#endif

// Low level function deserialize standard headers
// hFile					 - handle of opened file
// pKLHeader       - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeFullHeaders( HANDLE hFile,
																													 AVP_KL_Header *pKLHeader,
																													 TCHAR **ppDescription );
#ifdef KLDT_REALISE_UNICODE
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeFullHeadersW( HANDLE hFile,
																													  AVP_KL_Header *pKLHeader,
																													  wchar_t **ppDescription );
#endif

/////////////////////  Deserialize from memory part //////////////////////////////////////////

// Deserialize data tree from memory
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - pointer to handle of tree root
//                  0  - handle of deserialized tree will be puted into it
//                  !0 - DATA_Attach to this handle will be completed 
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFromMemory( AVP_char *pBuffer, 
																													 AVP_dword nBufferSize,
																													 HDATA *hRootData );

// Deserialize data header from memory
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// pKLHeader      - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
//                   Should be freed by "free" function
KLDT_PROC AVP_bool KLDT_PARAM KLDT_DeserializeHeaderFromMemory( AVP_char *pBuffer, 
																																AVP_dword nBufferSize,
																																AVP_KL_Header *pKLHeader,
																																TCHAR **ppszDescription );

// Deserialize data tree from memory using SWM
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - pointer to handle of tree root
//                  0  - handle of deserialized tree will be puted into it
//                  !0 - DATA_Attach to this handle will be completed 
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFromMemoryUsingSWM( AVP_char *pBuffer, 
																																	AVP_dword nBufferSize,
																																	HDATA *hRootData );

// Low level function deserialize data tree	from memory
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// hRootData      - handle of serialization tree root
// pSWMInfo				- pointer to SWM_Info struct
// pSWMStreamInfo - pointer to SWM_StreamInfo struct
// bReadRawDataHeader - whether standard header "RAW_DATA" must be read
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFromMemoryRead( AVP_char *pBuffer, 
																															 AVP_dword nBufferSize,
																															 HDATA *hRootData,
																															 SWM_Info *pSWMInfo, 
																															 SWM_StreamInfo *pSWMStreamInfo,
																															 AVP_bool bReadRawDataHeader);

// Low level function deserialize standard headers from memory
// pBuffer        - pointer to deserialization buffer
// nBufferSize		- amount of bytes in buffer
// bReadRawDataHeader - whether standard header "RAW_DATA" must be read
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeHeadersFromMemory( AVP_char *pBuffer, 
																																 AVP_dword nBufferSize,
																															   AVP_bool bReadRawDataHeader );

// Low level function deserialize standard headers from memory
// pBuffer         - pointer to deserialization buffer
// nBufferSize		 - amount of bytes in buffer
// pKLHeader       - pointer to KL header structure (might be 0)
// ppszDescription - pointer to pointer to buffer (allocated by function)	for description (might be 0)
KLDT_PROC AVP_dword KLDT_PARAM KLDT_DeserializeFullHeadersFromMemory( AVP_char *pBuffer, 
																																			AVP_dword nBufferSize,
																																			AVP_KL_Header *pKLHeader,
																																			TCHAR **ppszDescription );

#endif


