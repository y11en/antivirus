#include <stdio.h>
#include "compatutils.h"

char* _itoa ( int value, char *str, int radix )
{
  if ( !str )
    return 0;
  switch ( radix ) {
  case 8: sprintf ( str, "%o", value ); return str;
  case 10: sprintf ( str, "%d", value ); return str;
  case 16: sprintf ( str, "0x08%x", value ); return str;
  default:  return 0;
  }

}

