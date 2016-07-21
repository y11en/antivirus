#ifndef _ADIFTBL_H_
#define _ADIFTBL_H_  "diff table resolver"

#include <stdlib.h>
#include <memory.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/crc32.h"

tINT ApplyDiffTable(tBYTE*  Old, tUINT  OldSize, tBYTE* Dif, tUINT DifSize,
                    tBYTE** New, tUINT* NewSize);

#endif /* _ADIFTBL_H_ */
