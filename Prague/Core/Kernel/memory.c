/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>

#include "Prague/iface/i_heap.h"


#define MEM_OBJECT (g_kernel_heap ? ((hOBJECT)g_kernel_heap) : ((hOBJECT)g_root) )

#define STATIC_MEMORY_SIZE		0x800

hHEAP g_heap;
hHEAP g_kernel_heap;

static tBYTE _memory[STATIC_MEMORY_SIZE+sizeof(tINT)];
static tUINT _amount = STATIC_MEMORY_SIZE;
static tBYTE null_ptr;
//#define MEM_CHECK

#ifdef MEM_CHECK
  #include <Prague/iface/i_csect.h>
	tINT  mem_heap_count = 0;
	tINT  mem_self_count = 0;
	hCRITICAL_SECTION g_cs_mem = 0;
#endif


// ---
static tPTR pr_call find_prev( tPTR to_find ) {
	tBYTE* prev = 0;
	tBYTE* ptr = _memory;

	while ( ptr < (_memory+sizeof(_memory)) ) {
		if ( ptr == to_find )
			return prev;
		prev = ptr;
		if ( *(tINT*)ptr < 0 )
			ptr += sizeof(tINT) - *(tINT*)ptr;
		else
			ptr += sizeof(tINT) + *(tINT*)ptr;
	}
	return 0;
}




// ---
tVOID pr_call _MemInitialize() {
	*(tINT*)_memory = sizeof(_memory)-sizeof(tINT);
  mset( _memory+sizeof(tINT), 0xfe, sizeof(_memory)-sizeof(tINT) );
	#ifdef MEM_CHECK
		_SyncCreate( 0, g_hRoot, (hOBJECT*)&g_cs_mem, IID_CRITICAL_SECTION, PID_ANY, 0 );
	#endif
}




// ---
tERROR pr_call _MemDeinitialize( tPO* po ) {
	tHANDLE* handle;
	hOBJECT kernel_heap_obj = (hOBJECT)g_kernel_heap;
	hOBJECT global_heap_obj = (hOBJECT)g_heap;

	g_heap = 0;
	g_kernel_heap = 0;

	wlock(po);
	if ( kernel_heap_obj )
		_AddToChildList( MakeHandle(kernel_heap_obj), g_hRoot );
	if ( global_heap_obj )
		_AddToChildList( MakeHandle(global_heap_obj), g_hRoot );
	wunlock(po);

	if ( global_heap_obj ) {
		handle = MakeHandle( global_heap_obj );
		if ( PR_FAIL(_WrapperWithdraw(0,handle,0,0)) ) // remove all wrappers
			_set_vtbl( handle, _ObjFreedFunctions );
		if ( handle && handle->ext )
			handle->ext = 0;
		CALL_SYS_ObjectClose( global_heap_obj );
	}

	if ( kernel_heap_obj ) {
		handle = MakeHandle( kernel_heap_obj );
		if ( PR_FAIL(_WrapperWithdraw(0,handle,0,0)) ) // remove all wrappers
			_set_vtbl( handle, _ObjFreedFunctions );
		if ( handle && handle->ext )
			handle->ext = 0;
		CALL_SYS_ObjectClose( kernel_heap_obj );
	}
	
	#ifdef MEM_CHECK
		if ( g_cs_mem )
			_ObjectClose( 0, MakeHandle(g_cs_mem), 0 );
	#endif

	return errOK;
}



// ---
tERROR pr_call _MemInitObject( tPO* po ) {
	tERROR err = errOK;

	if ( !g_heap && PR_SUCC(err=CALL_SYS_ObjectCreateQuick(g_root,&g_heap,IID_HEAP,PID_ANY,SUBTYPE_ANY)) ) {
    wlock(po);
    g_hRoot->mem = g_heap;
		_Remove( MakeHandle(g_heap) ); // cut it from the hierarchy
    wunlock(po);
	}
	
	if ( PR_SUCC(err) && !g_kernel_heap && PR_SUCC(err=CALL_SYS_ObjectCreateQuick(g_root,&g_kernel_heap,IID_HEAP,PID_ANY,SUBTYPE_ANY)) ) {
    wlock(po);
		_Remove( MakeHandle(g_kernel_heap) ); // cut it from the hierarchy
    wunlock(po);
	}
	
	return err;
}




// ---
tERROR pr_call PrAlloc( tPTR* ptr, tUINT size ) {
	tINT   diff;
	tINT   tmp_size;
	tINT   excess;
	tBYTE* found;
	tBYTE* tmp;

	if ( !size ) {
		PR_TRACE(( MEM_OBJECT, prtNOTIFY, "krn\tMemAlloc: size is ZERO !!!" ));
		size = 1;
	}
	
	if ( g_kernel_heap ) {
		tERROR err = g_kernel_heap->vtbl->Alloc( g_kernel_heap, ptr, size );
		#ifdef MEM_CHECK
			if ( PR_SUCC(err) ) {
				lock_ex( g_cs_mem, SHARE_LEVEL_WRITE );
				mem_heap_count++;
				unlock_ex( g_cs_mem, 0 );
			}
		#endif
		return err;
	}

	else if ( !size ) {
		*ptr = &null_ptr;
		return errOK;
	}

	else {
		size = ( (size / sizeof(tINT)) + (!!(size%sizeof(tINT))) ) * sizeof(tINT);
		if ( size > _amount ) {
			*ptr = 0;
			return errNOT_ENOUGH_MEMORY;
		}
		else {
			excess = sizeof(_memory);
			found = 0;
			tmp = _memory;
			while( tmp < (_memory+sizeof(_memory)) ) {
				tmp_size = *(tINT*)tmp;
				diff = tmp_size - size;
				if ( !diff ) {
					found = tmp;
					break;
				}
				else if ( (diff > 0) && (diff < excess) ) {
					excess = diff;
					found = tmp;
				}
				if ( tmp_size < 0 )
					tmp_size = -tmp_size;
				tmp += sizeof(tINT) + tmp_size;
			}
			
			if ( found ) {
				diff = *(tINT*)found - size;
				*(tINT*)found = -(tINT)size;
				found += sizeof(tINT);
				if ( diff )
					*((tINT*)(found+size)) = diff - sizeof(tINT);
        mset( found, 0, size );
        *ptr = found;
				_amount -= size + sizeof(tINT);
				#ifdef MEM_CHECK
					lock_ex( g_cs_mem, SHARE_LEVEL_WRITE );
					mem_self_count++;
					unlock_ex( g_cs_mem, 0 );
				#endif
				return errOK;
			}
			else {
				*ptr = 0;
				return errNOT_ENOUGH_MEMORY;
			}
		}
	}
}




// ---
tERROR pr_call PrRealloc( tPTR* result, tPTR ptr, tUINT size ) {      // -- Change size of previously allocated block;
		
	if ( !ptr )
		return PrAlloc( result, size );
	
	if ( !size ) {
		PR_TRACE(( MEM_OBJECT, prtNOTIFY, "krn\tMemRealloc: size is ZERO !!!" ));
		size = 1;
	}
	
	if ( (ptr >= (tPTR)_memory) && (ptr < (tPTR)(_memory+sizeof(_memory))) ) {
		tERROR err;
		tPTR   res;
		tINT   add;
		tINT   diff;
		tINT   prev_size;
		tINT   free_block_count;
		tBYTE* found = 0;
		tBYTE* tmp = _memory;

		while( tmp < (_memory+sizeof(_memory)) ) {
			if ( ptr == (tBYTE*)(tmp+sizeof(tINT)) ) {
				found = tmp;
				break;
			}
			else {
				add = *((tINT*)tmp);
				if ( add < 0 )
					add = -add;
				tmp += sizeof(tINT) + add;
			}
		}

		if ( !found )
			return errNOT_FOUND;

		if ( *((tINT*)found) > 0 )
			return errOBJECT_ALREADY_FREED;

		prev_size = -*(tINT*)found;
		if ( g_kernel_heap ) {
			res = 0;
			if ( PR_FAIL(err=g_kernel_heap->vtbl->Alloc(g_kernel_heap,&res,size)) )
				return err;
			#ifdef MEM_CHECK
				else {
					lock_ex( g_cs_mem, SHARE_LEVEL_WRITE );
					mem_heap_count++;
					unlock_ex( g_cs_mem, 0 );
				}
			#endif
			if ( (tINT)size > prev_size )
				size = prev_size;
			mcpy( res, found+sizeof(tINT), size );
			*result = res;
			PrFree( ptr );
			return errOK;
		}
		else { // have to do it by myself
			size = ( (size / sizeof(tINT)) + (!!(size%sizeof(tINT))) ) * sizeof(tINT);
			diff = size - prev_size;
			if ( !diff ) { // do not need to do anything else
				*result = ptr;
				return errOK;
			}
			else if ( diff < 0 ) { // shrink the memory piece
				add = 0;
				free_block_count = 0;
				tmp = found + sizeof(tINT) + prev_size;
				while ( (tmp < _memory+sizeof(_memory)) && (*((tINT*)tmp) >= 0) ) { // check if i can expand it inplace (check tailed free blocks)
					free_block_count++;
					add += sizeof(tINT) + *((tINT*)tmp);
					tmp += sizeof(tINT) + *((tINT*)tmp);
				}
				*((tINT*)found) = -(tINT)size;  // mark it occupied
				add -= diff - sizeof(tINT);     // diff is negative
				found += sizeof(tINT) + size;
				*((tINT*)found) = add;          // mark it free
				mset( found+sizeof(tINT), 0xfd, add );
				_amount += -diff + (free_block_count - 1) * sizeof(tINT);
				*result = ptr;
				return errOK;
			}
			else if ( diff > (tINT)_amount ) { // expand the memory piece, but i have no enough memory
				*result = ptr;
				return errNOT_ENOUGH_MEMORY;
			}
			else { // expand the memory piece
				add = 0;
				free_block_count = 0;
				tmp = found + sizeof(tINT) + prev_size;
				while ( (add<diff) && (tmp < (_memory+sizeof(_memory))) && (*((tINT*)tmp) >= 0) ) { // check if i can expand it inplace
					free_block_count++;
					add += sizeof(tINT) + *((tINT*)tmp);
					tmp += sizeof(tINT) + *((tINT*)tmp);
				}
				
				add -= diff;
				if ( !add ) { // can do it inplace (expandable memory piece match to request)
					*((tINT*)found) = size;
					mset( found+sizeof(tINT)+prev_size, 0, diff );
					_amount -= add - free_block_count * sizeof(tINT);
					*result = ptr;
					return errOK;
				}

				else if ( add > 0 ) { // can do it inplace too (expandable memory piece bigger than request)
					*((tINT*)found) = -(tINT)size; // mark it occupied
					mset( found+sizeof(tINT)+prev_size, 0, diff ); // zero tail including next free piece
					*((tINT*)(found+sizeof(tINT)+size)) = add; // next piece is free
					_amount -= diff - (free_block_count-1) * sizeof(tINT);
					*result = ptr;
					return errOK;
				}

				else if ( PR_FAIL(err=PrAlloc(&res,size)) ) // cannot do it inplace
					return err;

				else {
					mcpy( res, ptr, prev_size );
					PrFree( ptr );
					*result = res;
					return errOK;
				}
			}
		}
	}

	else if ( g_kernel_heap )
		return g_kernel_heap->vtbl->Realloc( g_kernel_heap, result, ptr, size );

	else
		return errUNEXPECTED;
}




// ---
tERROR pr_call PrFree( tPTR ptr ) {
	
	if ( !ptr ) {
		PR_TRACE(( MEM_OBJECT, prtNOTIFY, "krn\tMemFree: pointer is NULL !!!" ));
		return errOK;
	}
	
	if ( (ptr >= (tPTR)_memory) && (ptr < (tPTR)(_memory+sizeof(_memory))) ) {
		tBYTE* tmp = _memory;
		while( tmp < (_memory+sizeof(_memory)) ) {
			tINT size = *(tINT*)tmp;
			if ( ptr == (tBYTE*)(tmp+sizeof(tINT)) ) {
				tBYTE* tmp2;
				
				if ( size < 0 )
					size = -size;
				else
					return errOBJECT_ALREADY_FREED;

				*((tINT*)tmp) = size; // mark it free
				_amount += size;
				tmp2 = find_prev( tmp );
				if ( tmp2 && (0<=(*(tINT*)tmp2)) )
					tmp = tmp2;
				else
					tmp2 = tmp;
				size = 0;
				// compact internal heap
				while ( (tmp2 < _memory+sizeof(_memory)) && (*((tINT*)tmp2) >= 0) ) {
					_amount += sizeof(tINT);
					size += sizeof(tINT) + *((tINT*)tmp2);
					tmp2 += sizeof(tINT) + *((tINT*)tmp2);
				}
				size -= sizeof(tINT);
				_amount -= sizeof(tINT);
				*((tINT*)tmp) = size;
				mset( tmp+sizeof(tINT), 0xfd, size );
				#ifdef MEM_CHECK
					lock_ex( g_cs_mem, SHARE_LEVEL_WRITE );
					mem_self_count--;
					unlock_ex( g_cs_mem, 0 );
				#endif
				return errOK;
			}
			else {
				if ( size < 0 )
					size = -size;
				tmp = tmp + sizeof(tINT) + size;
			}
		}
		return errUNEXPECTED;
	}

	else if ( g_kernel_heap ) {
		#ifdef MEM_CHECK
			lock_ex( g_cs_mem, SHARE_LEVEL_WRITE );
			mem_heap_count--;
			unlock_ex( g_cs_mem, 0 );
		#endif
		return g_kernel_heap->vtbl->Free( g_kernel_heap, ptr );
	}
	else
		return errOK;
}



#if defined(_DEBUG)

//_declspec(dllexport) int __cdecl _CrtDbgReport( int nRptType, const char * szFile, int nLine, const char * szModule, const char * szFormat, ... ) {
//  return 1;
//}


#endif
