// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  22 April 2003,  16:36 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин работы с временными файлами
// Author      -- Andy Nikishin
// File Name   -- posio.h
// Additional info
//    Этот плагин создан для работы с временными файлами. При этом файлы первоначально создаются в памяти. Если размер файла превышает некое пороговое значение (в текущей реализации 16 килобайт), происходит создание файла на диске, и в последующем идет работа с файлом через бефер памяти.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __posio_h__61bbf33b_9bd4_472a_a8e1_bb39bf481cbc )
#define __posio_h__61bbf33b_9bd4_472a_a8e1_bb39bf481cbc
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <IFace/i_root.h>
#include <IFace/i_posio.h>
#include "plugin_temporaryio.h"
#include <IFace/i_io.h>
#include <IFace/i_string.h>
#include <IFace/i_seqio.h>
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
  // data structure sLocals is declared in O:\Prague\TempFile\posio.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // posio_h
// AVP Prague stamp end



