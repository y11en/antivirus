#include <ctype.h>
#include "compatutils.h"

char * _strlwr(char *x)
{
  char *y=x;
  while(*y){
    *y=tolower(*y);
    y++;
  }
  return x;
}

