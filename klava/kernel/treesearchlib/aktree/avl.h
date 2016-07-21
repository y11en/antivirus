/*-----------------11.01.99 12:27-------------------
 * Std procedure to handle AVL trees              *
 *                                                *
 * header file                                    *
--------------------------------------------------*/

#include "aktypes.h"
#include "../common.h"

#ifndef TREEHANDLE
#define TREEHANDLE uint32_t
#define TREENODE   uint32_t
#endif

/* manipulating trees itself */

#ifdef __cplusplus
extern "C" {
#endif

TREEHANDLE _TreeCreate(int (*compare)(void *,void *), sHEAP_CTX* pHeapCtx);
void       _TreeDelete(TREEHANDLE tree);

/* searching tree */
TREENODE   _TreeSearch  (TREEHANDLE tree, void *data, int count);
TREENODE   _TreeAtLeast (TREEHANDLE tree, void *data, int count);
TREENODE   _TreeAtMost  (TREEHANDLE tree, void *data, int count);

TREENODE   _TreeFirst (TREEHANDLE tree);

TREENODE   _TreeLast  (TREEHANDLE tree);
TREENODE   _TreeNext  (TREENODE node);
TREENODE   _TreePrev  (TREENODE node);

/* node manipulating */
int        _TreeAdd   (TREEHANDLE tree, void *data, int count);
int        _TreeRemove(TREEHANDLE tree, void *data, int count);

/* node info extraction */
void*      _TreeNodeData(TREENODE node);
int        _TreeNodeSize(TREENODE node);

#ifdef __cplusplus
} // extern "C"
#endif
