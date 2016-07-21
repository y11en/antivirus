#include "stdafx.h"

void MyHandleError(char *s)
{
    PR_TRACE( (NULL, prtNOTIFY,"wincript => SSL:MyHandleError => %x, %s\n", GetLastError(), s));
}
