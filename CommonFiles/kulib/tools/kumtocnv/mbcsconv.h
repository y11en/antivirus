

#ifndef __MBCSCONV_H
#define __MBCSCONV_H

#include "kum.h"

#include <stdio.h>

struct mbcsconv;
typedef struct mbcsconv mbcsconv;

mbcsconv* mbcsconv_create( kum_file* kum );
void mbcsconv_write( mbcsconv* cnv, FILE* fp );



#endif /* __MBCSCONV_H */

