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
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __posio_h__60bf738c_87ae_4c74_8b9b_d05345f51ba9 )
#define __posio_h__60bf738c_87ae_4c74_8b9b_d05345f51ba9
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <iface/i_root.h>
#include <iface/i_posio.h>
#include "plugin_temporaryio2.h"
#include <iface/i_io.h>
#include <iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define cMAX_PATH                      ((tDWORD)(260)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// PosIO interface implementation
// Short comments -- IO with Position
//    Полностью совместим с интерфейсом IO. Представляет упрощенный вариант интерфейса SeqIO. Имеет текущую позицию в IO и может читать/писать от этой позиции.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call PosIO_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure sLocals is declared in O:\PRAGUE\TempFile2\posio.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // posio_h
// AVP Prague stamp end



