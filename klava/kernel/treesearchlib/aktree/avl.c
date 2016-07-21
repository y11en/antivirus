/*-----------------11.01.99 12:27-------------------
* Std procedure to handle AVL trees              *
*                                                *
* source code                                    *
--------------------------------------------------*/
#include <memory.h>

#include "aktypes.h"

#include "_avl.h"
#include "avl.h"

/* default compare procedure */
/*
// now inlined
static int lcompare(uint32_t *num1, uint32_t *num2)
{
    int rcode;
    if ( *num1 == *num2 )
    {
        rcode = 0;
    }
    else if ( *num1 < *num2 )
    {
        rcode = -1;
    }
    else
    {
        rcode = 1;
    }
    return rcode;
}
*/

/*
* internal _TreeSearch
*
* searches for given data in tree
* return node  ( NULL - node not found)
*
* save path in data
*
*/

static tNODE __fastcall __TreeSearch(tNODE node,tTREEDATA *data)
{
    int res;
    while ( node != NULL )
    {
		compareFunc compare = data->tree->compare;
		if (compare)
			res = data->tree->compare(data->data,(void*)((uint8_t*)(node)+sizeof(thNODE)));
		else
		{
			uint32_t num1 = *(uint32_t*)data->data;
			uint32_t num2 = *(uint32_t*)((uint8_t*)(node)+sizeof(thNODE));
			if ( num1 == num2 )
			{
				res = 0;
			}
			else if ( num1 < num2 )
			{
				res = -1;
			}
			else
			{
				res = 1;
			}
		}

        if ( res == AVL_BALANCED )
        {
            break;
        }
        *data->nodeptr++ = node;   /* from this node */
        *data->pathptr++ = res;    /* on this side */
        data->level++;
        node = node->child[res];
    }
    return node;
}

int _TreeAdd(tTREE tree, void *data, int count)
{
    tNODE node;
    tNODE *tnode;
    tNODE left, right;
    signed char *tpath, res;
	
    tTREEDATA tdata;
	
    tdata.data  = data;
    tdata.count = count;
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = tree;
    tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tree->root,&tdata);
	
    if ( node == NULL )
    {
        int taller;
        /* node not found */
		
        /* add new node in level position */
//        node = tdata.tree->alloc(sizeof(thNODE)+count);
        (void)_HeapAlloc2(tdata.tree->pHeapCtx, &node, (sizeof(thNODE)+count));	
		if (!node)
			return 3;
        node->child[AVL_BALANCED] = (tNODE)(tree);
        node->child[AVL_LEFT] = NULL;
        node->child[AVL_RIGHT] = NULL;
        node->balance = AVL_BALANCED;
        node->count = count;
        memcpy(((uint8_t*)(node))+sizeof(thNODE),data,count);
		
        /* balance up tree */
		
        tdata.level--;
        tnode = tdata.node+tdata.level;
        tpath = tdata.path+tdata.level;
        taller = 1;
		
        while ( tdata.level >= 0 )
        {
            /* balance tree from this node */
            (*tnode)->child[*tpath] = node;
            res = *tpath;
			
            node = *tnode;
			
            if ( taller )
            {
                switch ( node->balance * res )
                {
				case AVL_LEFT:
					/* same side */
					node->balance = AVL_BALANCED;
					taller = 0;
					break;
					
				case AVL_BALANCED:
					node->balance = res;
					break;
					
				case AVL_RIGHT:
					
					/* different sides, rebalance tree */
					
					left = node->child[res];
					
					switch ( left->balance *(-res) )
					{
					case AVL_LEFT:
						node->balance = left->balance = AVL_BALANCED;
						/* Rotate right */
						node->child[res] = left->child[-res];
						left->child[-res] = node;
						node = left;
						taller = 0;
						break;
						
					case AVL_BALANCED:
						/* balance error */
						//printf("BALANCE ERROR\n");
						tdata.error = -2;
						break;
						
					case AVL_RIGHT:
						right = left->child[-res];
						/* was without (-res) */
						switch ( right->balance * res )
						{
						case AVL_RIGHT:
							node->balance = -res;
							left->balance = AVL_BALANCED;
							break;
							
						case AVL_BALANCED:
							node->balance = left->balance = AVL_BALANCED;
							break;
							
						case AVL_LEFT:
							node->balance = AVL_BALANCED;
							/* was '-res' */
							left->balance = res;
							break;
						}
						right->balance = AVL_BALANCED;
						left->child[-res] = right->child[res];
						right->child[res] = left;
						
						node->child[res] = right->child[-res];
						right->child[-res] = node;
						
						node = right;
						taller = 0;
						break;
					}
					break;
                }
            }
			
            tnode--;
            tpath--;
            tdata.level--;
        }
        tree->root = node;
    }
    else
    {
        /* node found - cannot be added */
        node = NULL;
        tdata.error = 1;
    }
    return tdata.error;
}

int _TreeRemove(tTREE tree, void *data, int count)
{
    tNODE node, delnode;
    tTREEDATA tdata;
    tNODE *tnode;
    tNODE left, right;
    signed char *tpath;
    signed char res;
    int taller;
	
    tdata.data  = data;
    tdata.count = count;
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = tree;
	tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tree->root,&tdata);
	
    if ( node == NULL )
    {
        /* node not found - cannot be removed */
        tdata.error = 1;
    }
    else
    {
        /* node found - remove it */
        if ( node->child[AVL_LEFT] != NULL && node->child[AVL_RIGHT] != NULL )
        {
			uint32_t level = tdata.level;
			/* goes right look for leftmost node */
			
            /* save parent of removed node */
            tnode = tdata.nodeptr-1;
            tpath = tdata.pathptr-1;
			
            /* step right */
            *tdata.nodeptr++ = node;         /* from this node */
            *tdata.pathptr++ = AVL_RIGHT;    /* on this side */
            tdata.level++;
            node = node->child[AVL_RIGHT];
			
            /* goes left */
            while ( node->child[AVL_LEFT] != NULL )
            {
                *tdata.nodeptr++ = node;   /* from this node */
                *tdata.pathptr++ = AVL_LEFT;    /* on this side */
                tdata.level++;
                node = node->child[AVL_LEFT];
            }
			
            /* exchange nodes  node <-> *tnode */
            /* node   - leftmost node in right tree */
            /* *tnode - parent of node for delete */
			
			if ( level == 0 )
			{
				tdata.tree->root = node;
			}
			else
			{
				(*tnode)->child[*tpath] = node;
			}

            /* get next node in list (node to delete) */
            tnode++;
			
            /* change left child */
            node->child[AVL_LEFT] = (*tnode)->child[AVL_LEFT];
            (*tnode)->child[AVL_LEFT] = NULL;
			
            /* change right child */
            delnode = node->child[AVL_RIGHT];
            node->child[AVL_RIGHT] = (*tnode)->child[AVL_RIGHT];
            (*tnode)->child[AVL_RIGHT] = delnode;
			
            /* change node balance */
            res = node->balance;
            node->balance = (*tnode)->balance;
            (*tnode)->balance = res;
			
            /* change node pointer */
            delnode = node;
            node = *tnode;
            *tnode = delnode;
			
        }
		
        /* check node's children */
        delnode = node;
        if ( node->balance == AVL_BALANCED )
        {
            /* no children */
            node = NULL;
        }
        else
        {
            /* one child */
            node = node->child[node->balance];
        }
		
        /* delallocate node */
//        tree->free(delnode);
        _HeapFree(tree->pHeapCtx, delnode);
		
        /* balance up tree */
        tdata.level--;
        tdata.nodeptr--;
        tdata.pathptr--;
        taller = 1;
		
        while ( tdata.level-- >= 0 )
        {
            /* balance tree from this node */
            (*tdata.nodeptr)->child[*tdata.pathptr] = node;
			
            res  = *tdata.pathptr--;
            node = *tdata.nodeptr--;
			
            if ( taller )
            {
                switch ( node->balance * res )
                {
				case AVL_LEFT:
					/* different sides */
					right = node->child[-res];
					
					switch ( right->balance * res )
					{
					case AVL_LEFT:
						/* other child on other side */
						node->balance = right->balance = AVL_BALANCED;
						
						/* single rotation */
						node->child[-res] = right->child[res];
						right->child[res] = node;
						node = right;
						break;
						
					case AVL_BALANCED:
						/* other child is balanced */
						node->balance = -res;
						right->balance = res;
						taller = 0;
						
						/* single rotation */
						node->child[-res] = right->child[res];
						right->child[res] = node;
						node = right;
						break;
						
					case AVL_RIGHT:
						/* other child on same side */
						left = right->child[res];
						switch ( left->balance * res )
						{
						case AVL_LEFT:
							node->balance = res;
							right->balance = AVL_BALANCED;
							break;
							
						case AVL_BALANCED:
							node->balance = right->balance = AVL_BALANCED;
							break;
							
						case AVL_RIGHT:
							node->balance = AVL_BALANCED;
							right->balance = -res;
							break;
						}
						left->balance = AVL_BALANCED;
						
						/* double rotation */
						node->child[-res] = left->child[res];
						left->child[res] = node;
						right->child[res] = left->child[-res];
						left->child[-res] = right;
						node = left;
					}
					
					break;
					
                    case AVL_BALANCED:
                        /* balanced state before */
                        node->balance = -res;
                        taller = 0;
                        break;
						
                    case AVL_RIGHT:
                        /* same sides */
                        node->balance = AVL_BALANCED;
                        break;
                }
				
            }
        }
        tree->root = node;
    }
    return tdata.error;
}

tNODE _TreeSearch(tTREE tree, void *data, int count)
{
    tTREEDATA tdata;
	
    tdata.data  = data;
    tdata.count = count;
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = tree;
	tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    return __TreeSearch(tree->root,&tdata);
}

tTREE _TreeCreate(compareFunc compare, sHEAP_CTX* pHeapCtx)
{
    tTREE tree;
    tree = NULL;
	
    if ( pHeapCtx != NULL)
    {
        (void)_HeapAlloc2(pHeapCtx, &tree, sizeof(thTREE));
        if ( tree != NULL )
        {
            tree->root =  NULL;
//            tree->alloc = alloc;
//            tree->free =  free;
			tree->pHeapCtx = pHeapCtx;
//            if ( compare == NULL )
//            {
//                tree->compare = lcompare;
//            }
//			else
//			{
				tree->compare = compare;
//			}
        }
    }
    return tree;
	
}

static void __fastcall __TreeDelete(tNODE node, sHEAP_CTX* pHeapCtx)
{
    if ( node != NULL )
    {
        __TreeDelete(node->child[AVL_LEFT],pHeapCtx);
        __TreeDelete(node->child[AVL_RIGHT],pHeapCtx);
        //free(node);
        _HeapFree(pHeapCtx, node);
    }
}

void _TreeDelete(tTREE tree)
{
    __TreeDelete(tree->root,tree->pHeapCtx);
	_HeapFree(tree->pHeapCtx, tree);
}

void      *_TreeNodeData(TREENODE node)
{
    return (uint8_t*)(node)+sizeof(thNODE);
}

int        _TreeNodeSize(TREENODE node)
{
    return node->count;
}

TREENODE   _TreeFirst (TREEHANDLE tree)
{
    tNODE node;
	
    node = tree->root;
	
    while ( node != NULL && node->child[AVL_LEFT] != NULL)
    {
        node = node->child[AVL_LEFT];
    }
	
    return node;
}

TREENODE   _TreeLast  (TREEHANDLE tree)
{
    tNODE node;
	
    node = tree->root;
	
    while ( node != NULL && node->child[AVL_RIGHT] != NULL)
    {
        node = node->child[AVL_RIGHT];
    }
	
    return node;
}

TREENODE   _TreeNext  (TREENODE node)
{
    tTREEDATA tdata;
	
    tdata.data  = _TreeNodeData(node);
    tdata.count = _TreeNodeSize(node);
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = (tTREE)(node->child[AVL_PARENT]);
    tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tdata.tree->root,&tdata);
	
    if ( node != NULL )
    {
        if ( node->child[AVL_RIGHT] != NULL )
        {
            /* step right */
            node = node->child[AVL_RIGHT];
			
            /* then goes left */
            while ( node->child[AVL_LEFT] != NULL )
            {
                node = node->child[AVL_LEFT];
            }
        }
        else
        {
            /* goes to previous node with left step */
            node = NULL;
            while ( tdata.level > 0 )
            {
                tdata.pathptr--;
                tdata.nodeptr--;
				
                if ( *tdata.pathptr == AVL_LEFT )
                {
                    /* last left found, so it is result */
                    node = *tdata.nodeptr;
                    break;
                }
                tdata.level--;
            }
        }
    }
    return node;
}

TREENODE   _TreePrev  (TREENODE node)
{
    tTREEDATA tdata;
	
    tdata.data  = _TreeNodeData(node);
    tdata.count = _TreeNodeSize(node);
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = (tTREE)(node->child[AVL_PARENT]);
    tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tdata.tree->root,&tdata);
	
    if ( node != NULL )
    {
        if ( node->child[AVL_LEFT] != NULL )
        {
            /* step LEFT */
            node = node->child[AVL_LEFT];
			
            /* then goes right */
            while ( node->child[AVL_RIGHT] != NULL )
            {
                node = node->child[AVL_RIGHT];
            }
			
        }
        else
        {
            /* goes to previous node with right step */
            node = NULL;
            while ( tdata.level > 0 )
            {
                tdata.pathptr--;
                tdata.nodeptr--;
				
                if ( *tdata.pathptr == AVL_RIGHT )
                {
                    /* last left found, so it is result */
                    node = *tdata.nodeptr;
                    break;
                }
                tdata.level--;
            }
        }
    }
    return node;
}

TREENODE   _TreeAtLeast (TREEHANDLE tree, void *data, int count)
/* returns the first node comparing greater to or equal to the key */
{
    tTREEDATA tdata;
    tNODE node;
	
    tdata.data  = data;
    tdata.count = count;
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = tree;
    tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tdata.tree->root,&tdata);
	
    /* find nearest left step */
    if ( node == NULL )
    {
        while ( tdata.level > 0 )
        {
            tdata.pathptr--;
            tdata.nodeptr--;
			
            if ( *tdata.pathptr == AVL_LEFT)
            {
                /* last left found, so it is result */
                node = *tdata.nodeptr;
                break;
            }
            tdata.level--;
        }
    }
    return node;
}

TREENODE   _TreeAtMost  (TREEHANDLE tree, void *data, int count)
/* returns the first node comparing less then or equal to the key */
{
    tTREEDATA tdata;
    tNODE node;
	
    tdata.data  = data;
    tdata.count = count;
    tdata.error = 0;
    tdata.level = 0;
    tdata.tree  = tree;
    tdata.nodeptr = tdata.node;
    tdata.pathptr = tdata.path;
	
    node = __TreeSearch(tdata.tree->root,&tdata);
	
    /* find nearest left step */
    if ( node == NULL )
    {
        while ( tdata.level > 0 )
        {
            tdata.pathptr--;
            tdata.nodeptr--;
			
            if ( *tdata.pathptr == AVL_RIGHT)
            {
                /* last right found, so it is result */
                node = *tdata.nodeptr;
                break;
            }
            tdata.level--;
        }
    }
    return node;
}
