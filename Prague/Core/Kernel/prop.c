/*-----------------19.03.00 00:00 -----------------
 * Project Prague                                 *
 * Subproject Kernel iface registering        *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>



// ---
tBOOL pr_call _PropTableCheck( const tDATA* table, tDWORD propcount, tDATA* maxsize, tBOOL* code_page_implemented, tDATA** pid_user ) {

  tBOOL  ret;
  tDATA  size;
  tDWORD num = 0;

  *maxsize = 0;

  PR_TRACE_A0( 0, "Enter _PropTableCheck" );

  ret = cFALSE;

  if ( (table == 0) && (propcount == 0) ) {
    PR_TRACE(( 0, prtIMPORTANT, "krn\tEmpty property table" ));
    ret = cTRUE;
  }

  else if ( !table != !propcount ) 
    PR_TRACE(( 0, prtERROR, "krn\tBad property table registration parameters table=%p, prop count=%u", table, propcount  ));

  else {

    while ( num < propcount ) {

      if ( (PROP_IDENTIFIER(table) & pRANGE_MASK) == 0  ) {
        PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): bad property identifier: %tprop", num, PROP_IDENTIFIER(table) ));
        goto leave_func;
      }

      if ( !PROP_SIZE(table) ) {
        if ( !PROP_GETFN(table) && !PROP_SETFN(table) ) {
					PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): NO buffer and NO funcs assigned", num ));
					goto leave_func;
				}
			}

      else {
				tDWORD required;
				tTYPE_ID type = ( (PROP_IDENTIFIER(table)) & pTYPE_MASK ) >> pTYPE_SHIFT;

				if ( (type > tid_LAST_TYPE) || (0==(required=_type_size[type])) ) {
					PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): unknown type", num ));
					goto leave_func;
				}

				if ( (required != ((tDWORD)-1)) && (PROP_SIZE(table) != required) ) {
					PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): size of variable (%d) doesn't match size of property (%d): index:%u", num, PROP_SIZE(table), required ));
					goto leave_func;
				}
      }

      switch ( (PROP_MODE(table) & ~(cPROP_REQUIRED|cPROP_WRITABLE_ON_INIT|cPROP_BUFFER_HSTRING)) ) {

        case cPROP_BUFFER_NONE :
          if ( (PROP_BUFFER(table) != 0) || (PROP_SIZE(table) != 0) ) {
            PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): bad property flags (NONE+buffer): %tprop", num, PROP_IDENTIFIER(table) ));
            goto leave_func;
          }
          if ( (PROP_GETFN(table) == NULL) && (PROP_SETFN(table) == NULL) ) {
            PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): bad property flags (NONE without funcs): %tprop", num, PROP_IDENTIFIER(table) ));
            goto leave_func;
          }
          break;

        case cPROP_BUFFER_READ       :
        case cPROP_BUFFER_WRITE      :
        case cPROP_BUFFER_READ_WRITE :
          if ( PROP_SIZE(table) == 0) {
            PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): bad property size: %tprop", num, PROP_IDENTIFIER(table) ));
            goto leave_func;
          }
          size = PROP_OFFSET(table) + PROP_SIZE(table);
          if ( size > *maxsize )
            *maxsize = size;
          break;

        case cPROP_BUFFER_SHARED     :
          if ( PROP_SIZE(table) == 0) {
            PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): bad property size: %tprop", num, PROP_IDENTIFIER(table) ));
            goto leave_func;
          }
          break;

        case cPROP_BUFFER_SHARED_PTR :
        case cPROP_BUFFER_SHARED_VAR :
          if ( (PROP_BUFFER(table) == 0) || (PROP_SIZE(table) == 0)) {
            PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): you have to define buffer and size for shared property: %tprop", num, PROP_IDENTIFIER(table) ));
            goto leave_func;
          }
          break;

        default:
          PR_TRACE(( 0, prtERROR, "krn\tProperty index(%d): unknown property flags: %tprop", num, PROP_IDENTIFIER(table) ));
          goto leave_func;
          return cFALSE;
      } // switch ( PROP_MODE(table) )

			if ( PROP_IDENTIFIER(table) == pgOBJECT_CODEPAGE ) 
				*code_page_implemented = cTRUE;
      else if ( (PROP_IDENTIFIER(table) == pgPLUGIN_ID) && pid_user )
        *pid_user = (tDATA*)table;
      table += PROP_ARRAY_SIZE;
      num++;
    } // while ( propcount-- )

    if ( *maxsize > 0xFFFF ) {
      PR_TRACE(( 0, prtERROR, "krn\tCumulative property size must be less then 0xffff: %tprop", num, PROP_IDENTIFIER(table) ));
      goto leave_func;
    }

    ret = cTRUE;
  }

leave_func:
  PR_TRACE_A1( 0, "Leave _PropTableCheck ret tBOOL = %d", ret );
  return ret;
}



// ---
const tDATA* pr_call _PropTableSearch( const tINTERFACE* iface, tPROPID prop, tDWORD* init_prop_pos ) {

  const tDATA* table;
  tDWORD propcount;
  PR_TRACE_A0( 0, "Enter _PropTableSearch" );

  table = iface->proptable;
  propcount = iface->propcount;

  if ( init_prop_pos )
    *init_prop_pos = 0;

  if ( table != NULL ) {
    while ( propcount-- ) {

      if ( PROP_IDENTIFIER(table) == prop ) {
        if ( init_prop_pos && !(PROP_MODE(table) & cPROP_REQUIRED) )
          *init_prop_pos = 0xffffffff;
        PR_TRACE_A0( 0, "Leave _PropTableSearch ret tDATA* != NULL" );
        return table;
      }

      if ( init_prop_pos && (PROP_MODE(table) & cPROP_REQUIRED) )
        (*init_prop_pos)++;

      table += PROP_ARRAY_SIZE;
    }
  }

  PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tProperty(id=%tprop) not found",prop ));
  PR_TRACE_A0( 0, "Leave _PropTableSearch ret tDATA* = NULL" );
  return NULL;
}



// ---
tDWORD pr_call _PropInitDword( tDATA* table, tDWORD propcount ) {

  tDWORD num = 0;
  tDWORD init_flags = 0;

  PR_TRACE_A0( 0, "Enter _PropInitDword" );

  if ( table != NULL ) {
    while ( num < 32 && propcount-- ) {
      if ( PROP_MODE(table) & cPROP_REQUIRED )
        init_flags |= 1 << num++;
      table += PROP_ARRAY_SIZE;
    }
  }

  PR_TRACE_A1( 0, "Leave _PropInitDword ret tDWORD = 0x%x", init_flags );
  return init_flags;
}


// ---
tERROR pr_call _InterfaceGetProp( tPO* po, tINTERFACE* iface, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD count ) {

  tERROR        error = errOK;
  const tDATA*  table;
  tPTR          value;
	tDWORD        tmp;
  tDATA         pcount;
  tIID          iid;
	tINT          lock = 0;

  PR_TRACE_A0( 0, "Enter _InterfaceGetProp" );

  if ( (prop == pgPLUGIN_NAME) && iface->plugin ) {
		tHANDLE* plugin;
		if ( _HCP(plugin,po,lock,iface->plugin,&error) ) {
			error = _PropertyGetStr( po, plugin, &pcount, pgMODULE_NAME, buffer, count, cCP_ANSI );
			runlockc( po, lock );
		}
    goto exit;
  }
  
  pcount = sizeof(tDWORD);
  switch ( prop ) {
    case pgINTERFACE_ID            : value = &iface->iid;             break;
    case pgINTERFACE_CODEPAGE      : value = &iface->cp;              break;
    case pgPLUGIN_ID               : value = &iface->pid;             break;
    case pgINTERFACE_SUBTYPE       : value = &iface->subtype;         break;
    case pgINTERFACE_VERSION       : value = &iface->version;         break;
    case pgVENDOR_ID               : value = &iface->vid;             break;
    case pgINTERFACE_FLAGS         : value = &iface->flags;           break;
    case pgTRACE_LEVEL_MIN         : value = &tmp; tmp = iface->trace_level.min; break;
    case pgTRACE_LEVEL             : value = &tmp; tmp = iface->trace_level.max; break;

    case psINTERFACE_COMPATIBILITY_BASE : 
      error = _InterfaceCompatibleTopBase( iface, &iid );
      if ( PR_SUCC(error) )
        value = &iid;
      else
        value = 0;
      break;

    case pgINTERFACE_COMPATIBILITY : 
      if ( (buffer == &iface->compat) || (iface->compat == IID_ANY) ) 
        ;
      else {
        value = &iface->compat; 
        break;
      }

    default:
      table = /*(iface->flags & IFACE_LOADED) ?*/ _PropTableSearch(iface,prop,0) /*: 0*/;
      if ( table && (PROP_MODE(table) & cPROP_BUFFER_SHARED) ) {
        pcount = PROP_SIZE(table);
				if ( PROP_MODE(table) == cPROP_BUFFER_SHARED_VAR )
          value = *(tPTR*)PROP_BUFFER(table);
        else if ( PROP_MODE(table) == cPROP_BUFFER_SHARED_PTR )
          value = (tPTR)PROP_BUFFER(table);
        else
          value = &((tDATA*)table)[1];  // (tPTR)&(PROP_BUFFER(table)); !!! Palm OS doesn't accept it
      }
      else {
				error = errPROPERTY_NOT_FOUND;
        pcount = 0;
        value  = 0;
      }
      break;
  }

  if ( !value ) { 
    if ( iface->plugin && iface->index ) {
			tDWORD pc;
      error = CALL_Plugin_GetInterfaceProperty( iface->plugin, &pc, iface->index, prop, buffer, count );
			pcount = pc;
		}
    else if ( PR_SUCC(error) )
      error = errPROPERTY_NOT_FOUND;
  }
  else if ( buffer ) {
    if ( pcount > count )
      pcount = count;
    if ( value && pcount ) 
      mcpy( buffer, value, (tDWORD)pcount );
  }

exit:
  if ( out_size )
    *out_size = pcount;

  if ( PR_FAIL(error) )
  {
	  if (PR_PROP_TYPE(prop) != pTYPE_CODEPAGE)
		  PR_TRACE(( 0,prtIMPORTANT,"krn\tShared interface(iid=%tiid) property(id=%tprop) error(%terr)",iface->iid,prop,error ));
  }

  PR_TRACE_A1( 0, "Leave _InterfaceGetProp ret tDWORD = %u (prop size)", pcount );
  return error;
}




