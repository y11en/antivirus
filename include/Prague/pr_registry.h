#if !defined( __pr_registry_h ) && defined( __cplusplus )
#define __pr_registry_h

#include <Prague/pr_cpp.h>
#ifdef _WIN32
	#include <Prague/plugin/p_win32_reg.h>
#endif
#include <Prague/plugin/p_dtreg.h>

// -----------------------------------------------------
// cAutoRegistry

class cAutoRegistry : public cAutoObj<cRegistry>
{
public:
	//! по умолчанию выставляем работу с 32 реестром [tBOOL bIsWow64_32key = cTRUE]
	cAutoRegistry( cObject* pOwner = NULL, const tCHAR* strRoot = NULL, tPID pid = PID_ANY, tBOOL bReadOnly = cTRUE,
		tBOOL bIsWow64_32key = cTRUE, tBOOL bIsWow64_64key = cFALSE ) {
		if ( pOwner )
			create( pOwner, strRoot, pid, bReadOnly, bIsWow64_32key, bIsWow64_64key );
	}

	// ---
	tERROR create( cObject* pOwner, const tCHAR* strRoot, tPID pid, tBOOL bReadOnly = cTRUE, tBOOL bIsWow64_32key = cTRUE, tBOOL bIsWow64_64key = cFALSE ) {
		m_last_error = pOwner->sysCreateObject((hOBJECT*)&m_obj, IID_REGISTRY, pid);
		if( PR_SUCC(m_last_error) )
			m_last_error = m_obj->propSetStr( 0, pgROOT_POINT, (tPTR)strRoot, cCP_ANSI );
		if( PR_SUCC(m_last_error) )
			m_last_error = m_obj->propSetDWord( pgOBJECT_RO, bReadOnly );
#ifdef _WIN32
		if( PR_SUCC(m_last_error) && (pid == PID_WIN32_REG) )
			m_last_error = m_obj->propSetBool(plWOW64_64KEY, bIsWow64_64key);
		if( PR_SUCC(m_last_error) && (pid == PID_WIN32_REG) )
			m_last_error = m_obj->propSetBool(plWOW64_32KEY, bIsWow64_32key);
#endif
		if( PR_SUCC(m_last_error) )
			m_last_error = m_obj->sysCreateObjectDone();
		if ( PR_FAIL(m_last_error) && m_obj ) {
			m_obj->sysCloseObject();
			m_obj = 0;
		}
		return m_last_error;
	}
	
	// ---
	tERROR create_dt( cObject* pOwner, const tCHAR* file = 0, const tCHAR* root = 0, tBOOL bReadOnly = cTRUE, tBOOL bNew = cFALSE, tBOOL save_file = cTRUE ) {
		m_last_error = pOwner->sysCreateObject( (hOBJECT*)&m_obj, IID_REGISTRY, PID_DTREG );
		if ( PR_SUCC(m_last_error) && file && *file ) 
			m_last_error = m_obj->propSetStr( 0, pgOBJECT_NAME, (tPTR)file, cCP_ANSI );
		if( PR_SUCC(m_last_error) && root && *root )
			m_last_error = m_obj->propSetStr( 0, pgROOT_POINT, (tPTR)root, cCP_ANSI );
		if( PR_SUCC(m_last_error) )
			m_last_error = m_obj->propSetDWord( pgOBJECT_RO, bReadOnly );
		if( PR_SUCC(m_last_error) && bNew )
			m_last_error = m_obj->propSetBool( pgCLEAN, bNew );
		if ( PR_SUCC(m_last_error) && save_file )
			m_last_error = m_obj->propSetBool( pgSAVE_RESULTS_ON_CLOSE, save_file );
		if( PR_SUCC(m_last_error) )
			m_last_error = m_obj->sysCreateObjectDone();
		if ( PR_FAIL(m_last_error) && m_obj ) {
			m_obj->sysCloseObject();
			m_obj = 0;
		}
		return m_last_error;
	}
	
	tERROR last_error(){ return m_last_error; }

private:
	tERROR m_last_error;
};



// ---
class cAutoRegKey {
protected:
	hREGISTRY m_reg;
	tRegKey   m_key;
	
public:
	cAutoRegKey() : m_reg(0), m_key(cRegNothing) {}
	cAutoRegKey( hREGISTRY reg, tRegKey key ) : m_reg(reg), m_key(key) {}
	cAutoRegKey( hREGISTRY reg, tRegKey parent, const tCHAR* path, tBOOL create_if_not_exist = cFALSE ) 
		: m_reg(0), m_key(cRegNothing) { 
			if ( reg && (parent != cRegNothing) ) 
				open( reg, parent, path, create_if_not_exist ); 
	}
	cAutoRegKey( hREGISTRY reg, tRegKey parent, tUINT index ) 
		: m_reg(0), m_key(cRegNothing) { 
			if ( reg && (parent != cRegNothing) ) {
				if ( PR_SUCC(CALL_Registry_OpenKeyByIndex(reg,&m_key,parent,index,cFALSE)) )
					m_reg = reg;
			}
	}
	cAutoRegKey( const cAutoRegKey& parent, const tCHAR* path, tBOOL create_if_not_exist = cFALSE ) 
		: m_reg(0), m_key(cRegNothing) { 
			if ( parent.m_reg && (parent.m_key != cRegNothing) ) 
				open( parent.m_reg, parent.m_key, path, create_if_not_exist ); 
	}
	cAutoRegKey( const cAutoRegKey& parent, tUINT index ) 
		: m_reg(0), m_key(cRegNothing) { 
			if ( parent.m_reg && (parent.m_key != cRegNothing) ) {
				if ( PR_SUCC(CALL_Registry_OpenKeyByIndex(parent.m_reg,&m_key,parent.m_reg,index,cFALSE)) )
					m_reg = parent.m_reg;
			}
	}
	~cAutoRegKey() { close(); }

	operator hREGISTRY () const { return m_reg; }
	operator tRegKey () const { return m_key; }
	tBOOL opened() const { return (m_key == cRegNothing) ? cFALSE : cTRUE; }

	tRegKey attach(hREGISTRY reg, tRegKey key) { tRegKey last = detach(); m_reg = reg; m_key = key; return last; }
	tRegKey detach() { tRegKey last = m_key; m_reg = NULL; m_key = cRegNothing; return last; }

	tERROR open( hREGISTRY reg, tRegKey parent, const tCHAR* path, tBOOL create = cFALSE/*, tBOOL delete_old = cFALSE*/ ) {
		close();
		if ( !reg )
			return errPARAMETER_INVALID;
		tERROR err = errOK;
		//if ( delete_old && (m_key != cRegNothing) )
		//	CALL_Registry_DeleteKey( reg, parent, path);
		if ( PR_SUCC(err) )
			err = CALL_Registry_OpenKey( reg, &m_key, parent, path, create );
		if ( PR_SUCC(err) )
			m_reg = reg;
		return err;
	}

	tERROR open_by_index( cAutoRegKey& key, tUINT index ) {
		if ( !m_reg || (m_key == cRegNothing) )
			return errOBJECT_BAD_INTERNAL_STATE;
		if ( key.m_reg && (key.m_reg != m_reg) )
			return errPARAMETER_INVALID;
		key.close();
		cERROR err = CALL_Registry_OpenKeyByIndex( m_reg, &key.m_key, m_key, index, cFALSE );
		if ( PR_SUCC(err) )
			key.m_reg = m_reg;
		return err;
	}

	tERROR close() { 
		tERROR err = errOK; 
		if ( m_reg && (m_key != cRegRoot) && (m_key != cRegNothing) ) { 
			err = CALL_Registry_CloseKey( m_reg, m_key ); 
			m_reg = 0; 
			m_key = cRegNothing; 
		} 
		return err; 
	}

	tERROR get_key_name_by_index( tUINT index, tCHAR* buff, tDWORD buff_size )
	{
		if ( !m_reg || (m_key == cRegNothing) )
			return errOBJECT_BAD_INTERNAL_STATE;
		return m_reg->GetKeyNameByIndex( 0, m_key, index, buff, buff_size, 0 );
	}

	tERROR get_val(const tCHAR* name, tTYPE_ID type, tPTR value, tDWORD* size = NULL)
	{
		return _get_val(false, name, type, value, size);
	}

	tERROR get_val_by_index(tUINT index, tTYPE_ID type, tPTR value, tDWORD* size = NULL)
	{
		return _get_val(true, (const tCHAR*)index, type, value, size);
	}
	
	tERROR _get_val(bool _by_idx, const tCHAR* name, tTYPE_ID type, tPTR value, tDWORD* size)
	{
		if( !m_reg || m_key == cRegNothing )
			return errOBJECT_NOT_INITIALIZED;
		if( !value )
			return errPARAMETER_INVALID;

		tERROR err = errOK;
		tDWORD nSize;
		tTYPE_ID nRegType;
		switch( type )
		{
		case tid_STRING_OBJ :
			{
				nSize = 0;
				nRegType = tid_WSTRING;
				if( PR_FAIL(err=_by_idx ? m_reg->GetValueByIndex(&nSize, m_key, (tDWORD)name, &nRegType, NULL, nSize) : m_reg->GetValue(&nSize, m_key, name, &nRegType, NULL, nSize)) )
					return err;
				tChunckBuff buff;
				tCHAR* pData = buff.get(nSize, false);
				if( PR_FAIL(err=_by_idx ? m_reg->GetValueByIndex(&nSize, m_key, (tDWORD)name, &nRegType, pData, nSize) : m_reg->GetValue(&nSize, m_key, name, &nRegType, pData, nSize)) )
					return err;
				((cStrObj*)value)->assign(pData, cCP_UNICODE, nSize);
			}
			break;

		case tid_QWORD :
			nSize = sizeof(tQWORD);
			nRegType = tid_QWORD;
			if( PR_FAIL(err = _by_idx ? m_reg->GetValueByIndex(&nSize, m_key, (tDWORD)name, &nRegType, value, nSize) : m_reg->GetValue(&nSize, m_key, name, &nRegType, value, nSize)) )
				return err;
			break;

		case tid_DWORD :
			nSize = sizeof(tDWORD);
			nRegType = tid_DWORD;
			if( PR_FAIL(err = _by_idx ? m_reg->GetValueByIndex(&nSize, m_key, (tDWORD)name, &nRegType, value, nSize) : m_reg->GetValue(&nSize, m_key, name, &nRegType, value, nSize)) )
				return err;
			break;

		case tid_BINARY:
			if( !size )
				return errPARAMETER_INVALID;
			nRegType = tid_BINARY;
			err = _by_idx ? m_reg->GetValueByIndex(size, m_key, (tDWORD)name, &nRegType, value, *size) : m_reg->GetValue(size, m_key, name, &nRegType, value, *size);
			if( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) )
				*size = 0;
		break;

		default:
			return errNOT_IMPLEMENTED;
		}
		
		return err;
	}

	tERROR set_val(const tCHAR* name, tTYPE_ID type, tPTR value, tDWORD size = 0, tBOOL bCreateIfNotExist = cTRUE)
	{
		return _set_val(false, name, type, value, size, bCreateIfNotExist);
	}

	tERROR set_val_by_index(tUINT index, tTYPE_ID type, tPTR value, tDWORD size = 0, tBOOL bCreateIfNotExist = cTRUE)
	{
		return _set_val(true, (const tCHAR*)index, type, value, size, bCreateIfNotExist);
	}

	tERROR _set_val(bool _by_idx, const tCHAR* name, tTYPE_ID type, tPTR value, tDWORD size = 0, tBOOL bCreateIfNotExist = cTRUE)
	{
		if( !m_reg || m_key == cRegNothing )
			return errOBJECT_NOT_INITIALIZED;
		if( !value )
			return errPARAMETER_INVALID;
		
		switch( type )
		{
			case tid_STRING_OBJ :
				{
					cStrObj& strData = *(cStrObj *)value;
					type = tid_WSTRING;
					size = sizeof(tWCHAR)*strData.size();
					value = (tPTR)strData.data();
				}
				break;
			case tid_QWORD  : size = sizeof(tQWORD); break;
			case tid_DWORD  : size = sizeof(tDWORD); break;
			case tid_BINARY : break;
			default         : return errNOT_IMPLEMENTED;
		}
		
		return _by_idx ? m_reg->SetValueByIndex(m_key, (tDWORD)name, type, value, size, bCreateIfNotExist) : m_reg->SetValue(m_key, name, type, value, size, bCreateIfNotExist);
	}

	tERROR get_strobj(const tCHAR* name, cStrObj& val) { return get_val(name, tid_STRING_OBJ, &val); }
	tERROR set_strobj(const tCHAR* name, const cStrObj& val, tBOOL bCreateIfNotExist = cTRUE) { return set_val(name, tid_STRING_OBJ, (tPTR)&val, 0, bCreateIfNotExist); }

	tERROR get_dword(const tCHAR* name, tDWORD& val) { return get_val(name, tid_DWORD, &val); }
	tERROR set_dword(const tCHAR* name, tDWORD val, tBOOL bCreateIfNotExist = cTRUE) { return set_val(name, tid_DWORD, &val, 0, bCreateIfNotExist); }

	tERROR get_qword(const tCHAR* name, tQWORD& val) { return get_val(name, tid_QWORD, &val); }
	tERROR set_qword(const tCHAR* name, tQWORD val, tBOOL bCreateIfNotExist = cTRUE) { return set_val(name, tid_QWORD, &val, 0, bCreateIfNotExist); }

	tERROR del_val( const tCHAR* name ) {
		if( !m_reg || m_key == cRegNothing )
			return errOBJECT_NOT_INITIALIZED;
		return m_reg->DeleteValue(m_key, name);
	}

	tERROR del_key( const tCHAR* name ) {
		if( !m_reg || m_key == cRegNothing )
			return errOBJECT_NOT_INITIALIZED;
		return m_reg->DeleteKey( m_key, name );
	}

	tDWORD get_val_count() {
		if( !m_reg || m_key == cRegNothing )
			return 0;
		tDWORD _count = 0; m_reg->GetValueCount(&_count, m_key);
		return _count;
	}

	tDWORD get_key_count() {
		if( !m_reg || m_key == cRegNothing )
			return 0;
		tDWORD _count = 0; m_reg->GetKeyCount(&_count, m_key);
		return _count;
	}

	tDWORD get_count() { return get_key_count() + get_val_count(); }
};

// -----------------------------------------------------

#endif  //__pr_registry_h
