#if !defined (__SWM_H__)
#define __SWM_H__

// SWM.h - common definitions and function prototypes
// 
//

#include "../AVP_data.h"

#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
# if defined( __cplusplus )
#pragma pack(1)
# endif
# define PACKSTRUCT __attribute ((packed))
#else
#pragma pack(push,1)
# define PACKSTRUCT 
#endif


#if (defined (__unix__))
 #define SWM_PARAM
#else
 #define SWM_PARAM __cdecl
#endif


#ifdef __cplusplus
#    define SWM_PROC extern "C" 
#else
#    define SWM_PROC
#endif

typedef AVP_bool (SWM_PARAM *BufferProc)( void* pBuffer, AVP_dword nSize, AVP_dword *pnOutSize, void* pUserData );

// ---
typedef struct {
	BufferProc	m_pProc;
} PACKSTRUCT SWM_Info;


typedef struct {
	void *pSWMHandle;
	void *pUserData;
}	PACKSTRUCT SWM_StreamInfo;

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
# if defined( __cplusplus )
#pragma pack()
# endif
#endif

// Init library
// pfMemAlloc - pointer to memory-alloc function
// pfMemFree  - pointer to memory-free function
SWM_PROC AVP_bool SWM_PARAM SWM_Init_Library( void* (*pfMemAlloc)(AVP_size_t), 
																							void (*pfMemFree)(void*) );


// Init library
// pfMemAlloc   - pointer to memory-alloc function
// pfMemFree    - pointer to memory-free function
// pfMemLock	  - pointer to inter-thread synchronization lock function
// pfMemUnlock	- pointer to inter-thread synchronization unlock function
// pContext     - pointer to inter-thread synchronization functions context
SWM_PROC AVP_bool  SWM_PARAM SWM_Init_LibraryEx( void* (*pfMemAlloc)(AVP_size_t), 
																								    void (*pfMemFree)(void*),
																										void (*pfMemLock)(void *), 
																										void (*pfMemUnlock)(void *), 
																										void* pContext );
// Deinit library
SWM_PROC AVP_bool SWM_PARAM  SWM_Deinit_Library( void );

// Create new instance of SW manager
// Can be called some times
// Return handle for giving to functions SWM_Prepare_XXX and SWM_Register_XXX
SWM_PROC void *	 SWM_PARAM SWM_Create_Manager();

// Prepare to write
// pSrcInfo and pDestInfo mustn't be 0 - it means library will work on global manager
// Can be called some times
// Return handle for placing into struct "SWM_StreamInfo"
SWM_PROC void * SWM_PARAM SWM_Prepare_To_Write( void *pManager, SWM_Info *pSrcInfo, SWM_Info *pDestInfo );

// Prepare to read
// pSrcInfo and pDestInfo mustn't be 0 - it means library will work on global manager
// Can be called some times
// Return handle for placing into struct "SWM_StreamInfo"
SWM_PROC void * SWM_PARAM SWM_Prepare_To_Read( void *pManager, SWM_Info *pSrcInfo, SWM_Info *pDestInfo );

// Shutdown library
// pMamager can be 0
SWM_PROC void SWM_PARAM SWM_Shut_Down_Library( void *pManager );


// Register Packer wrapper for writing
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Packer( void *pManager );

// Register Packer wrapper for reading
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Unpacker( void *pManager );


// Register Encrypter wrapper for writing
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Encrypter( void *pManager );

// Register Encrypter wrapper for reading
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_Decrypter( void *pManager );


// Register CRCChecker wrapper for writing
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_CRCOutChecker( void *pManager );

// Register CRCChecker wrapper for reading
// pMamager can be 0
SWM_PROC AVP_bool SWM_PARAM SWM_Register_CRCInChecker( void *pManager );

#endif

