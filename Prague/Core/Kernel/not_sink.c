/*-----------------31.01.03 00:00 -----------------
* Project Prague                                 *
* Author Petrovitch                              *
* Copyright (c) Kaspersky Lab                    *
* Purpose Prague internal API                    *
*                                                *
--------------------------------------------------*/


#include "kernel.h"


#define NSC_SIZE(s) ( sizeof(tNotificationSinkArr) + (((s)-1)*sizeof(tNotificationSink)*NOTIFICATION_SINC_CLUSTER) )

tNotificationSinkArr* g_sinks[USER_SINKS];


// ---
tUINT _unique_id() {
  tUINT i, j, k, c;
  tBOOL nf;
  tNotificationSinkArr* arr;
  tNotificationSink*    s;
  for( i=1; i<cMAX_UINT; i++ ) {
    for( j=0,nf=cTRUE; nf && (j<countof(g_sinks)); j++ ) {
      if ( 0 != (arr=g_sinks[j]) ) {
        for( k=0,c=arr->count,s=&arr->arr[0]; nf && (k<c); k++,s++ )
          nf = (i != s->id);
      }
    }
    if ( nf )
      break;
  }
  return i;
}




// ---
tERROR pr_call _ns_add( tPO* po, tSNS_ACTION action, tDWORD* id, tFUNC_PTR func, tPTR ctx ) {
  tERROR                err;
  tNotificationSinkArr* arr;
  tBOOL                 changed;
  
  if ( action >= countof(g_sinks) )
    return errBAD_INDEX;

  wlock(po);
  arr = g_sinks[action];
  if ( !arr ) {
    changed = cTRUE; 
    err = PrAlloc( (tPTR*)&arr, NSC_SIZE(1) );
  }
  else if ( 0 == (arr->count % NOTIFICATION_SINC_CLUSTER) ) {
    changed = cTRUE;
    err = PrRealloc( (tPTR*)&arr, arr, NSC_SIZE((arr->count / NOTIFICATION_SINC_CLUSTER)+1) );
  }
  else {
    changed = cFALSE;
    err = errOK;
  }
  
  if ( PR_SUCC(err) ) {
    tUINT              i;
    tNotificationSink* s = (&arr->arr[0]) + arr->count;

    if ( changed )
      g_sinks[action] = arr;
    i = _unique_id();
    ++arr->count;
    s->id   = i;
    s->func = func;
    s->ctx  = ctx;

    if ( id )
      *id = s->id;
  }
  wunlock(po);
  return err;
}



// ---
tERROR pr_call _ns_del( tPO* po, tDWORD id ) {
  tUINT  i;
  tERROR e;

  e = errNOT_FOUND;
  rlock(po);
  for( i=0; i<countof(g_sinks); i++ ) {
    
    tUINT                 c, k;
    tBOOL                 nf, locked;
    tNotificationSink*    s;
    tNotificationSinkArr* a = g_sinks[i];

    if ( !a )
      continue;

    c  = a->count;
    s  = &a->arr[0];
    nf = cTRUE;
    locked = cFALSE;
    
    for( k=0; k<c; k++,s++ ) {
      tNotificationSink* n;
      if ( nf && (id != s->id) ) 
        continue;
      nf = cFALSE;
      if ( !locked ) {
        wlock(po);
        locked = cTRUE;
      }
      if ( k == (c-1) )
        break;
      n = s + 1;
      s->id   = n->id;
      s->func = n->func;
      s->ctx  = n->ctx;
    }

    if ( !nf ) {
      wunlock(po);
      a->count--;
      e = errOK;
      break;
    }

  }
  runlock(po);
  return e;
}



// ---
tERROR pr_call _ns_get_arr( tSNS_ACTION action, tNotificationSink** sinks, tUINT* count ) {
  
  if ( action >= countof(g_sinks) )
    return errBAD_INDEX;

  if ( !g_sinks[action] || !g_sinks[action]->count ) {
    *sinks = 0;
    *count = 0;
    return 0;
  }

  *sinks = &g_sinks[action]->arr[0];
  *count = g_sinks[action]->count;
  return errOK;
}


