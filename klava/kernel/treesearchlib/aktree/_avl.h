/*-----------------11.01.99 12:27-------------------
 * Std procedure to handle AVL trees              *
 *                                                *
 * internal header file                           *
--------------------------------------------------*/

#include "../common.h"

#ifndef AVL_DEPTH
#define AVL_DEPTH 32                /* maximum depth of tree */

typedef int   (*compareFunc)(void *, void *);
typedef struct
{
    struct sTREENODE *root;
//    void *(*alloc)(uint32_t count);
//    void  (*free)(void*);
	sHEAP_CTX* pHeapCtx;
    compareFunc compare;
} thTREE, *tTREE;


typedef struct sTREENODE
{
    struct sTREENODE *left;
    struct sTREENODE *child[2];
    int balance;
    uint32_t   count;
} thNODE, *tNODE   ;

#define TREEHANDLE tTREE
#define TREENODE   tNODE


#define AVL_LEFT     -1
#define AVL_BALANCED  0
#define AVL_RIGHT     1
#define AVL_PARENT    0

typedef struct
{
    void *        data;                     /* data to add/remove */
    uint32_t      count;                    /* size of data */
    int           error;
    tTREE         tree;                     /* tree root */
    tNODE      *  nodeptr;                  /* pointer to current node */
    signed char * pathptr;
    struct sTREENODE *node[AVL_DEPTH];      /* node path */
    signed char       path[AVL_DEPTH];      /* balance path */
    int  level;                             /* current level */
}
tTREEDATA;

#endif
