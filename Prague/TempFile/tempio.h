// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  18 April 2003,  09:56 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Base Technologies
// Purpose     -- Предоставляет виртуальное IO
// Author      -- Pavlushchik
// File Name   -- posio.h
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
// AVP Prague stamp end


// AVP Prague stamp begin( Data structure,  )
// Interface TempIO. Inner data structure

typedef struct tag_sTempLocals {
	hIO      real_io;                   // Handle to NativeIO if created
	tDWORD   m_dwfAccessMode;           // --
	tDWORD   m_dwfOpenMode;             // --
	tQWORD   FileSize;                  // --
	tCHAR    m_szFileName[cMAX_PATH];   // Имя временного файла, если таковой был создан
	tCHAR    m_szPath[cMAX_PATH];       // Путь к временному файлу, если таковой был создан
	tORIG_ID m_ObjectOrigin;            // --
	tBYTE    byFillChar;                // --
	tBOOL    bDeleteOnClose;            // --
	hOBJECT  hBasedOn;                  // --
	hSTRING  hStr;                      // --
	tCHAR    m_szObjectName[cMAX_PATH]; // Имя объекта (может не совпадать с именем файла)
	tCHAR    m_szObjectPath[cMAX_PATH]; // Путь к объекту (может не совпадать с путем к файлу)
	tERROR   WriteBackError;            // --
	tQWORD   m_qwCurPos;                // IO current position
	tQWORD   SwapSize;                  // --
} sTempLocals;

// AVP Prague stamp end


// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call TempIO_ObjectInit( hi_TempIO _this );
tERROR pr_call TempIO_ObjectInitDone( hi_TempIO _this );
tERROR pr_call TempIO_ObjectPreClose( hi_TempIO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
// Standard IO methods
tERROR pr_call TempIO_SeekRead( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call TempIO_SeekWrite( hi_TempIO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call TempIO_GetSize( hi_TempIO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call TempIO_SetSize( hi_TempIO _this, tQWORD new_size );
tERROR pr_call TempIO_Flush( hi_TempIO _this );
// Additional TempIO methods
tERROR pr_call TempIO_Seek( hi_TempIO _this, tQWORD* result, tLONGLONG offset, tDWORD origin );
tERROR pr_call TempIO_Read( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count );
tERROR pr_call TempIO_Write( hi_TempIO _this, tDWORD* result, tPTR buffer, tDWORD count );
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call TempIO_PROP_CommonGet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call TempIO_PROP_CommonSet( hi_TempIO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end

