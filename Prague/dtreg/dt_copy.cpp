#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_reg.h>

#define RET_IF_ERR(a)  if (PR_FAIL(e=a)) return e;
#define cStartBuffLen  ((tDWORD)(256)) // maximum name length



// ---
typedef cBuff<tCHAR,cStartBuffLen> cChBuff;

// ---
tERROR get_val( cRegistry* reg, tRegKey key, tDWORD ind, tTYPE_ID* type, tDWORD* len, cChBuff& buff );
tERROR get_val_name( cRegistry* reg, tRegKey key, tDWORD ind, cChBuff& buff );
tERROR get_key_name( cRegistry* reg, tRegKey key, tDWORD ind, cChBuff& buff );
tERROR reg_copy( cRegistry* from, tRegKey from_key, cRegistry* to, tRegKey to_key );


// ---
extern "C" tERROR Reg_CopyKey( hREGISTRY from, tRegKey from_key, hREGISTRY to, tRegKey to_key ) {
  return reg_copy( (cRegistry*)from, from_key, (cRegistry*)to, to_key );
}


// ---
tERROR reg_copy( cRegistry* from, tRegKey from_key, cRegistry* to, tRegKey to_key ) {

  cERROR   e;
  tDWORD   i, count;
  cChBuff  name(from);
  cChBuff  value(from);
  
	e = from->GetValueCount( &count, from_key );
  for( i=0; PR_SUCC(e) && i<count; i++ ) {
    tTYPE_ID type;
    tDWORD   vlen;
    e = get_val_name( from, from_key, i, name );
		if ( PR_SUCC(e) )
			e = get_val( from, from_key, i, &type, &vlen, value );
		if ( PR_SUCC(e) )
			e = to->SetValue( to_key, name, type, value, vlen, cTRUE );
  }

  if ( PR_SUCC(e) )
		e = from->GetKeyCount( &count, from_key );
  for( i=0; PR_SUCC(e) && i<count; i++ ) {
    tDWORD name_len = 0;
    tRegKey to_next_key = cRegNothing;
    tRegKey from_next_key = cRegNothing;
		e = get_key_name( from, from_key, i, name );
		if ( PR_SUCC(e) )
			e = from->OpenKey( &from_next_key, from_key, name, cFALSE );
		if ( PR_SUCC(e) )
			e = to->OpenKey( &to_next_key, to_key, name, cTRUE );
    name.clean();
		if ( PR_SUCC(e) )
			e = reg_copy( from, from_next_key, to, to_next_key );
		if ( to_next_key != cRegNothing)
			to->CloseKey( to_next_key );
		if ( from_next_key )
			from->CloseKey( from_next_key );
  }
  return e;
}




// ---
tERROR get_val( cRegistry* reg, tRegKey key, tDWORD ind, tTYPE_ID* type, tDWORD* len, cChBuff& buff ) {
  tERROR e;
  if ( PR_FAIL(e=reg->GetValueByIndex(len,key,ind,0,0,0)) )
    return e;
  if ( !buff.get(*len,false) )
    return errNOT_ENOUGH_MEMORY;
  return reg->GetValueByIndex( 0, key, ind, type, buff, *len );
}



// ---
tERROR get_val_name( cRegistry* reg, tRegKey key, tDWORD ind, cChBuff& buff ) {
  tERROR e;
  tUINT len;
  if ( PR_FAIL(e=reg->GetValueNameByIndex(&len,key,ind,0,0)) )
    return e;
  if ( !buff.get(len,false) )
    return errNOT_ENOUGH_MEMORY;
  return reg->GetValueNameByIndex( 0, key, ind, buff, len );
}



// ---
tERROR get_key_name( cRegistry* reg, tRegKey key, tDWORD ind, cChBuff& buff ) {
  tERROR e;
  tUINT len;
  if ( PR_FAIL(e=reg->GetKeyNameByIndex(&len,key,ind,0,0,cFALSE)) )
    return e;
  if ( !buff.get(len,false) )
    return errNOT_ENOUGH_MEMORY;
  return reg->GetKeyNameByIndex( 0, key, ind, buff, len, cFALSE );
}

