// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  25 September 2007,  16:27 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_as_trainsupport.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_as_trainsupport )
#define __public_plugin_as_trainsupport
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Mail/iface/i_as_trainsupport.h>
#include <Mail/iface/i_addressbookextractor.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_AS_TRAINSUPPORT  ((tPID)(40022))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, AS_TrainSupport )
// AS_TrainSupport interface implementation
// Short comments --

// message class
#define pmc_AS_TRAIN_STATUS 0x19acf64d //
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, AddressBookExtractor )
// AddressBookExtractor interface implementation
// AVP Prague stamp end



// the message pm_AS_BEGIN_TRINING is sent to gui to initiate trainnig
#define pm_AS_BEGIN_TRAINING  0xad91afc7 //is a crc32 for "pm_AS_BEGIN_TRAINING"

// the message pm_AS_TRAIN_STATUS is sent to gui then the trainig status changes
#define pm_AS_TRAIN_STATUS    0x405d3a3c //is a crc32 for "pm_AS_TRAIN_STATUS"

// the message pm_AS_TRAIN_STATUS is sent to gui then the trainig completes
#define pm_AS_FINISH_TRAINING 0xbd3ecaf8 //is a crc32 for "pm_AS_FINISH_TRAINING"

// gui send this message to terminate trainig process
#define pm_AS_CANCEL_TRAINING 0x187f2cd6 //is a crc32 for "pm_AS_CANCEL_TRAINING"

// gui send this message to skip further trainig
#define pm_AS_SKIP_TRAINING   0x6e16989b //is a crc32 for "pm_AS_SKIP_TRAINING"

// gui send this message to skip further trainig
#define pm_AS_APPLY_RESULTS   0x9b7bcdea //is a crc32 for "pm_AS_APPLY_RESULTS"
#define pm_AS_MERGE_RESULTS   0x7f021dae /*is a crc32 for "pm_AS_MERGE_RESULTS"*/

// gui send this message to skip further trainig
#define pm_AS_CANCEL_RESULTS  0x25032ec1 //is a crc32 for "pm_AS_CANCEL_RESULTS"


// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_as_trainsupport
// AVP Prague stamp end



