// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  05 February 2003,  16:09 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Anti-Virus Plugin Engine 3.0
// Author      -- Graf
// File Name   -- engine.h
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __engine_h__fb8c5bac_27c0_4e3a_9272_5d506cb93c4f )
#define __engine_h__fb8c5bac_27c0_4e3a_9272_5d506cb93c4f
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include "plugin_avp1.h"

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_posio.h>

#include <AV/iface/i_engine.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Engine interface implementation
// Short comments -- Базовый интерфейс обработки объектов.//eng:Generic interface for object processing.
//    Интерфейс определяет основные правила и методы обработки сложных объектов.
//    Служит для реализации как абстрактного антивирусного ядра, так и подключаемых сканеров.
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Engine_Register( hROOT root );
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure data is declared in O:\PRAGUE\AVP1\engine.c source file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
   #define plENGINE_USE_MULTITHREAD_DRIVER mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // engine_h
// AVP Prague stamp end



