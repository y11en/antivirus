#if defined (_WIN32)
#include "wheap.h"
#include <Prague/iface/e_ktrace.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

#define FILE_SECTION "\n------------------------------\n   "



#ifdef MEM_CHECK

#include "../codegen/prdbginfo/prdbginfo.h"
#include "WHeap_check.h"


typedef struct tag_tBDsc {
	tUINT m_block_size;
	tUINT m_counter;
} tBDsc;


// ---
int qsort_cmp( const void* f, const void* s ) {
	tUINT fr = ((const tBDsc*)f)->m_block_size * ((const tBDsc*)f)->m_counter;
	tUINT sr = ((const tBDsc*)s)->m_block_size * ((const tBDsc*)s)->m_counter;
	if ( fr < sr )
		return 1;
	if ( fr > sr )
		return -1;
	return 0;
}


// -------------------------------------------------------------------------------------------------
// ---
#if defined(MEM_CHECK_ADV)

#define _post_check_sig '+--+'

	tVOID _heap_check_init( hi_Heap heap ) {
		HeapData* d = heap->data;
		tHeapCheckData* c = &d->m_check_data;
		char l_name[50];

		PrMakeUniqueString( l_name, sizeof(l_name), "pg_mem_counter" );
		c->m_sync = _create_mutex( 0, TRUE, l_name );
		c->m_head.m_next = &c->m_tail;
		c->m_tail.m_prev = &c->m_head;
		c->m_head.m_sig = c->m_tail.m_sig = signature(d);
		c->m_head.m_num = 1;
		c->m_head.m_tag = 0;
		c->m_tail.m_num = (tDWORD)-1;
		c->m_head.m_caller_address = 0;
		c->m_tail.m_caller_address = 0;
		ReleaseMutex( c->m_sync );
	}
#endif



// ---
#if defined(MEM_CHECK_ADV)

tVOID _heap_check_deinit( hi_Heap heap ) {
	HeapData* d = heap->data;
	tHeapCheckData* c = &d->m_check_data;
	if ( c->m_sync && (c->m_sync != INVALID_HANDLE_VALUE) ) {
		CloseHandle( c->m_sync );
		c->m_sync = 0;
	}
}


// ---
tUINT _heap_calc_tagged_size( hi_Heap heap ) {
	tUINT tag;
	tUINT size = 0;
	tHeapCheckData* c = &heap->data->m_check_data;

	if ( c->m_sync )
		WaitForSingleObject( c->m_sync, INFINITE );

	tag = c->m_head.m_tag;
	if ( !tag )
		size = heap->data->m_requested_size;
	else {
		tHeader* h = c->m_head.m_next;
		size = 0;
		while( h ) {
			if ( h->m_tag == tag )
				size += h->m_size;
			h = h->m_next;
		}
	}
	if ( c->m_sync )
		ReleaseMutex( c->m_sync );
	return size;
}
#endif // MEM_CHECK_ADV



// ---
tVOID _heap_check_pre_alloc( hi_Heap heap, tDWORD* size ) {
	#if defined(MEM_CHECK_ADV)
		WaitForSingleObject( heap->data->m_check_data.m_sync, INFINITE );
		*size += sizeof(tDWORD);
	#endif

	*size += sizeof(tHeader);
}



#ifdef MEM_CHECK
// ---
tVOID _heap_check_post_alloc( hi_Heap heap, tPTR* ptr, tDWORD* psize ) {
	tHeapCheckData* c = &heap->data->m_check_data;
	tHeader*        h = ((tHeader*)*ptr);

	if ( h ) { // alloc succeeded
		tDWORD oldsize;
		*ptr = h + 1;
		*psize -= sizeof(tHeader);
		#ifdef MEM_CHECK_ADV
			*psize -= sizeof(tDWORD);
		#endif
		oldsize = h->m_size;
		h->m_size = *psize;
		#ifdef MEM_CHECK_ADV
			if ( h->m_sig != signature(heap->data) ) { // it's a new block -> have to insert it to the list 
				tHeader* t = &c->m_tail;
				h->m_dummy1 = 1;
				h->m_dummy2 = 2;
				h->m_sig = signature(heap->data);
				h->m_num = c->m_head.m_num++;
				h->m_tag = c->m_head.m_tag;
				h->m_prev = t->m_prev;
				h->m_next = t;
				t->m_prev->m_next = h;
				t->m_prev = h;
			}
			else { // it was realloc!! just to reinit neighbours
				*(tDWORD*)((tBYTE*)*ptr + oldsize) = 0;
				h->m_prev->m_next = h;
				h->m_next->m_prev = h;
			}
			#ifdef MEM_CHECK_ADV
				*(tDWORD*)((tBYTE*)*ptr + *psize) = _post_check_sig;
			#endif
			#if 0				
				for (i=0; i<10; i++) {
					tCHAR szFuncName[0x100];
					if (PR_FAIL(error = PrDbgGetInfoEx( 0, MEM_CHECK_SKIP_MODULES, i, DBGINFO_CALLER_ADDR, &h->m_caller_address, sizeof(h->m_caller_address), 0 )))
						break;
					if (PR_FAIL(error = PrDbgGetInfoEx(h->m_caller_address, NULL, 0, DBGINFO_SYMBOL_NAME, szFuncName, sizeof(szFuncName), 0 )))
						break;
					strlwr(szFuncName);
					if (strstr(szFuncName, "alloc")==0)
						break;
				}
			#elif !defined(_M_X64)
				PrDbgGetInfoEx( 0, MEM_CHECK_SKIP_MODULES, 1, DBGINFO_CALLER_ADDR, &h->m_caller_address, sizeof(h->m_caller_address), 0 );
			#endif
		#endif
	}
	#ifdef MEM_CHECK_ADV
		ReleaseMutex( c->m_sync );
	#endif
}
#endif

// ---
tERROR _heap_check_obj( hi_Heap heap, tPTR* ptr ) {
	#if defined( MEM_CHECK_ADV )
		tHeader* h;
	#endif

	if ( !*ptr )
		return errOBJECT_INVALID;

	(*ptr) = ((tHeader*)(*ptr)) - 1;
		
	#if defined( MEM_CHECK_ADV )
		h = *ptr;
		{
			tDWORD* ptrnew = (tDWORD*)((tBYTE*)*ptr + sizeof(tHeader) + h->m_size);
			if (*ptrnew != _post_check_sig)
			{
				PR_TRACE(( heap, prtERROR, "ldr\tPOST BUFFER OVERRUN(%p)!", *ptr));
			}
		}
		if ( h->m_sig != signature(heap->data) ) {
			if ( h->m_sig == DELETED )
				PR_TRACE(( heap, prtERROR, "ldr\tBAD memory pointer(%p)! Pointer already deleted", *ptr ));
			else
				PR_TRACE(( heap, prtERROR, "ldr\tBAD memory pointer(%p)! Signature(0x%08x) check failed", *ptr, h->m_sig ));
			return errOBJECT_INVALID;
		}
	#endif
	return errOK;
}

tVOID    _heap_locked_add( tDWORD* pvar, tDWORD size ) {
	enter;
	InterlockedExchangeAdd( (tLONG*)pvar, (tLONG)size );
	leave; 
}

tVOID    _heap_locked_sub( tDWORD* pvar, tDWORD size ) { 
	enter;
	InterlockedExchangeAdd( (tLONG*)pvar, -(tLONG)size );
	leave; 
}



// ---
tERROR _heap_check_pre_delete( hi_Heap heap, tPTR* ptr ) {

	tERROR          e;
	tHeader*        h;
	#if defined( MEM_CHECK_ADV )
		tHeapCheckData* c;
	#endif

	if ( PR_FAIL(e=_heap_check_obj(heap,ptr)) )
		return e;

	h = ((tHeader*)*ptr);
	memset( h+1, -2, h->m_size );

	#if defined( MEM_CHECK_ADV )
		c = &heap->data->m_check_data;
		WaitForSingleObject( c->m_sync, INFINITE );
	
		h->m_prev->m_next = h->m_next;
		h->m_next->m_prev = h->m_prev;
		h->m_sig = DELETED;
		h->m_num = 0;
		h->m_prev = h->m_next = 0;
	#endif
	return errOK;
}



// ---
tVOID _heap_check_post_delete( hi_Heap heap, tPTR ptr ) {
	#ifdef MEM_CHECK_ADV
		ReleaseMutex( heap->data->m_check_data.m_sync );
	#endif
}



// ---
tVOID _heap_check( tVOID* heap_to_check, tHeapCheckOutputFunc output, tVOID* params ) {

	hi_Heap heap = (hi_Heap)heap_to_check;
	HeapData* d;
	
	#if defined( MEM_CHECK_ADV )
		tINT i;
		tHeapCheckData* c;
		tHeader* h;
	#endif

	if ( !heap ) {
		heap = (hi_Heap)CALL_SYS_PropertyGetObj( g_root, pgOBJECT_HEAP );
		if ( !heap )
			return;
	}

	d = heap->data;

	if ( d->m_allocated_count ) {
		if ( output )
			output( params, heap, "%u memory leaks detected:", d->m_allocated_count );
		return;
	}

	#if defined( MEM_CHECK_ADV )
		c = &d->m_check_data;
		WaitForSingleObject( c->m_sync, INFINITE );

		h = c->m_head.m_next;
		for( i=0; (i<d->m_allocated_count) && (h != &c->m_tail); i++,h=h->m_next ) {
			if ( heap == *(hi_Heap*)(h+1) ) // prkernel enlarges size on sizeof(hi_Heap)
				h->m_size -= sizeof(hi_Heap); // but user expect to see ordered size
		}
		
		h = c->m_head.m_next;
		for( i=0; (i<d->m_allocated_count) && (h != &c->m_tail); i++,h=h->m_next ) {
			tUINT j, len;
			tCHAR szSrcFile[MAX_PATH];
			tDWORD dwSrcLine;
			tCHAR szModuleName[32];
			tCHAR szFunc[0x100];
			tCHAR szMemBlocks[0x200];
			tBDsc a_blocks[100] = {0};
			tUINT blocks;
			tUINT total_size;
			tUINT total_blocks;
			tHeader* h2;
			tBOOL finished = cFALSE;
			
			if ( h->m_caller_address == -1 )
				continue;
			
			szMemBlocks[0] = 0;
			#if !defined(_M_X64)
				if ( h->m_caller_address && PR_SUCC(PrDbgGetInfoEx(h->m_caller_address,NULL,0,DBGINFO_MODULE_NAME,szModuleName,countof(szModuleName),NULL)) ) {
					PrDbgGetInfoEx(h->m_caller_address, NULL, 0, DBGINFO_SRC_FILE, szSrcFile, countof(szSrcFile), NULL);
					PrDbgGetInfoEx(h->m_caller_address, NULL, 0, DBGINFO_SRC_LINE, &dwSrcLine, sizeof(tDWORD), NULL);
					PrDbgGetInfoEx(h->m_caller_address, NULL, 0, DBGINFO_SYMBOL_NAME, szFunc, countof(szFunc), NULL);
				}
				else {
			#endif
				strcpy( szModuleName, "unknown module" );
				strcpy( szFunc, "?" );
				*szSrcFile = 0;
				dwSrcLine = 0;
			#if !defined(_M_X64)
				}
			#endif			
			total_size = h->m_size;
			total_blocks = 1;
			blocks = 1;
			a_blocks[0].m_block_size = h->m_size;
			a_blocks[0].m_counter = 1;
			
			for(h2 = h->m_next; h2 && (h2 != &c->m_tail); h2=h2->m_next ) {
				if ( (h2->m_caller_address == -1) || (h2->m_caller_address != h->m_caller_address) )
					continue;
				h2->m_caller_address = -1;
				total_blocks++;
				total_size += h2->m_size;
				for( j=0; j<countof(a_blocks) && a_blocks[j].m_block_size; ++j ) {
					if ( a_blocks[j].m_block_size == h2->m_size ) {
						a_blocks[j].m_counter++;
						break;
					}
				}
				if ( j<countof(a_blocks) && !a_blocks[j].m_block_size ) {
					a_blocks[j].m_block_size = h2->m_size;
					a_blocks[j].m_counter = 1;
					blocks++;
				}
			}
			
			if ( blocks > 1 )
				qsort( a_blocks, blocks, sizeof(tBDsc), qsort_cmp );
			
			len = pr_sprintf( szMemBlocks, sizeof(szMemBlocks)-10, "%u bytes in %u block(s) = <", total_size, total_blocks );
			for( j=0; j<blocks; ++j ) {
				if ( (len+25 > sizeof(szMemBlocks)) ) {
					if ( !finished ) {
						memcpy( szMemBlocks+len, "...", 3 );
						len += 3;
					}
					finished = cTRUE;
				}
				else
					len += pr_sprintf( szMemBlocks+len, sizeof(szMemBlocks)-len-10, "%u*%u%s", a_blocks[j].m_counter, a_blocks[j].m_block_size, (((j+1)<blocks) ? "+" : "") );
			}
			szMemBlocks[len++] = '>';
			szMemBlocks[len++] = 0;
			if ( output ) {
				if ( *szSrcFile )
					output( params, heap, "%s(%d): %s : %s : %s", szSrcFile, dwSrcLine, szModuleName, szFunc, szMemBlocks);
				else
					output( params, heap, "%s : caller address(0X%08X) : %s", szModuleName, h->m_caller_address, szMemBlocks );
			}
		}
		ReleaseMutex( c->m_sync );
	#endif
}


#else

	tVOID _heap_check( tVOID* heap_to_check, tHeapCheckOutputFunc output, tVOID* params ) {
    //    DWORD dwSize = 0;
    //    if ( 0x80000000l & GetVersion() ) // win9x
    //      dwSize = _this->data->m_requested_size;
    //    else {
    //      PROCESS_HEAP_ENTRY Entry;
    //      HANDLE heap = _this->data->m_hHeap;
    //      Entry.lpData = NULL;
    //      while ( HeapWalk(heap,&Entry) ) {
    //        if ( Entry.wFlags & PROCESS_HEAP_ENTRY_BUSY )
    //          dwSize += Entry.cbData + Entry.cbOverhead;
    //      }
    //    }
    //		*(tDWORD*)buffer = dwSize;
	}

#endif // 






	// ---
tVOID _heap_check_trace_output_func( tVOID* params, tVOID* heap, const tCHAR* format, ... ) {
	#if defined( PR_TRACE )
		hTRACER t;
		tHeapCheckTraceOutParams* par = (tHeapCheckTraceOutParams*)params;
		if ( (OBJ_TR_CHK(par->heap,par->prtLevel) && (0!=(t=OBJ_TRACER(par->heap)))) ) {
			va_list a;
			va_start( a, format );
			CALL_Tracer_TraceV( t, (hOBJECT)par->heap, par->prtLevel, (tCHAR*)format, VA_LIST_ADDR(a) );
			va_end( a );
		}
	#endif
}





// ---
tVOID _heap_check_file_output_func( tVOID* params, tVOID* heap, const tCHAR* format, ... ) {
	
	tUINT len;
	tCHAR buff[0x400];
	HANDLE file;
	va_list arglist;
	tHeapCheckFileOutParams* par = (tHeapCheckFileOutParams*)params;
	
	if ( !par || !par->m_file_handle ) {
		const tCHAR* folder;
		if ( par && par->m_output_folder )
			folder = par->m_output_folder;
		else
			folder = "";
		
		pr_sprintf( buff, sizeof(buff), "%s\\%08x_%06u_heap_leaks.log", folder, heap, GetCurrentProcessId() );
		file = CreateFile( buff, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( INVALID_HANDLE_VALUE == file )
			return;
		
		len = GetFileSize( file, 0 );
		if ( len )
			SetFilePointer( file, len, 0, FILE_BEGIN );
		WriteFile( file, FILE_SECTION, sizeof(FILE_SECTION), &len, 0 );
		if ( par )
			par->m_file_handle = file;
	}
	else
		file = par->m_file_handle;
	
	va_start( arglist, format );
	len = pr_vsprintf( buff, sizeof(buff), format, VA_LIST_ADDR(arglist) );
	va_end( arglist );
	
	if ( len >= (sizeof(buff)-1) )
		len = sizeof(buff)-1;
	
	if ( *(buff+len-1) != '\n' ) {
		*(tWORD*)(buff+len) = '\n';
		len += 1;
	}
	
	WriteFile( file, buff, len, &len, 0 );
	FlushFileBuffers( file );
}



// ---
tVOID _heap_check_trace_output( hi_Heap heap, tDWORD prtLevel ) {
	tHeapCheckTraceOutParams p = { heap, prtLevel };
	_heap_check( heap, _heap_check_trace_output_func, &p );
}



// ---
tVOID _heap_check_file_output( hi_Heap heap, const tCHAR* output_folder ) {
	tHeapCheckFileOutParams p = { output_folder, 0 };
	_heap_check( heap, _heap_check_file_output_func, &p );
	if ( p.m_file_handle )
	{
		CloseHandle( p.m_file_handle );
		p.m_file_handle = 0;
	}
}


#endif // _WIN32
