// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  22 April 2003,  14:21 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин работы с временными файлами
// Author      -- Andy Nikishin
// File Name   -- plugin_temporaryio.h
// Additional info
//    Этот плагин создан для работы с временными файлами. При этом файлы первоначально создаются в памяти. Если размер файла превышает некое пороговое значение (в текущей реализации 16 килобайт), происходит создание файла на диске, и в последующем идет работа с файлом через бефер памяти.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_temporaryio )
#define __pligin_temporaryio
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define TEMPIO_ID  ((tPID)(PID_TMPFILE))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_temporaryio
// AVP Prague stamp end



