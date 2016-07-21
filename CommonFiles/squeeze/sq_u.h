#ifndef UNSQUEEZE_H
#define UNSQUEEZE_H

#include <stdio.h>

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

namespace Squeeze 
{
    /////////////////////////////////////////////////////////////////////////////
    // SQ compressor header file

    DWORD unsqu (BYTE *i_buf, BYTE *o_buf);

};

#endif

