#ifndef _MDIFTBL_H_
#define _MDIFTBL_H_  "main diff table maker"

#include <stdlib.h>
#include <memory.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/crc32.h"

tINT MakeDiffTable(tBYTE*  Old, tUINT  OldSize, tBYTE* New, tUINT NewSize,
                   tBYTE** Dif, tUINT* DifSize);

#endif /* _MDIFTBL_H_ */
