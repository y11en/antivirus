#if !defined (__SWMI_H__)
#define __SWMI_H__

// SWMI.h - common definitions for serialization wrappers and manager
// 
//

#include "SWManager/Swm.h"
#include "AVPComID.h"


#pragma pack(1)


typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef unsigned char   BYTE; 
typedef char            CHAR;



// ---
typedef struct SWContext {
	void				 *m_pBuffer;
	AVP_dword     m_nBufferSize;
	AVP_dword     m_nBufferFilled;
	void         *m_pCurrPosition; 
	AVP_bool      m_bHeaderProcessed;
	BufferProc    m_pReadProc;
	BufferProc    m_pWriteProc;
	BufferProc    m_pSiblingProc;
	int           m_nLevel;
	AVP_bool      m_bProcessingCompleted;
	AVP_bool      m_bProcessingStarted;
	void         *m_pData;
} SWContext;


#pragma pack()


#ifndef DECLARE_SW_INTERFACE  
#define DECLARE_SW_INTERFACE(iface)  																			\
																	typedef struct iface {									\
																	  unsigned int  m_cRef;							    \
																	  SWContext    *m_pSWContext;						\
																		struct iface##Vtbl *lpVtbl;				    \
																	} iface;																\
																	typedef struct iface##Vtbl iface##Vtbl; \
																	struct iface##Vtbl
#endif

#ifndef DECLARE_SWM_INTERFACE  
#define DECLARE_SWM_INTERFACE(iface)   																		\
																	typedef struct iface {									\
																	  unsigned int	m_cRef;							    \
																		int						m_nCurrLevel;						\
																		SWDescriptor *m_pSWTable;							\
																		int           m_nSWTableCount;				\
																		struct iface##Vtbl *lpVtbl;				    \
																	} iface;																\
																	typedef struct iface##Vtbl iface##Vtbl; \
																	struct iface##Vtbl

#define SWM_THIS_  INTERFACE *This,
#define SWM_THIS   INTERFACE *This
#define PURE
#endif

#undef  INTERFACE						
#define INTERFACE   SWControl

typedef struct SWManager _SWManager;

// ---
DECLARE_SW_INTERFACE(SWControl) {
	unsigned int (SWM_PARAM *AddRef)( SWM_THIS ) PURE;
	unsigned int (SWM_PARAM *Release) ( SWM_THIS ) PURE;

	AVP_dword     (SWM_PARAM *SWGetId)( SWM_THIS ) PURE;
	AVP_bool      (SWM_PARAM *SWAcceptBuffer)( SWM_THIS_ _SWManager *pManager, void *pBuffer, AVP_dword nBufferSize, void* pUserData ) PURE;
	AVP_bool      (SWM_PARAM *SWCreateInstance)( SWM_THIS_ _SWManager *pManager, SWControl **pSWControl ) PURE;
	AVP_bool      (SWM_PARAM *SWDuplicate)( SWM_THIS_ SWControl **pSWControl ) PURE;
};

#undef  INTERFACE						
#define INTERFACE   SWManager

// ---
typedef struct SWDescriptor {
	unsigned long  m_nSWId;
	SWControl			*m_piSWControl;
} SWDescriptor;



// ---
DECLARE_SWM_INTERFACE(SWManager) {
	unsigned int (SWM_PARAM *AddRef)( SWM_THIS ) PURE;
	unsigned int (SWM_PARAM *Release) ( SWM_THIS ) PURE;

	AVP_bool		 (SWM_PARAM *SWMIsSWRegistered)( SWM_THIS_ unsigned long nId );
	SWContext*   (SWM_PARAM *SWMGetContext)( SWM_THIS_ AVP_dword nId ) PURE;
	AVP_bool		 (SWM_PARAM *SWMAttachSW)( SWM_THIS_ AVP_dword nNewId, 
																				 AVP_dword nOldId, 
																				 void *pBuffer, AVP_dword nSize, void* pUserData ) PURE;
	void         (SWM_PARAM *SWMEnterBufferProc)( SWM_THIS ) PURE; 
	void         (SWM_PARAM *SWMLeaveBufferProc)( SWM_THIS ) PURE; 
};

#undef  INTERFACE						


SWM_PROC AVP_bool   SWM_PARAM SWMAdvise( SWManager *pManager, SWControl *iControl );

extern void* (* SWAllocator)(AVP_size_t);
extern void  (* SWLiberator)(void*);
extern SWManager *gpSWManager;

// Зарегистрировать RawData
SWM_PROC AVP_bool SWM_PARAM SWM_Register_RawData( void *pManager );
SWM_PROC AVP_bool SWM_PARAM SWM_Register_UnRawData( void *pManager );



#endif

