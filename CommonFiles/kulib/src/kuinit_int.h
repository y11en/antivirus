

#ifndef __KUINIT_INT_H
#define __KUINIT_INT_H

#include "kulib/ku_defs.h"

KU_CDECL_BEGIN

int _kuconv_init( void );
int _kuconv_term( void );

int _kuconv_alias_init( void );
int _kuconv_alias_term( void );

int _kuconv_case_init( void );
int _kuconv_case_term( void );

int _kudata_init( void );
int _kudata_term( void );

KU_CDECL_END

#endif /* __KUINIT_INT_H */
