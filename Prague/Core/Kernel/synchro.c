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
#include <Prague/iface/i_root.h>


tPO*    g_po;
tPO     g_po_array[0x10];


tIID    g_sync_iids[]  = { IID_CRITICAL_SECTION, IID_MUTEX, IID_SEMAPHORE };
tDWORD  g_sync_flags[] = { fPRAGUE_SYNC_CS_AVAIL, fPRAGUE_SYNC_MUTEX_AVAIL, fPRAGUE_SYNC_SEMAPHORE_AVAIL };
tDWORD  g_sync_iids_count = countof(g_sync_iids);



// ---
tERROR pr_call _SyncCreateAvailable( hOBJECT dad, hCRITICAL_SECTION* cs ) {
	tDWORD   i;
	tERROR   sync_err;
	tERROR   err  = errINTERFACE_NOT_FOUND;
  tPO*     po;
  tHANDLE* dadh = MakeHandle( dad );

  enter_lf();

	if ( _HC(dadh,po,dad,&sync_err) ) {
		for ( *cs=0,i=0; (err==errINTERFACE_NOT_FOUND) && i<countof(g_sync_iids); i++ ) {
			tHANDLE* csh = 0;
			if ( !g_sync_iids[i] )
				continue;
			err = _ObjectIIDCreate( po, dadh, &csh, g_sync_iids[i], PID_ANY, SUBTYPE_ANY );
			if ( PR_FAIL(err) )
				continue;
			err = _ObjectCreateDone( po, csh );
			if ( PR_FAIL(err) )
				_ObjectClose( po, csh, 0 );
			else {
				*cs = (hCRITICAL_SECTION)MakeObject( csh );
				break;
			}
		}
		runlock(po);
	}
	else
		err = sync_err;

  leave_lf();
	return err;
}



// ---
tPO* pr_call get_free_po() {
  tUINT i;
  for( i=0; i<countof(g_po_array); i++ ) {
    if ( !g_po_array[i].cs )
      return &g_po_array[i];
  }
  return 0;
}



// ---
tERROR pr_call _SyncInitObj( tPO* po ) {
	tERROR       err = errOK;
	tDWORD       i;
	const tDATA* sync_prop;

	if ( g_po )
		return errOK;
	
	sync_prop = _PropTableSearch( g_hRoot->iface, pgSYNCHRONIZATION_AVAILABLE, 0 );
	if ( sync_prop )
		((tDATA*)sync_prop)[1] = 0;

	for ( i=0; i<countof(g_sync_iids); i++ ) {
    
    if ( !g_po ) {
      tPO* prot_obj;
      hCRITICAL_SECTION cs;
      err = CALL_SYS_ObjectCreateQuick( MakeObject(g_hRoot), &cs, g_sync_iids[i], PID_ANY, SUBTYPE_ANY );

      if ( PR_FAIL(err) )
        ;
      else if ( !(prot_obj=get_free_po()) ) {
        CALL_SYS_ObjectClose( cs );
        err = errNOT_ENOUGH_MEMORY;
      }
      else {
        wlock(po);
        _Remove( MakeHandle(cs) ); // cut it from the hierarchy
        prot_obj->cs = cs;
        prot_obj->two_level = CALL_SYS_PropertyGetBool( cs, pgSHARE_LEVEL_SUPPORT );
        prot_obj->usage = 0;
        prot_obj->condemned = cFALSE;
        g_po = prot_obj;
        wunlock(po);
      }
    }

		if ( sync_prop && _InterfaceFind(0,g_sync_iids[i],PID_ANY,SUBTYPE_ANY,VID_ANY,cFALSE) )
			((tDATA*)sync_prop)[1] |= g_sync_flags[i];
	}
	return err;
}



// ---
tERROR pr_call _SyncDeinitObj() {
  tUINT i;
  
  g_po = 0;

  mset( g_sync_iids, 0, sizeof(g_sync_iids) );

  for( i=0; i<countof(g_po_array); i++ ) {
    hCRITICAL_SECTION cs = g_po_array[i].cs;
	  if ( cs ) {
			tHANDLE* h;
			tERROR error;
      g_po_array[i].cs = 0;
      if ( _HC(h,0,(hOBJECT)cs,&error) && PR_SUCC(error) ) {
        _AddToChildList( MakeHandle(cs), g_hRoot );
			  CALL_SYS_ObjectClose( cs );
		  }
	  }
  }
	return errOK;
}



// ---
tERROR pr_call _SyncChangeObj( tPO* po, tIID iid, tPID pid, tDWORD subtype ) {
  tERROR   err = errOK;
  tHANDLE* hcs;
  tPO*     prot;
  
  prot = get_free_po();

  if ( !prot )
    err = errNOT_ENOUGH_MEMORY;

  else if ( PR_FAIL(err=_ObjectIIDCreate(po,g_hRoot,&hcs,iid,pid,subtype)) ) 
    ;

  else if ( PR_FAIL(err=_ObjectCreateDone(po,hcs)) )
    _ObjectClose( po, hcs, 0 );

  else {
    tPO* opo;

    wlock(po);
    _Remove( hcs ); // cut it from the hierarchy
    prot->condemned = cFALSE;
    prot->usage = 0;
    prot->cs = (hCRITICAL_SECTION)MakeObject( hcs );

    opo = g_po;
    g_po = prot;
    if ( opo )
      opo->condemned = cTRUE;
    wunlock(po);

    if ( !opo->usage )
      _SyncCloseObj( opo );
  }
  return err;
}



// ---
tERROR pr_call _SyncCloseObj( tPO* prot ) {
  tPO* po;
  
  enter_lf();
  wlock(po);
  _AddToChildList( MakeHandle(prot->cs), g_hRoot );
  wunlock(po);

  _ObjectClose( po, MakeHandle(prot->cs), 0 );
  prot->cs = 0;
  prot->usage = 0;
  prot->condemned = cFALSE;
  leave_lf();
  return errOK;
}


