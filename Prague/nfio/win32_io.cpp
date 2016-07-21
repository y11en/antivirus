// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  27 June 2006,  12:10 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_io.cpp
// Additional info
//    File input/output for Win32
// -------------------------------------------
// AVP Prague stamp end



//#define _PERFORM_TEST_


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_os.h>
#include "../../commonfiles/wincompat.h"
#include <Prague/plugin/p_win32_nfio.h>
// AVP Prague stamp end



#include <Prague/pr_oid.h>
#include "win32_io.h"
#include "win32_os.h"
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_token.h>

#include <Prague/pr_cpp.h>
#include <Prague/pr_wtime.h>

#define  IMPEX_IMPORT_LIB
#include <Prague/plugin/p_fsdrvplgn.h>

extern "C" tBOOL  g_bFSDRVImportresolved;
extern "C" tDWORD g_dwTotalWrites;

// ---
const unsigned DEF_BUFF_SIZE = 0x200;

typedef cBuff<tCHAR,80>  cChBuff;
typedef cBuff<tWCHAR,80> cwChBuff;


#define ARE_BITS_SET(v,m) ( (m) == ((v)&(m)) )

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Win32IOData : public cIO /*cObjImpl*/ {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call GetName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SetName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SetPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_access( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_REOPEN_DATA( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_REOPEN_DATA( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SetFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call iGetFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call iSetFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call check_exclusive( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call IsRemote( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetNativePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetShortFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetShortName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLongFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call GetLongName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call _get_security( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call _set_security( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
	tCHAR*  m_full_name;                           // full name buffer
	tDWORD  m_full_name_buff_len;                  // length of the buffer
	tSTRING m_ppath;                               // pointer to the full path
	tSTRING m_pname;                               // pointer to the file name
	HANDLE  m_handle;                              // windows file handle
	tQWord  m_cp;                                  // current file position
	tDWORD  m_open_mode;                           // open mode
	tDWORD  m_access_mode;                         // file attributes
	tQWord  m_size;                                // file size
	tDWORD  m_open_attr;                           // open time file attributes
	tBOOL   m_del_on_close;                        // must be deleted on close
	cOS*    m_os;                                  // owner os
	tDWORD  m_win32err;                            // last win32 error
	tDWORD  m_origin;                              // object origin
	tDWORD  m_has_to_restore_attribs;              // --
	tBOOL   m_user_wants_to_restore_attribs;       // file attributes has been changed on InitDone and need to be resore
	tBOOL   m_user_wants_to_restore_file_times;    // file times has to be unchanged on write operations
	tBOOL   m_user_wants_to_restore_access_time;   // file last aceess time has to be restored on close
	tBOOL   m_user_wants_to_restore_modified_time; // file modified has to be restored on close
	tBOOL   m_ignore_ro_attrib;                    // --
	tBOOL   m_control_size_on_read;                // check and control size on operations
	tBOOL   m_extended_name_processing;            // cut "\??\" prefix and expand environment variables in the file name
	tDWORD  m_preparse_name;                       // --
	bool    m_written_something;                   // some byte were written to the file successfully
	tBOOL   m_short_name_4_reopen;                 // --
// AVP Prague stamp end



	tBOOL    m_reopen_mode;
	tBOOL    m_direct_mode;
	PSECURITY_DESCRIPTOR m_secure_to_restore;

	// Property function declarations
	tERROR pr_call SetNameW( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size );
	tERROR pr_call SetPathW( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size );
	
	// private utils
	tERROR pr_call iSetName( tSTRING buffer, tDWORD* size );
	tERROR pr_call iSetPath( tSTRING buffer, tDWORD* size );
	tERROR pr_call iSetFull( tSTRING buffer, tDWORD* size, tDWORD path_len );
	tERROR pr_call iSetNameW( tWSTRING buffer, tDWORD* size );
	tERROR pr_call iSetPathW( tWSTRING buffer, tDWORD* size );
	tERROR pr_call iSetFullW( tWSTRING buffer, tDWORD* size, tDWORD path_len );
	tERROR pr_call FillUpTo( tQWORD* offset );
	tERROR pr_call processString( tCHAR*& str, tDWORD& size, cChBuff& local_buff );
	tERROR pr_call processStringW( tWCHAR*& str, tDWORD& size, cwChBuff& local_buff );
	tERROR pr_call prepareReopenExecutable();
	tERROR pr_call prepareElevatedToken(cAutoObj<cToken> &hToken, tCHAR*& path, tDWORD& len, tCHAR*& new_path);
	tDWORD pr_call GetSubstPrefix( tCHAR* buff, tDWORD* dwOffset, tBOOL bUNC);

	tERROR pr_call parse_name( bool* the_same, tCHAR*& full, tDWORD& size, tCHAR*& path, tCHAR* name );
	tVOID  pr_call check_slashes();
	tVOID  pr_call check_slashesW();

	bool extended_name_processing() const {
		return m_extended_name_processing ? true : false;
	}

	bool windows_name_parsing() const {
		return (m_open_mode & fOMODE_FILE_NAME_PREPARSING) ? true : false;
	}
	//FILETIME m_open_access_time;                    // open access time and date
	//FILETIME m_open_modification_time;              // open modification time and date

	//---
	friend struct cSavedTimes;
	struct cSavedTimes {
		HANDLE    m_h;
		bool      m_restored;
		FILETIME  m_at;
		FILETIME  m_mt;
#if defined(_DEBUG)
			SYSTEMTIME m_sat;
			SYSTEMTIME m_smt;
#endif
		
		cSavedTimes() : m_h(0), m_restored(false) { 
			m_at.dwLowDateTime = m_at.dwHighDateTime = m_mt.dwLowDateTime = m_mt.dwHighDateTime = 0; 
		}
		
		static bool check( bool* acc, bool* mod, tBOOL restore_access, tBOOL restore_modified, tDWORD access );

		bool        init( HANDLE h, tBOOL restore_access, tBOOL restore_modified, tDWORD access, const FILETIME* last_access, const FILETIME* modified );
		tERROR      restore();

		operator bool () const;
	} m_saved_times;

	#if defined( _PERFORM_TEST_ )
		static tERROR pr_call _test_0( Win32IOData* io, cStrObj& obj, const tWCHAR* path );
		static tERROR pr_call _test();
		#define _call_test()  _test()
	#else
		#define _call_test()
	#endif
	
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(Win32IOData)
};
// AVP Prague stamp end



// ---
inline const FILETIME* _validate_ft( const FILETIME& ft ) {
	return (ft.dwLowDateTime || ft.dwHighDateTime) ? &ft : 0;
}

// ---
inline Win32IOData::cSavedTimes::operator bool () const { 
	return _validate_ft(m_at) || _validate_ft(m_mt); 
}

// ---
bool Win32IOData::cSavedTimes::init( HANDLE h, tBOOL restore_access, tBOOL restore_modified, tDWORD access, const FILETIME* last_access, const FILETIME* modified ) {
	
	bool acc, mod;
	if ( check(&acc,&mod,restore_access,restore_modified,access) ) {
		m_h = h;
		m_restored = false;
		if ( run_under_nt ) {
			if ( acc )
				m_at.dwLowDateTime = m_at.dwHighDateTime = -1;
			else
				m_at.dwLowDateTime = m_at.dwHighDateTime = 0;
			if ( mod )
				m_mt.dwLowDateTime = m_mt.dwHighDateTime = -1; 
			else
				m_mt.dwLowDateTime = m_mt.dwHighDateTime = 0; 
		}
		else {
			if ( acc && last_access )
				m_at = *last_access;
			else
				m_at.dwLowDateTime = m_at.dwHighDateTime = 0;
			if ( mod && modified )
				m_mt = *modified;
			else
				m_mt.dwLowDateTime = m_mt.dwHighDateTime = 0; 
		}
#if defined(_DEBUG)
		FileTimeToSystemTime( &m_at, &m_sat );
		FileTimeToSystemTime( &m_mt, &m_smt );
#endif
		return true;
	}
	else {
		m_h = 0;
		m_restored = true;
		m_at.dwLowDateTime = m_at.dwHighDateTime = m_mt.dwLowDateTime = m_mt.dwHighDateTime = 0; 
#if defined(_DEBUG)
		memset( &m_sat, 0, sizeof(m_sat) );
		memset( &m_smt, 0, sizeof(m_sat) );
#endif
		return false;
	}
}



// ---
tVOID Win32IOData::check_slashesW() {
	tWCHAR* dst = UNI(m_full_name) + 1;
	if ( dst ) {
		tWCHAR* src = dst;
		bool slash = false;
		tUINT deleted_slash_bytes = 0;
		for( ; *src; ++src ) {
			if ( *src == L'/' ) {
				if ( slash )
					deleted_slash_bytes += UNIS;
				else
					*dst++ = L'\\';
				slash = true;
			}
			else if ( *src == L'\\' ) {
				if ( slash )
					deleted_slash_bytes += UNIS;
				else if ( src > dst )
					*dst++ = L'\\';
				else
					++dst;
				slash = true;
			}
			else {
				if ( src > dst )
					*dst = *src;
				++dst;
				slash = false;
			}
		}
		if ( src > dst )
			*dst = 0;
		m_pname -= deleted_slash_bytes;
	}
}


#if defined( _WIN32 )
#  define WRONG_DELIMITER_CHAR '/'
#else
#  define WRONG_DELIMITER_CHAR '\\'
#endif

// ---
tVOID Win32IOData::check_slashes() {
	tCHAR* dst = MB(m_full_name) + 1;
	if ( dst ) {
		tCHAR* src = dst;
		bool slash = false;
		tUINT deleted_slash_bytes = 0;
		for( ; *src; ++src ) {
			if ( *src == WRONG_DELIMITER_CHAR ) {
				if ( slash )
					deleted_slash_bytes += MBS;
				else
					*dst++ = PATH_DELIMETER_CHAR;
				slash = true;
			}
			else if ( *src == PATH_DELIMETER_CHAR ) {
				if ( slash )
					deleted_slash_bytes += MBS;
				else if ( src > dst )
					*dst++ = PATH_DELIMETER_CHAR;
				else
					++dst;
				slash = true;
			}
			else {
				if ( src > dst )
					*dst = *src;
				++dst;
				slash = false;
			}
		}
		if ( src > dst )
			*dst = 0;
		m_pname -= deleted_slash_bytes;
	}
}



// ---
bool Win32IOData::cSavedTimes::check( bool* pacc, bool* pmod, tBOOL restore_access, tBOOL restore_modified, tDWORD access ) {
	bool acc = restore_access && (0 != (access & fACCESS_RW));
	bool mod = restore_modified && (0 != (access & fACCESS_WRITE));
	if ( pacc )
		*pacc = acc;
	if ( pmod )
		*pmod = mod;
	return acc || mod;
}



// ---
tERROR Win32IOData::cSavedTimes::restore() {

	if ( m_h ) {
		if ( m_restored )
			return errOK;
		m_restored = true;
		if ( SetFileTime(m_h,0,_validate_ft(m_at),_validate_ft(m_mt)) )
			return errOK;
		return _ConvertWin32Err( GetLastError() );
	}
	return errPARAMETER_INVALID;
}



#define IS_ATTR(a) ( (a) != ((tDWORD)-1) )

// ---
tERROR Win32IOData::processString( tCHAR*& str, tDWORD& size, cChBuff& local_buff ) {

	if ( !size )
		return errOK;

	if ( !memcmp(str,"\\??\\",4 * sizeof(tCHAR)) ) {
		((tCHAR*&)str) += 4 * sizeof(tCHAR);
		size -= 4 * sizeof(tCHAR);
	}

	if ( !strchr((LPCSTR)str,'%') )
		return errOK;

	tDWORD len = ExpandEnvironmentStrings( (LPCSTR)str, 0, 0 );
	if ( !len )
		return errUNEXPECTED;
	else if ( 0 == local_buff.get(len,false) )
		return errNOT_ENOUGH_MEMORY;
	else if ( ExpandEnvironmentStrings((LPCSTR)str,(LPSTR)local_buff,len) ) {
		str = (tCHAR*)local_buff;
		size = len;
		return errOK;
	}
	else
		return errUNEXPECTED;
}


// ---
tERROR Win32IOData::processStringW( tWCHAR*& str, tDWORD& size, cwChBuff& local_buff ) {
#if defined (_WIN32)	
	if ( !size )
		return errOK;
	
	if ( !memcmp(str,L"\\??\\",4 * sizeof(tWCHAR)) ) {
		((tCHAR*&)str) += 4 * sizeof(tWCHAR);
		size -= 4 * sizeof(tWCHAR);
	}

	if ( !wcschr((LPCWSTR)str,L'%') )
		return errOK;

	tDWORD len = ExpandEnvironmentStringsW( (LPCWSTR)str, 0, 0 );

	if ( !len )
		return errUNEXPECTED;
	else if ( 0 == local_buff.get(len,false) )
		return errNOT_ENOUGH_MEMORY;
	else if ( ExpandEnvironmentStringsW((LPCWSTR)str,(LPWSTR)local_buff,len) ) {
		str = (tWCHAR*)local_buff;
		size = len * sizeof(tWCHAR);
		return errOK;
	}
	else
#endif
		return errUNEXPECTED;
}



// ---
tERROR Win32IOData::prepareReopenExecutable() {

	if (!run_under_nt)
		return errNOT_SUPPORTED;

	HANDLE hProcessToken = NULL, hThreadToken = NULL, hToken;
	HANDLE hFile = NULL;
	SECURITY_DESCRIPTOR pDescr;
	PTOKEN_OWNER pTokenInfo = NULL;
	PACL pACL = NULL;
	DWORD  size = 0;
	tERROR error = errOK;

	if( m_secure_to_restore )
		return errOK;

	hFile = fCreateFile(m_full_name, GENERIC_EXECUTE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return errUNEXPECTED;

	CloseHandle(hFile);

	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken);
	OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hThreadToken);
	hToken = hThreadToken ? hThreadToken : hProcessToken;
	if( hToken )
		GetTokenInformation(hToken, TokenOwner, NULL, 0, &size);

	if( !size )
		error = errUNEXPECTED;

	if( PR_SUCC(error) )
		error = heapAlloc((tPTR*)&pTokenInfo, size);

	if( PR_SUCC(error) && !GetTokenInformation(hToken, TokenOwner, pTokenInfo, size, &size) )
		error = errUNEXPECTED;

	if( PR_SUCC(error) ) {
		PSID pSID = pTokenInfo->Owner;
		DWORD dwLengthACL =	GetLengthSid(pSID) + sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);

		error = heapAlloc((tPTR*)&pACL, dwLengthACL);
		if( PR_SUCC(error) )
			if( !InitializeAcl(pACL, dwLengthACL, ACL_REVISION) ||
				!AddAccessAllowedAce(pACL, ACL_REVISION, GENERIC_ALL, pSID) ||
				!InitializeSecurityDescriptor(&pDescr, SECURITY_DESCRIPTOR_REVISION) ||
				!SetSecurityDescriptorDacl(&pDescr, TRUE, pACL, FALSE) )
				error = errUNEXPECTED;
	}

	if( PR_SUCC(error) ) {
//		if( hThreadToken )
//			SetThreadToken(NULL, hProcessToken);

		if( !m_secure_to_restore ) {
			size = 0;
			fGetFileSecurity(m_full_name, DACL_SECURITY_INFORMATION, NULL, 0, &size);
			if( !size )
				error = errUNEXPECTED;

			if( PR_SUCC(error) )
				error = heapAlloc(&m_secure_to_restore, size);

			if( PR_SUCC(error) && !fGetFileSecurity(m_full_name, DACL_SECURITY_INFORMATION, m_secure_to_restore, size, &size) )
				error = errUNEXPECTED;
		}

		if( PR_SUCC(error) && !fSetFileSecurity(m_full_name, DACL_SECURITY_INFORMATION, &pDescr) )
			error = errUNEXPECTED;

//		if( hThreadToken )
//			SetThreadToken(NULL, hThreadToken);
	}

	if( pACL )
		heapFree(pACL);

	if( pTokenInfo )
		heapFree(pTokenInfo);

	if( hThreadToken )
		CloseHandle(hThreadToken);

	if( hProcessToken )
		CloseHandle(hProcessToken);

	return error;
}

// ---
tERROR Win32IOData::prepareElevatedToken(cAutoObj<cToken> &hToken, tCHAR*& path, tDWORD& path_len, tCHAR*& new_path) {

	tDWORD len = path_len;
	tERROR error = GetNativePath(&len, 0, path, path_len);
	if( error == errOK )
	{
		error = heapAlloc( (tPTR*)&path, len );
		if ( PR_SUCC(error) )
			error = GetNativePath(&len, 0, path, path_len=len);

	}

	if( PR_SUCC(error) )
	{
		new_path = path;
		PR_TRACE(( this, prtNOTIFY, "nfio\tprepareElevatedToken new path - \"%S\"", new_path));
	}

	error = sysCreateObjectQuick(hToken, IID_TOKEN, PID_ANY);
	if( PR_SUCC(error) )
		error = hToken->Impersonate(eitAsAdmin);

	if( PR_FAIL(error) )
		hToken.clean();

	return error;
}

// ---
tERROR pr_call Win32IOData::parse_name( bool* the_same, tCHAR*& full, tDWORD& size, tCHAR*& path, tCHAR* file ) {

	if ( the_same )
		*the_same = false;
	
	tDWORD new_size = fGetFullPathName( m_full_name, 0, 0, 0 );
	if ( !new_size )
		return _ConvertWin32Err( GetLastError() );

	tUINT mem_size;
	if ( run_under_nt ) {
		new_size += UNIS;
		mem_size = UNIS * new_size;
	}
	else {
		new_size += MBS;
		mem_size = MBS * new_size;
	}

	cERROR error = heapAlloc( (tPTR*)&full, mem_size );
	if ( PR_SUCC(error) ) {
		new_size = fGetFullPathName( m_full_name, new_size, full, (const tVOID**)&file );
		if ( !new_size )
			return  _ConvertWin32Err( GetLastError() );

		size = mem_size;
		path = full + (m_ppath - m_full_name);

		if ( the_same ) {
			tBOOL same = run_under_nt ? !lstrcmpW(UNI(m_full_name),UNI(full)) : !lstrcmpA(m_full_name,full);
			if ( same )
				*the_same = true;
			else
				*the_same = false;
		}

	}
	return error;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR Win32IOData::ObjectInit() {
	tERROR error;
	PR_TRACE_A0( this, "Enter IO::ObjectInit method" );
	
	if ( !fCreateFile ) {
		PR_TRACE(( this, prtERROR, "nfio\tCreateFile export pointer cannot be imported from the Windows kernel" ));
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	else {
		m_win32err = 0;
		m_open_mode = fOMODE_OPEN_IF_EXIST;
		m_access_mode = fACCESS_READ;
		m_origin = OID_GENERIC_IO;
		m_control_size_on_read = cTRUE;
		m_ignore_ro_attrib = cTRUE;
		m_user_wants_to_restore_attribs = cTRUE;
		m_user_wants_to_restore_file_times = cFALSE;
		m_user_wants_to_restore_access_time = cFALSE;   // file last aceess time has to be restored on close
		m_user_wants_to_restore_modified_time = cFALSE; // file modified has to be restored on close

		m_written_something = false;
		m_open_attr = -1;
		
		cObject* los = (cObject*)sysGetParent( IID_ANY );
		if ( !los )
			m_os = 0;
		else if ( IID_OS != los->propGetIID() )
			m_os = 0;
		//else if ( PID_WIN32_NFIO != los->propGetPID() )
		//	m_os = 0;
		else
			m_os = (cOS*)los;
		/*
		while( los ) {
			cObj* child;
			los->sysGetFirstChild( (hOBJECT*)&child, IID_IO, PID_WIN32_NFIO );
			while( child ) {
				if ( child == this ) {
					m_os = (cOS*)los;
					los = 0;
					break;
				}
				if ( PR_FAIL(child->sysGetNextChild((hOBJECT*)&child)) ) {
					los = 0;
					break;
				}
			}
			if ( los )
				los = (cObj*)los->sysGetParent( IID_OS );
		}
		*/
		error = errOK;
	}

	PR_TRACE_A1( this, "Leave IO::ObjectInit method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR Win32IOData::ObjectInitDone() {
	cERROR error;
	PR_TRACE_FUNC_FRAME__( "IO::ObjectInitDone method", &error._ );
	PR_TRACE_VARS;

	m_win32err = ERROR_SUCCESS;

	if ( !m_full_name || (run_under_nt && (0 == *UNI(m_full_name))) || (!run_under_nt && (0 == *MB(m_ppath))) ) {
		m_handle = 0;
		error = errOBJECT_NOT_INITIALIZED;
		PR_TRACE(( this, prtERROR, "nfio\tIO::ObjectInitDone -- err=errOBJECT_NOT_INITIALIZED, bad object name (%s%S)", PR_TRACE_PREPARE_STR_CP(m_ppath,codepage) ));
	}
	else if ( (m_access_mode & fACCESS_CHECK_WRITE) && !(m_access_mode & fACCESS_WRITE) ) {
		m_handle = 0;
		error = errPARAMETER_INVALID;
		PR_TRACE(( this, prtERROR, "nfio\tIO::ObjectInitDone(%s%S) -- err=errPARAMETER_INVALID, bad access flags - fACCESS_CHECK_WRITE set without fACCESS_WRITE", PR_TRACE_PREPARE_STR_CP(m_ppath,codepage) ));
	}
	else {
		
		tDWORD access;
		tDWORD creation;
		tDWORD share;
		tDWORD flags;
		tDWORD omode;
		
		if ( run_under_nt ) {
			
			// pre parse file name if necessary
			if ( m_preparse_name && (m_full_name!=m_ppath) && fGetFullPathName ) {
				bool   the_same = true;
				tDWORD full_len = 0;
				tCHAR* full = 0;
				tCHAR* path = 0;
				tCHAR* name = 0;
				if ( PR_FAIL(error=parse_name(&the_same,full,full_len,path,name)) ) {
					if ( full )
						heapFree( full );
					return error;
				}
				if ( !the_same ) {
					if ( m_full_name )
						heapFree( m_full_name );
					m_full_name_buff_len = full_len;
					m_full_name          = (tCHAR*)full;
					m_ppath              = (tCHAR*)path;
					m_pname              = (tCHAR*)name;

					check_slashesW();
				}
			}
			//else
			//	check_slashesW();
		}
		//else
		//	check_slashes();

		tBOOL restore_access;
		tBOOL restore_modified;

		if ( m_user_wants_to_restore_file_times || ARE_BITS_SET(m_access_mode,fACCESS_RESTORE_FILE_TIMES) )
			restore_access = restore_modified = cTRUE;
		else {
			if ( m_user_wants_to_restore_access_time || (m_access_mode & fACCESS_RESTORE_ACCESS_TIME) )
				restore_access = cTRUE;
			else
				restore_access = cFALSE;
			if ( m_user_wants_to_restore_modified_time || (m_access_mode & fACCESS_RESTORE_MODIFIED_TIME) )
				restore_modified = cTRUE;
			else
				restore_modified = cFALSE;
		}

		omode = m_open_mode & (mOMODE_CREATION | mOMODE_SHARE | fOMODE_TEMPORARY);
		access = 0;
		share = 0;
		creation = 0;
		flags = FILE_FLAG_RANDOM_ACCESS;

		if ( omode & fOMODE_TEMPORARY )
			flags |= FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE;
		
		if ( omode & fOMODE_FOLDER_AS_IO ) {
			omode &= ~fOMODE_FOLDER_AS_IO;
			creation = OPEN_EXISTING;
			flags |= FILE_FLAG_BACKUP_SEMANTICS;
		}
		else if ( propIsOperational() )
			creation = OPEN_EXISTING;
		else if ( omode & fOMODE_TRUNCATE ) {
			switch( omode & (mOMODE_CREATION & (~fOMODE_TRUNCATE)) ) {
				case fOMODE_OPEN_IF_EXIST                              : creation = TRUNCATE_EXISTING; break;
				case fOMODE_CREATE_IF_NOT_EXIST                        : creation = CREATE_NEW;        break;
				case fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST : creation = CREATE_ALWAYS;     break;
			}
		}
		else {
			switch( omode & mOMODE_CREATION ) {
				case fOMODE_OPEN_IF_EXIST                              : creation = OPEN_EXISTING;     break;
				case fOMODE_CREATE_IF_NOT_EXIST                        : creation = CREATE_NEW;        break;
				case fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST : creation = OPEN_ALWAYS;       break;
			}
		}
		
		if ( m_access_mode & fACCESS_READ )
			access |= GENERIC_READ;
		if ( m_access_mode & fACCESS_WRITE )
			access |= GENERIC_WRITE;

		if ( /*run_under_nt &&*/ cSavedTimes::check(0,0,restore_access,restore_modified,m_access_mode) )
			access |= FILE_WRITE_ATTRIBUTES;

		if ( !(omode & fOMODE_SHARE_DENY_READ) )
			share |= FILE_SHARE_READ;
		if ( !(omode & fOMODE_SHARE_DENY_WRITE) )
			share |= FILE_SHARE_WRITE;
		if ( run_under_nt && !(omode & fOMODE_SHARE_DENY_DELETE) )
			share |= FILE_SHARE_DELETE;

		if ( fACCESS_FILE_FLAG_NO_BUFFERING == (m_access_mode & fACCESS_FILE_FLAG_NO_BUFFERING) )
			flags |= FILE_FLAG_NO_BUFFERING;
		
		// вредно это
		//if ( m_del_on_close )
		//  flags |= FILE_FLAG_DELETE_ON_CLOSE;
		
		if ( 0 == (m_access_mode & fACCESS_NONE) ) {

			tDWORD err_size = sizeof(tERROR);
			tBOOL  try_reopen_by_short_name = !run_under_nt;
			tBOOL  try_reopen_after_reparse_name = !!run_under_nt; //cTRUE;
			tBOOL  try_reopen_with_ro_attrib = (m_ignore_ro_attrib && (access & GENERIC_WRITE));
			tBOOL  try_reopen_with_exec_attrib = (m_open_mode & fOMODE_OPEN_IF_EXECUTABLE);
			tBOOL  try_reopen_with_elevated_token = ((tBYTE)GetVersion() >= 6);

			tWCHAR path[MAX_PATH];
			tDWORD path_len = sizeof(path), len;
			tPTR   p_path = path;
			tPTR   current_file_name = m_full_name;
			tUINT  attempt;
			tDWORD old_full_len = m_full_name_buff_len;
			tCHAR* old_full     = m_full_name;
			tCHAR* old_path     = m_ppath;
			tCHAR* old_name     = m_pname;
			cAutoObj<cToken> hToken;

			for( attempt=0; 1; ++attempt ) { // open file and reopen it on some conditions

				m_handle = fCreateFile( current_file_name, access, share, NULL, creation, flags, NULL );
				if ( m_handle != INVALID_HANDLE_VALUE ) {
					m_win32err = ERROR_SUCCESS;
					error = errOK;
					#if defined( _PRAGUE_TRACE_)
						if ( attempt )
							PR_TRACE(( this, prtIMPORTANT, "nfio\tCreateFile reopen succeeded - attempt(%d) - \"%s%S\" (access=0x%08x, share=0x%08x, creation=0x%08x, flags=0x%08x)", attempt, PR_TRACE_PREPARE_STR_CP(current_file_name,codepage), access, share, creation, flags ));
						else
							PR_TRACE(( this, prtNOTIFY, "nfio\tCreateFile succeeded - \"%s%S\" (access=0x%08x, share=0x%08x, creation=0x%08x, flags=0x%08x)", PR_TRACE_PREPARE_STR_CP(current_file_name,codepage), access, share, creation, flags ));
					#endif
					break;
				}

				m_win32err = GetLastError();
				error = _ConvertWin32Err( m_win32err );

				PR_TRACE(( this, prtIMPORTANT, "nfio\tCreateFile failed (0x%08x) - \"%s%S\"", m_win32err, PR_TRACE_PREPARE_STR_CP(current_file_name,codepage) ));
				
				if ( ((m_win32err == ERROR_FILE_NOT_FOUND) || (m_win32err == ERROR_PATH_NOT_FOUND)) ) {
					PR_TRACE(( this, prtNOTIFY, "nfio\tfile not found - try to reopen" ));
					if ( try_reopen_after_reparse_name ) {
						try_reopen_after_reparse_name = cFALSE;
						if ( !run_under_nt )
							PR_TRACE(( this, prtNOTIFY, "nfio\tunder 9x we do not reparse name" ));
						else if ( !m_preparse_name )
							PR_TRACE(( this, prtNOTIFY, "nfio\tfile name already preparsed" ));
						else if ( !fGetFullPathName )
							PR_TRACE(( this, prtNOTIFY, "nfio\thas no fGetFullPathName pointer (to get long name)" ));
						else if ( m_full_name == m_ppath )
							PR_TRACE(( this, prtNOTIFY, "nfio\tfile name seems to be the same as parsed" ));
						else {
							bool   the_same = false;
							tDWORD full_len = 0;
							tCHAR* full = 0;
							tCHAR* path = 0;
							tCHAR* name = 0;
							tERROR err  = parse_name( &the_same, full, full_len, path, name );
							if ( PR_SUCC(err) ) {
								if ( the_same ) {
									if ( full )
										heapFree( full );
									PR_TRACE(( this, prtNOTIFY, "nfio\tlong name is the same" ));
								}
								else {
									try_reopen_by_short_name = cFALSE;
									m_full_name_buff_len = full_len;
									m_full_name          = (tCHAR*)full;
									m_ppath              = (tCHAR*)path;
									m_pname              = (tCHAR*)name;
									current_file_name    = m_full_name;

									check_slashesW();
									PR_TRACE(( this, prtNOTIFY, "nfio\ttry to reopen by long name \"%S\"", m_full_name ));
									continue; // try again
								}
							}
							else {
								if ( full )
									heapFree( full );
								PR_TRACE(( this, prtNOTIFY, "nfio\terror (%terr) getting long name", err ));
							}
						}
					}
					if ( try_reopen_by_short_name ) {
						try_reopen_by_short_name = cFALSE;
						PR_TRACE(( this, prtNOTIFY, "nfio\ttry to reopen by short name" ));
						error = GetShortFullName( &len, plOBJECT_NATIVE_SHORT_FULLNAME, (tCHAR*)p_path, path_len );
						if ( error == errBUFFER_TOO_SMALL ) {
							error = heapAlloc( (tPTR*)&p_path, len );
							if ( PR_SUCC(error) )
								error = GetShortFullName( &len, plOBJECT_NATIVE_SHORT_FULLNAME, (tCHAR*)p_path, path_len=len );
						}
						if ( PR_SUCC(error) ) {
							current_file_name = p_path;
							PR_TRACE(( this, prtNOTIFY, "nfio\tshort name - \"%s%S\"", PR_TRACE_PREPARE_STR_CP(current_file_name,codepage) ));
							continue;
						}
						else
							PR_TRACE(( this, prtIMPORTANT, "nfio\tget short name failed %terr(%d,0x%08x) - \"%s%S\"", error, m_win32err, m_win32err, PR_TRACE_PREPARE_STR_CP(m_full_name,codepage) ));
					}
					break;
				}

				//if ( m_win32err == ERROR_SHARING_VIOLATION )
				//	break;

				if ( m_win32err == ERROR_ACCESS_DENIED ) {

					if ( run_under_nt && (access & FILE_WRITE_ATTRIBUTES) && !(access & GENERIC_WRITE) ) {
						restore_access = restore_modified = false;
						access &= ~FILE_WRITE_ATTRIBUTES;
						PR_TRACE(( this, prtIMPORTANT, "nfio\tFailed to open with FILE_WRITE_ATTRIBUTES flag set %terr(%d,0x%08x) - \"%s%S\" - will try to reopen", error, m_win32err, m_win32err, PR_TRACE_PREPARE_STR_CP(m_full_name,codepage) ));
						continue;
					}

					if ( try_reopen_with_elevated_token ) {
						try_reopen_with_elevated_token = cFALSE;

						if( PR_SUCC(prepareElevatedToken(hToken, (tCHAR*&)p_path, path_len, (tCHAR*&)current_file_name)) )
							continue;
					}

					if ( try_reopen_with_ro_attrib ) {
						try_reopen_with_ro_attrib = cFALSE;

						if ( !IS_ATTR(m_open_attr) )
							m_open_attr = fGetFileAttributes( m_full_name );
						
						// Windows Server 2003 and Windows XP/2000:  If CREATE_ALWAYS and FILE_ATTRIBUTE_NORMAL are specified,
						//   CreateFile fails and sets the last error to ERROR_ACCESS_DENIED if the file exists and has the
						//   FILE_ATTRIBUTE_HIDDEN or FILE_ATTRIBUTE_SYSTEM attribute. To avoid the error, specify the same
						//   attributes as the existing file.
						if ( IS_ATTR(m_open_attr) && (m_open_attr & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) ) {
							if ( !fSetFileAttributes(m_full_name,m_open_attr&(~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN))) ) {	
								m_win32err = GetLastError();
								error = _ConvertWin32Err( m_win32err );
								sysSendMsg( pmc_IO, pm_IO_SET_ATTRIBUTES_FAILED, 0, &error, &err_size );
								break;
							}
							else {
								if( !(m_open_mode & fOMODE_CREATE_ALWAYS) )           // do not restore attributes if file recreated
									m_has_to_restore_attribs = cTRUE;
								continue;	// try again
							}
						}
					}

					if ( try_reopen_with_exec_attrib ) {
						try_reopen_with_exec_attrib = cFALSE;
						if ( fGetFileSecurity && PR_SUCC(prepareReopenExecutable()) )
							continue;
					}
				}

				break;
			} // while fCreateFile

			if( !!hToken )
				hToken->Impersonate(eitAsUser);

			if ( old_full && (old_full != m_full_name) ) {
				if ( PR_SUCC(error) )
					heapFree( old_full );
				else {
					heapFree( m_full_name );
					m_full_name_buff_len = old_full_len;
					m_full_name          = old_full;
					m_ppath              = old_path;
					m_pname              = old_name;
				}
			}

			if ( p_path != path )
				heapFree( p_path );

			if( PR_FAIL(error) && run_under_nt && g_bFSDRVImportresolved && FSDirectRead ) {
				if ( (m_win32err == ERROR_SHARING_VIOLATION) && (m_access_mode & fACCESS_FORCE_READ) && !(m_access_mode & fACCESS_WRITE) ) {
					PR_TRACE(( this, prtIMPORTANT, "nfio\tIO::ObjectInitDone -- failed, forcing open..."));
					m_handle = fCreateFile( m_full_name, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
					if ( INVALID_HANDLE_VALUE != m_handle ) {
						// check read
						char buff;
						tDWORD sizetmp = sizeof(buff);
						tERROR errtmp = FSDirectRead(m_handle, 0, &buff, sizetmp, &sizetmp);

						if (PR_FAIL(errtmp))
						{
							CloseHandle(m_handle);
							m_handle = INVALID_HANDLE_VALUE;
							error = errLOCKED;
							m_win32err = ERROR_SHARING_VIOLATION;
						}
						else
						{
							m_win32err = 0;
							error = errOK;

							PR_TRACE(( this, prtIMPORTANT, "nfio\tIO::ObjectInitDone -- force read access granted (open existing, random access)"));
						}
					}
				}
			}

			if ( PR_FAIL(error) && run_under_nt && g_bFSDRVImportresolved && FSDirectOpen ) {
				if ( m_direct_mode && (m_access_mode & fACCESS_WRITE) ) {
					error = errLOCKED;
					m_win32err = ERROR_SHARING_VIOLATION;
					PR_TRACE(( this, prtIMPORTANT, "nfio\tIO::ObjectInitDone -- force read and write: object locked!"));
				}
				else if ( (m_access_mode & fACCESS_FORCE_OPEN) && !(m_access_mode & fACCESS_WRITE) ) {
					PR_TRACE(( this, prtIMPORTANT, "nfio\tIO::ObjectInitDone -- force read open request"));
					tERROR errtmp = FSDirectOpen( (tPTR*)&m_handle, (tWCHAR*)m_full_name );
					if ( PR_SUCC(errtmp) ) {
						error = errOK;
						m_win32err = 0;
						PR_TRACE(( this, prtIMPORTANT, "nfio\tIO::ObjectInitDone -- force read open granted"));
						m_direct_mode = cTRUE;
					}
				}
			}

			tUINT pm_code;
			if( PR_FAIL(error) ) {
				if ( ((m_win32err == ERROR_ACCESS_DENIED) || (m_win32err == ERROR_PATH_NOT_FOUND)) && PR_SUCC(is_folder(m_full_name,0)) )
					error = errIO_FOLDER_NOT_FILE;
				

				/*#if defined( _PRAGUE_TRACE_ )
					tPTR pshort_a;
					tPTR pshort_w;
					tDWORD empty = 0;
					if ( current_file_name == m_full_name )
						pshort_a = pshort_w = &empty;
					else if ( run_under_nt ) {
						pshort_a = &empty;
						pshort_w = current_file_name;
					}
					else {
						pshort_a = current_file_name;
						pshort_w = &empty;
					}

					PR_TRACE(( this, prtERROR, "nfio\tIO::ObjectInitDone %terr, \"%s%S\"(%s%S), WinErr:%d(0x%08x), access(%x), share(%d), creation(%d), flags(%x)", error, PR_TRACE_PREPARE_STR_CP(m_full_name,codepage), pshort_a, pshort_w, m_win32err, m_win32err, access, share, creation, flags ));
				#endif*/

				if ( m_reopen_mode )
					pm_code = pm_IO_REOPEN_FAILED;
				else
					pm_code = pm_IO_CREATE_FAILED;
				sysSendMsg( pmc_IO, pm_code, 0, &error, &err_size );
			}
			else {
				BY_HANDLE_FILE_INFORMATION fi = {0};

				m_written_something = false;
				if ( GetFileInformationByHandle(m_handle,&fi) ) {
					m_size.l = fi.nFileSizeLow;
					m_size.h = fi.nFileSizeHigh;
					m_saved_times.init( m_handle, restore_access, restore_modified, m_access_mode, &fi.ftLastAccessTime, &fi.ftLastWriteTime );
				}
				else {
					m_size.l = GetFileSize( m_handle, (DWORD*)&m_size.h );
					if ( m_size.l == INVALID_FILE_SIZE )
						m_size.v = 0;
					m_saved_times.init( m_handle, restore_access, restore_modified, m_access_mode, 0, 0 );
				}

				//if ( run_under_nt && m_saved_times )
				//	m_saved_times.restore();

				//if ( m_user_wants_to_restore_file_times && run_under_nt ) {
				//	minus_one.dwLowDateTime = minus_one.dwHighDateTime = -1;
				//	SetFileTime( m_handle, 0, 0, &minus_one );
				//}
			
				if ( m_has_to_restore_attribs  ) {
					// restore attributes cleared on IO open because of m_ignore_ro_attrib
					if ( fSetFileAttributes(m_full_name,m_open_attr) )
						m_has_to_restore_attribs = cFALSE;
					else {	
						tERROR local = _ConvertWin32Err( m_win32err = GetLastError() );
						sysSendMsg( pmc_IO, pm_IO_RESORE_ATTRIBUTES_FAILED, 0, &local, &err_size );
					}
				}

				if ( ARE_BITS_SET(m_access_mode,(fACCESS_WRITE|fACCESS_CHECK_WRITE)) ) {
					tULONG size;
					tBYTE  storage;

					//if ( !m_user_wants_to_restore_file_times && run_under_nt ) {
					//	minus_one.dwLowDateTime = minus_one.dwHighDateTime = -1;
					//	SetFileTime( m_handle, 0, 0, &minus_one );
					//}

					if ( fi.nFileSizeLow || fi.nFileSizeHigh ) { // file is not empty
						size = 0;
						if ( !ReadFile(m_handle,&storage,sizeof(storage),&size,0) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (-1 == SetFilePointer(m_handle,0,0,SEEK_SET)) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && !WriteFile(m_handle,&storage,sizeof(storage),&size,0) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (-1 == SetFilePointer(m_handle,0,0,SEEK_SET)) )
							m_win32err = GetLastError();
					}
					else { // file is empty
						tBYTE get_back;
						size = (ULONG)&storage;
						storage = (tBYTE)((size >> 16) & 0xff);
						if ( !WriteFile(m_handle,&storage,sizeof(storage),&size,0) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (-1 == SetFilePointer(m_handle,0,0,SEEK_SET)) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && !ReadFile(m_handle,&get_back,sizeof(get_back),&size,0) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (-1 == SetFilePointer(m_handle,0,0,SEEK_SET)) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (-1 == SetEndOfFile(m_handle)) )
							m_win32err = GetLastError();
						if ( (m_win32err == ERROR_SUCCESS) && (storage != get_back) )
							m_win32err = ERROR_ACCESS_DENIED;
					}

					//if ( !m_user_wants_to_restore_file_times && run_under_nt )
					//	SetFileTime( m_handle, 0, 0, &minus_one /*fi.ftLastWriteTime*/ );

					if ( m_win32err != ERROR_SUCCESS ) {
						error = _ConvertWin32Err( m_win32err );
						if ( m_reopen_mode )
							pm_code = pm_IO_REOPEN_FAILED;
						else
							pm_code = pm_IO_CREATE_FAILED;
						sysSendMsg( pmc_IO, pm_code, 0, &error, &err_size );
					}
				}
			}
		}
	}

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR Win32IOData::ObjectClose() {

	tERROR error = errOK;
	tBOOL  has_been_deleted;
	tDWORD err_size;
	PR_TRACE_A0( this, "Enter IO::ObjectClose method" );
	
	has_been_deleted = cFALSE;
	m_win32err = 0;
	
	if ( m_handle ) {
		if ( !m_del_on_close && !m_written_something && m_saved_times )
			m_saved_times.restore();
		
		if ( CloseHandle(m_handle) ) {
			PR_TRACE(( this, prtNOT_IMPORTANT, "nfio\t\"%S\" has been closed", m_full_name ));
		}
		else {
			m_win32err = GetLastError();
			PR_TRACE(( this, prtNOT_IMPORTANT, "nfio\t\"%S\" has not been closed, win err is - %d(0x%08x)", m_full_name, m_win32err, m_win32err ));
		}
		m_handle = 0;
	}

	if ( m_del_on_close ) {
		tDWORD msg_id;
		tDWORD del_err;
		#ifdef _PRAGUE_TRACE_
			tSTRING obj_type = m_del_on_close ? "File" : "Temporary file";
		#endif

		err_size = sizeof(del_err);

		if ( m_open_mode & fOMODE_TEMPORARY ) {
			has_been_deleted = cTRUE;
			del_err     = errOK;
			msg_id      = pm_IO_DELETE_ON_CLOSE_SUCCEED;
			if ( run_under_nt )
				PR_TRACE(( this, prtSPAM, "nfio\t%s \"%S\" has been deleted on close", obj_type, m_full_name ));
			else
				PR_TRACE(( this, prtSPAM, "nfio\t%s \"%s\" has been deleted on close", obj_type, m_full_name ));
		}
		else {

			if ( IS_ATTR(m_open_attr) && (m_open_attr & FILE_ATTRIBUTE_READONLY) && !fSetFileAttributes(m_full_name,m_open_attr&(~FILE_ATTRIBUTE_READONLY)) ) {
				tDWORD err_size = sizeof(tERROR);
				tERROR local;
				m_win32err = GetLastError();
				local = _ConvertWin32Err( m_win32err );
				sysSendMsg( pmc_IO, pm_IO_SET_ATTRIBUTES_FAILED, 0, &local, &err_size );
			}

			if ( !fDeleteFile ) {
				m_win32err = ERROR_PROC_NOT_FOUND;
				del_err     = errMETHOD_NOT_FOUND;
				msg_id      = pm_IO_DELETE_ON_CLOSE_FAILED;
				error       = errIO_DELETE_ON_CLOSE_FAILED;
				PR_TRACE(( this, prtERROR, "nfio\tDeleteFile export pointer cannot be imported from the Windows kernel" ));
			}
			else {
				tPTR *cur_path = (tPTR*)m_full_name, *path = NULL;
				tBOOL  try_delete_with_elevated_token = ((tBYTE)GetVersion() >= 6);
				cAutoObj<cToken> hToken;

				for(int i =0; i < 15; i++) {
					if ( fDeleteFile(cur_path) ) {
						has_been_deleted = cTRUE;
						m_win32err = ERROR_SUCCESS;
						del_err     = errOK;
						msg_id      = pm_IO_DELETE_ON_CLOSE_SUCCEED;
						if ( run_under_nt )
							PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%S\" has been deleted on close", obj_type, m_full_name ));
						else
							PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%s\" has been deleted on close", obj_type, m_full_name ));
						break;
					}
					else {

						// save error
						m_win32err = GetLastError();
						del_err    = _ConvertWin32Err( m_win32err );
						msg_id     = pm_IO_DELETE_ON_CLOSE_FAILED;
						error      = errIO_DELETE_ON_CLOSE_FAILED;

						if( m_win32err == ERROR_ACCESS_DENIED ) {

							if ( try_delete_with_elevated_token ) {
								try_delete_with_elevated_token = cFALSE;

								tDWORD len = 0;
								prepareElevatedToken(hToken, (tCHAR*&)path, len, (tCHAR*&)cur_path);
							}

							if( i )
							{
								Sleep(100);
								continue;
							}
						}

						if ( !IS_ATTR(m_open_attr) )
							m_open_attr = fGetFileAttributes( cur_path );

						if ( IS_ATTR(m_open_attr) && (m_open_attr & FILE_ATTRIBUTE_READONLY) && fSetFileAttributes( m_full_name,m_open_attr&(~FILE_ATTRIBUTE_READONLY)) && fDeleteFile(cur_path) ) {
							has_been_deleted = cTRUE;
							m_win32err  = ERROR_SUCCESS;
							del_err     = errOK;
							msg_id      = pm_IO_DELETE_ON_CLOSE_SUCCEED;
							error       = errOK;
							if ( run_under_nt )
								PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%S\" has been deleted on close", obj_type, cur_path ));
							else
								PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%s\" has been deleted on close", obj_type, cur_path ));
						}
						else {
							if ( run_under_nt )
								PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%S\" cannot be deleted on close. Win error is -- %d", obj_type, cur_path, m_win32err ));
							else
								PR_TRACE(( this, prtNOTIFY, "nfio\t%s \"%s\" cannot be deleted on close. Win error is -- %d", obj_type, cur_path, m_win32err ));
						}

						if( m_win32err != ERROR_ACCESS_DENIED )
							break;
					}
				}

				if( !!hToken )
					hToken->Impersonate(eitAsUser);

				if( path )
					heapFree(path);
			}
		}

		sysSendMsg( pmc_IO, msg_id, 0, &del_err, &err_size );
	}
	else if ( (m_has_to_restore_attribs || m_user_wants_to_restore_attribs) && IS_ATTR(m_open_attr) && !fSetFileAttributes(m_full_name,m_open_attr) ) {
		tERROR local;
		err_size = sizeof(local);
		local = _ConvertWin32Err( m_win32err = GetLastError() );
		sysSendMsg( pmc_IO, pm_IO_RESORE_ATTRIBUTES_FAILED, 0, &local, &err_size );
	}

	if( m_secure_to_restore ) {
		fSetFileSecurity(m_full_name, DACL_SECURITY_INFORMATION, m_secure_to_restore);

		heapFree( m_secure_to_restore );
		m_secure_to_restore = NULL;
	}

	if ( m_full_name ) {
		heapFree( m_full_name );
		m_full_name = 0;
	}

	PR_TRACE_A1( this, "Leave IO::ObjectClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR Win32IOData::GetName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT  len;
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgOBJECT_NAME" );
	
	if ( run_under_nt )
		len = _toui32(UNIS * ( 1 + (m_pname ? wcslen((tWCHAR*)m_pname) : 0) ));
	else
		len = _toui32(MBS * ( 1 + (m_pname ? strlen(m_pname) : 0 ) ));
	
	if ( buffer ) {
		if ( len > size ) {
			error = errBUFFER_TOO_SMALL;
			len = 0;
		}
		else if ( m_pname )
			memcpy( buffer, m_pname, len );
		else if ( run_under_nt )
			*UNI(buffer) = (tWCHAR)0;
		else
			*buffer = 0;
	}
	*out_size = len;
	
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetName )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR Win32IOData::SetName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method ObjectClose" );
	
	try {
		
		if ( !buffer )
			size = MAX_PATH;
		
		else {
			tCHAR* p0 = FindSeparator( buffer, &size );
			if ( !p0 )                                  // there is only name in the buffer
				error = iSetName( buffer, &size );
			else if ( *p0 == 0 ) {                      // there is only path in the buffer
				size = _toui32(p0 - buffer);
				error = iSetPath( buffer, &size );
			}
			else                                        // there are path and name in the buffer
				error = iSetFull( buffer, &size, _toui32(p0-buffer) );
			if ( PR_SUCC(error) && (prop != pgOBJECT_NAME) )
				propInitialized( pgOBJECT_NAME, cTRUE );
		}
	}
	catch (...) {
		size = 0;
		error = errUNEXPECTED;
	}
	*out_size = size;
	
	PR_TRACE_A2( this, "Leave *SET* multyproperty method ObjectClose, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// ---
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR pr_call Win32IOData::SetNameW( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A1( this, "Enter IO::SetNameW(%S)\"", buffer ? buffer : L"NULL" );

	try {

		if ( !buffer ) 
			size = UNICODE_MAX_PATH;

		else if ( size % UNIS ) {
			size = 0;
			error = errPARAMETER_INVALID;
		}

		else {
			tWCHAR* p0 = FindSeparatorW( buffer, &size );
			if ( !p0 )                                               // there is only name in the buffer
				error = iSetNameW( buffer, &size );
			else if ( *p0 == 0 ) {                                   // there is only path in the buffer
				size = _toui32(MB(p0) - MB(buffer));
				error = iSetPathW( buffer, &size );
			}
			else                                                     // there are path and name in the buffer
				error = iSetFullW( buffer, &size, _toui32(MB(p0)-MB(buffer)) );
			if ( PR_SUCC(error) )
				propInitialized( pgOBJECT_NAME, cTRUE );
		}
	}
	catch( ... ) {
		size = 0;
		error = errUNEXPECTED;
	}

	*out_size = size;
	PR_TRACE_A3( this, "Leave IO::PROP_SetNameW method, ret hUINT = %u, error = 0x%08X, name = \"%s\"", size, error, m_pname );
	return error;
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetPath )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR Win32IOData::GetPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT  copy_len;
	tUINT  full_len;
	tBOOL  pfx = cFALSE;
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgOBJECT_PATH" );
	
	copy_len = _toui32(m_pname - m_ppath);
	if ( run_under_nt )
		full_len = calc_full_path_len( UNI(m_full_name), UNI(m_ppath), copy_len, &pfx );
	else
		full_len = copy_len + MBS;
	
	if ( !buffer )
		*out_size = full_len;
	
	else if ( size < full_len ) {
		error = errBUFFER_TOO_SMALL;
		*out_size = 0;
	}
	
	else {
		if ( pfx ) {
			*UNI(buffer) = *(UNI(buffer)+1) = PATH_DELIMETER_WIDECHAR;
			buffer += 2 * UNIS;
		}
		memcpy( buffer, m_ppath, copy_len );
		if ( run_under_nt )
			*UNI(MB(buffer)+copy_len) = 0;
		else
			*(MB(buffer)+copy_len) = 0;
		*out_size = full_len;
	}
	
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetPath )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR Win32IOData::SetPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectClose for property pgOBJECT_PATH" );
	
	try {
		if ( !buffer )
			size = MAX_PATH;
		else {
			if ( !size && *buffer )
				size = lstrlen(buffer) * MBS;
			error = iSetPath( buffer, &size );
			if ( PR_SUCC(error) )
				propInitialized( pgOBJECT_NAME, cTRUE );
		}
	}
	catch( ... ) {
		size = 0;
		error = errUNEXPECTED;
	}
	*out_size = size;
	
	PR_TRACE_A2( this, "Leave *SET* method ObjectClose for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR Win32IOData::SetPathW( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectClose for property pgOBJECT_PATH" );
	
	try {
		if ( !buffer ) 
			size = UNICODE_MAX_PATH;
		else if ( size % UNIS ) {
			size = 0;
			error = errPARAMETER_INVALID;
		}
		else {
			if ( !size && *buffer )
				size = _toui32(wcslen(buffer) * UNIS);
			error = iSetPathW( buffer, &size );
			if ( PR_SUCC(error) )
				propInitialized( pgOBJECT_NAME, cTRUE );
		}
	}
	catch( ... ) {
		size = 0;
		error = errUNEXPECTED;
	}
	*out_size = size;
	
	PR_TRACE_A2( this, "Leave *SET* method ObjectClose for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetFullName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR Win32IOData::GetFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT  copy_len;
	tUINT  full_len;
	tBOOL  pfx = cFALSE;
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgOBJECT_FULL_NAME" );

	if ( run_under_nt ) {
		copy_len = _toui32(UNIS * wcslen((tWCHAR*)m_ppath));
		full_len = calc_full_path_len( UNI(m_full_name), UNI(m_ppath), copy_len, &pfx );
	}
	else {
		copy_len = _toui32(MBS * strlen(m_ppath));
		full_len = copy_len + MBS;
	}
	
	if ( !buffer )
		*out_size = full_len;
	
	else if ( full_len > size ) {
		error = errBUFFER_TOO_SMALL;
		*out_size = 0;
	}
	else {
		if ( pfx ) {
			*UNI(buffer) = *(UNI(buffer)+1) = PATH_DELIMETER_WIDECHAR;
			buffer += 2 * UNIS;
		}
		memcpy( buffer, m_ppath, copy_len );
		if ( run_under_nt )
			*UNI(MB(buffer)+copy_len) = 0;
		else
			*(MB(buffer)+copy_len) = 0;
		*out_size = full_len;
	}
	
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_access )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR Win32IOData::set_access( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tBOOL  done_performed;
	PR_TRACE_A0( this, "Enter *SET* method ObjectClose for property pgOBJECT_ACCESS_MODE" );
	
	done_performed = propIsOperational();
	if ( done_performed && (m_access_mode & fACCESS_NO_CHANGE_MODE) ) {
		size = 0;
		error = errACCESS_DENIED;
	}
	
	else if ( !buffer != !size ) {
		size = 0;
		error = errPARAMETER_INVALID;
	}
	
	else if ( buffer ) {
		
		tDWORD mode = *(tDWORD*)buffer;
		
		size = sizeof(m_access_mode);
		
		if ( (mode & (mACCESS|fACCESS_NONE)) == 0 )
			mode |= fACCESS_READ;
		
		error = errOK;
		if ( m_access_mode == mode )
			;
		
		else if ( !done_performed )
			m_access_mode = mode;
		
//		else if ( (m_access_mode & fACCESS_RW) == (mode & fACCESS_RW) )
//			m_access_mode = mode;
		
		else {
			HANDLE old_one  = m_handle;
			tDWORD old_mode = m_access_mode;

			cSavedTimes restore_times = m_saved_times;

			if ( m_handle )
				m_handle = 0;
			m_access_mode = mode;
			m_reopen_mode = cTRUE;

			#if defined( _DEBUG )
				if ( m_access_mode & fACCESS_WRITE ) {
					m_access_mode = m_access_mode;
				}
			#endif

			if ( PR_FAIL(error=ObjectInitDone()) ) {
				if ( m_handle ) {
					if ( m_saved_times )
						m_saved_times.restore();
					CloseHandle( m_handle );
				}
				m_access_mode = old_mode;
				m_handle = old_one;
				size = 0;
			}
			else {
				m_reopen_mode = cFALSE;
				if ( old_one ) {
					if ( restore_times )
						restore_times.restore();
					CloseHandle( old_one );
				}
			}
			
			m_cp.v = 0;
		}
	}
	else
		size = sizeof(m_access_mode);
	
	*out_size = size;
	
	PR_TRACE_A2( this, "Leave *SET* method ObjectClose for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_REOPEN_DATA )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR Win32IOData::get_REOPEN_DATA( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	PR_TRACE_VARS;
	tERROR  error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method get_REOPEN_DATA for property pgOBJECT_REOPEN_DATA" );

	tCHAR  sPrefix[MAX_PATH];
	tDWORD dwOffset = 0;
	tDWORD dwPrefix = GetSubstPrefix(sPrefix, &dwOffset, cFALSE);
	if( dwPrefix )
	{
		if( buffer )
			buffer += dwOffset;
		size = (size > dwOffset) ? size - dwOffset : 0;
	}

	tBYTE cExInfo = 0;
	if( m_short_name_4_reopen )
		cExInfo = 1;
	else if( m_access_mode & fACCESS_NONE )
		cExInfo = 2;

	if ( cExInfo ) {
		if ( buffer )
		{
			*buffer = cExInfo-1;
			if( m_short_name_4_reopen )
				error = GetShortFullName( out_size, plOBJECT_NATIVE_SHORT_FULLNAME, buffer+1, size-1 );
			else
				error = GetFullName( out_size, pgOBJECT_FULL_NAME, buffer+1, size-1 );
		}
		else
		{
			if( m_short_name_4_reopen )
				error = GetShortFullName( out_size, plOBJECT_NATIVE_SHORT_FULLNAME, buffer, size );
			else
				error = GetFullName( out_size, pgOBJECT_FULL_NAME, buffer, size );
		}
		if ( PR_SUCC(error) )
			++(*out_size);
	}
	else
		error = GetLongFullName( out_size, plOBJECT_NATIVE_LONG_FULLNAME, buffer, size );

	if( dwPrefix && (PR_SUCC(error) || error == errBUFFER_TOO_SMALL) )
	{
		*out_size += dwOffset;
		if( buffer )
		{
			buffer -= dwOffset;
			if( PR_SUCC(error) )
				memcpy(buffer, sPrefix, dwPrefix);
		}
	}

	#if defined( _PRAGUE_TRACE_ )
		const tCHAR* type;
		if ( buffer || PR_FAIL(error) ) {
			if ( !buffer ) {
				if ( run_under_nt )
					buffer = MB(L"");
				else
					buffer = "";
				type = m_short_name_4_reopen ? "short" : "long";
			}
			else if ( cExInfo ) {
				++buffer;
				type = "short";
			}
			else
				type = "long";

			PR_TRACE(( this, PR_TRACE_COND_LEVEL(error,prtNOTIFY,prtERROR), "nfio\tget %s reopen data %terr - \"%s%S\"", type, error, PR_TRACE_PREPARE_STR_CP(buffer,codepage) ));
		}
	#endif

	PR_TRACE_A2( this, "Leave *GET* method get_REOPEN_DATA for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_REOPEN_DATA )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR Win32IOData::set_REOPEN_DATA( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	PR_TRACE_VARS;
	tERROR  error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method set_REOPEN_DATA for property pgOBJECT_REOPEN_DATA" );

	PR_TRACE(( this, prtIMPORTANT, "nfio\tset reopen data \"%s%S\"", PR_TRACE_PREPARE_STR_CP(buffer,codepage) ));

	if ( buffer ) {
		if ( *buffer <= 1 ) {
			if( !*buffer )
				m_short_name_4_reopen = cTRUE;
			else
				m_access_mode = fACCESS_NONE;

			++buffer;
			--size;
		}
		else
			m_short_name_4_reopen = cFALSE;

		if ( run_under_nt )
			error = SetNameW( out_size, pgOBJECT_NAME, UNI(buffer), size );
		else
			error = SetName( out_size, pgOBJECT_NAME, MB(buffer), size );

		if ( PR_SUCC(error) )
			propInitialized( pgOBJECT_NAME, cTRUE );
	}
	else
		error = errPARAMETER_INVALID;

	#if defined( _PRAGUE_TRACE_ )
		if ( PR_SUCC(error) ) {
			tWCHAR path[MAX_PATH];
			tCHAR* par = "ext param";
			if ( PR_SUCC(propGetStr(0,pgOBJECT_FULL_NAME,path,sizeof(path),codepage)) ) {
				par = "int name";
				buffer = MB(path);
			}
			PR_TRACE(( this, prtNOTIFY, "nfio\tset reopen data succeeded (%s) \"%s%S\"", par, PR_TRACE_PREPARE_STR_CP(buffer,codepage) ));
		}
		else
			PR_TRACE(( this, prtERROR, "nfio\tset reopen data failed %terr", error ));
	#endif

	PR_TRACE_A2( this, "Leave *GET* method set_REOPEN_DATA for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, check_exclusive )
// Interface "IO". Method "Get" for property(s):
//  -- EXCLUSIVE_ACCESS
tERROR Win32IOData::check_exclusive( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR e;
	HANDLE f;
	tBOOL  v;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property plEXCLUSIVE_ACCESS" );
	
	m_win32err = errOK;
	if ( m_handle ) {
		*out_size = 0;
		v = cFALSE;
		e = errOBJECT_BAD_INTERNAL_STATE;
	}
	else if ( INVALID_HANDLE_VALUE == (f=fCreateFile(m_full_name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0)) ) {
		e = errOK;
		m_win32err = GetLastError();
		*out_size = sizeof(tBOOL);
		if ( m_win32err == ERROR_SHARING_VIOLATION )
			v = cFALSE;
		else
			v = cTRUE;
	}
	else {
		CloseHandle( f );
		*out_size = sizeof(tBOOL);
		v = cTRUE;
	}
	
	if ( buffer )
		*(tBOOL*)buffer = v;
	
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property plEXCLUSIVE_ACCESS, ret tDWORD = %u(size), %terr", *out_size, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, IsRemote )
// Interface "IO". Method "Get" for property(s):
//  -- IS_REMOTE
tERROR Win32IOData::IsRemote( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	tBOOL remote = cTRUE;
	PR_TRACE_A0( this, "Enter *GET* method IsRemote for property plIS_REMOTE" );

	if( !m_full_name )
		error = errOBJECT_NOT_INITIALIZED;
	else {
		tBOOL local;
		if( run_under_nt )
			local = *UNI(m_ppath) && *UNI(m_ppath + UNIS) == L':';
		else
			local = *MB(m_ppath) && *MB(m_ppath + MBS) == ':';

		if( local ) {
			tCHAR root[3*UNIS];
			tDWORD type;
			memset(root, 0, sizeof(root));
			memcpy(root, m_ppath, 2*(run_under_nt ? UNIS : MBS));
			type = fGetDriveType(root);
			remote = (type == DRIVE_REMOTE || type == DRIVE_RAMDISK);
		}

		*out_size = sizeof(tBOOL);
		if( buffer )
			*(tBOOL*)buffer = remote;
	}

	PR_TRACE_A2( this, "Leave *GET* method IsRemote for property plIS_REMOTE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

tDWORD Win32IOData::GetSubstPrefix( tCHAR* buff, tDWORD* dwOffset, tBOOL bUNC) {
	tDWORD dwSize = 0;
	if( run_under_nt )
	{
		tWCHAR pDriveName[MAX_PATH];
		memcpy(pDriveName, m_ppath, 2*UNIS);
		pDriveName[2] = 0;

		if( ::QueryDosDeviceW(pDriveName, (tWCHAR*)buff, MAX_PATH) )
			if( !wcsncmp((tWCHAR*)buff, L"\\??\\", 4) )
			{
				_wcslwr_s((tWCHAR*)buff, wcslen((tWCHAR*)buff) + 1);
				dwSize = _toui32(UNIS*(wcslen((tWCHAR*)buff)));
				if( !bUNC )
				{
					if( !_wcsnicmp((tWCHAR*)buff + 4, L"UNC\\", 4) )
					{
						dwSize -= 6*UNIS;
						memmove(buff + UNIS, buff + 7*UNIS, dwSize);
					}
					else
					{
						dwSize -= 4*UNIS;
						memmove(buff, buff + 4*UNIS, dwSize);
					}
				}
				*dwOffset = dwSize - 2*UNIS;
			}
	}
	else
	{
		tCHAR pDriveName[MAX_PATH];
		memcpy(pDriveName, m_ppath, 2*MBS);
		pDriveName[2] = 0;

		if( ::QueryDosDeviceA(pDriveName, buff, MAX_PATH) )
			if( pDriveName[0] != buff[0] )
			{
				_strlwr_s(buff, strlen(buff) + 1);
				dwSize = _toui32(MBS*(strlen(buff)));
				if( dwSize == 3 )
					dwSize--;
				*dwOffset = dwSize - 2*MBS;
			}
	}
	return dwSize;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetNativePath )
// Interface "IO". Method "Get" for property(s):
//  -- NATIVE_PATH
tERROR Win32IOData::GetNativePath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetNativePath for property plNATIVE_PATH" );

	if( !m_full_name )
		return errOBJECT_NOT_INITIALIZED;

	tCHAR  sPrefix[MAX_PATH];
	tDWORD dwOffset = 0;
	tDWORD dwPrefix = GetSubstPrefix(sPrefix, &dwOffset, cTRUE);
	tDWORD dwSize = m_full_name_buff_len;

	if( dwPrefix )
	{
		dwSize = run_under_nt ? (wcslen((tWCHAR*)m_ppath) + 1)*UNIS : (strlen(m_ppath) + 1)*MBS;
		dwSize += dwOffset;
	}

	if( buffer ){
		if( size < dwSize )
			error = errBUFFER_TOO_SMALL;
		else
		{
			if( dwPrefix )
			{
				memcpy(buffer+dwOffset, m_ppath, dwSize-dwOffset);
				memcpy(buffer, sPrefix, dwPrefix);
			}
			else
				memcpy(buffer, m_full_name, dwSize);
		}
	}

	*out_size = dwSize;

	PR_TRACE_A2( this, "Leave *GET* method GetNativePath for property plNATIVE_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetShortFullName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NATIVE_SHORT_FULLNAME
tERROR Win32IOData::GetShortFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method GetShortFullName for property plOBJECT_NATIVE_SHORT_FULLNAME" );

	*out_size = 0;
	m_win32err = ERROR_SUCCESS;

	if ( !fGetShortPathName )
		error = errNOT_SUPPORTED;

	else {
		tDWORD  lsize = 0;
		tWCHAR  lbuff[MAX_PATH];
		tPTR    plbuff = lbuff;

		error = GetFullName( &lsize, pgOBJECT_FULL_NAME, MB(lbuff), sizeof(lbuff) );
		if ( error == errBUFFER_TOO_SMALL ) {
			error = heapAlloc( &plbuff, lsize );
			if ( PR_SUCC(error) )
				error = GetFullName( &lsize, pgOBJECT_FULL_NAME, MB(plbuff), lsize );
		}


		if ( PR_SUCC(error) ) {
			if ( run_under_nt )
				size /= sizeof(tWCHAR);
			*out_size = fGetShortPathName( plbuff, buffer, size );
			if ( !*out_size ) {
				m_win32err = GetLastError();
				error = _ConvertWin32Err( m_win32err );
			}
			else {
				if ( size && (*out_size > size) )
					error = errBUFFER_TOO_SMALL;
				++*out_size;
				if ( run_under_nt )
					*out_size *= sizeof(tWCHAR);
			}
		}

		if ( plbuff != lbuff )
			heapFree( plbuff );
	}

	PR_TRACE_A2( this, "Leave *GET* method GetShortFullName for property plOBJECT_NATIVE_SHORT_FULLNAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetShortName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NATIVE_SHORT_NAME
tERROR Win32IOData::GetShortName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR   error = errOK;
	tWCHAR   short_path[MAX_PATH];
	tWCHAR*  p_short_path = short_path;
	PR_TRACE_A0( this, "Enter *GET* method GetShortName for property plOBJECT_NATIVE_SHORT_NAME" );

	*out_size = 0;
	if ( !fGetShortPathName )
		error = errNOT_SUPPORTED;
	else {
		tDWORD ret = 0;
		error = GetShortFullName( &ret, plOBJECT_NATIVE_SHORT_FULLNAME, MB(short_path), sizeof(short_path) );
		if ( error == errBUFFER_TOO_SMALL ) {
			error = heapAlloc( (tPTR*)&p_short_path, ret );
			if ( PR_SUCC(error) )
				error = GetShortFullName( &ret, plOBJECT_NATIVE_SHORT_FULLNAME, MB(p_short_path), ret );
		}
		if ( PR_SUCC(error) ) {
			tPTR  ptr;
			tUINT len;
			if ( run_under_nt ) {
				ptr = wcsrchr( UNI(p_short_path), PATH_DELIMETER_WIDECHAR );
				if ( ptr )
					ptr = UNI(ptr) + 1;
				else
					ptr = p_short_path;
				len = _toui32(UNIS * (1 + wcslen(UNI(ptr))));
			}
			else {
				ptr = strrchr( MB(p_short_path), PATH_DELIMETER_CHAR );
				if ( ptr )
					ptr = MB(ptr) + 1;
				else
					ptr = p_short_path;
				len = MBS * (1 + lstrlen(MB(ptr)));
			}
			if ( buffer ) {
				tUINT cpy_size = len;
				if ( size < cpy_size ) {
					error = errBUFFER_TOO_SMALL;
					if ( run_under_nt ) {
						cpy_size = size - UNIS;
						*UNI( MB(ptr)+size-UNIS ) = 0;
					}
					else {
						cpy_size = size - MBS;
						*(MB(ptr)+size-MBS) = 0;
					}
				}
				memcpy( buffer, ptr, cpy_size );
			}
			*out_size = len;
		}
	}

	if ( p_short_path != short_path )
		heapFree( p_short_path );

	PR_TRACE_A2( this, "Leave *GET* method GetShortName for property plOBJECT_NATIVE_SHORT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetLongFullName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NATIVE_LONG_FULLNAME
tERROR Win32IOData::GetLongFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	
	tERROR  error = errOK;
	tDWORD  fname_size;
	tWCHAR  fname[MAX_PATH];
	tWCHAR* pfname = fname;

	PR_TRACE_A0( this, "Enter *GET* method GetLongFullName for property plOBJECT_NATIVE_LONG_FULLNAME" );

	error = GetFullName( &fname_size, pgOBJECT_FULL_NAME, MB(fname), sizeof(fname) );
	if ( error == errBUFFER_TOO_SMALL ) {
		error = heapAlloc( (tPTR*)&pfname, fname_size );
		if ( PR_SUCC(error) )
			error = GetFullName( &size, pgOBJECT_FULL_NAME, MB(pfname), size );
	}

	if ( PR_FAIL(error) )
		*out_size = 0;

	else if ( fGetLongPathName ) {
		m_win32err = ERROR_SUCCESS;
		if ( run_under_nt )
			size /= UNIS;
		*out_size = fGetLongPathName( pfname, buffer, size );
		if ( *out_size ) {
			if ( run_under_nt ) {
				if ( buffer && (size >= *out_size) ) {
					*out_size += 1; // if size of buffer is enough to store full path GetLongPathName returns len WITHOUT terminating zero
					if ( !g_case_sensitive )
						_wcslwr_s( UNI(buffer), size );
				}
				*out_size *= UNIS;
			}
			else if ( buffer && (size >= *out_size) ) {
				*out_size += 1; // if size of buffer is enough to store full path GetLongPathName returns len WITHOUT terminating zero
				if ( !g_case_sensitive )
					_strlwr_s( MB(buffer), size );
			}
		}
		else {
			m_win32err = GetLastError();
			error = _ConvertWin32Err( m_win32err );
		}
	}

	else {

		tWCHAR  lname[MAX_PATH];
		tWCHAR* plname = lname;
		tUINT   plname_size = sizeof(lname);
		
		if ( run_under_nt )
			error = _to_long_w( *this, pfname, &plname, &plname_size, out_size, &m_win32err );
		else
			error = _to_long_a( MB(pfname), MB(plname), sizeof(lname), out_size, &m_win32err );
		
		if ( PR_SUCC(error) ) {
			if ( !buffer )
				;
			else if ( *out_size > size )
				error = errBUFFER_TOO_SMALL;
			else
				memcpy( buffer, plname, *out_size );
		}
		
		if ( plname && (plname != lname) )
			heapFree( plname );
	}

	if ( pfname && (pfname != fname) )
		heapFree( pfname );


	if( PR_FAIL(error) )
		PR_TRACE(( this, prtERROR, "nfio\tIO:GetLongFullName error %x", error));
	else if( buffer )
		PR_TRACE(( this, prtNOTIFY, "nfio\tIO:GetLongFullName succeded %s", buffer));

	PR_TRACE_A2( this, "Leave *GET* method GetLongFullName for property plOBJECT_NATIVE_LONG_FULLNAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetLongName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NATIVE_LONG_NAME
tERROR Win32IOData::GetLongName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {

	tERROR  error = errOK;
	tDWORD  lsize;
	tWCHAR  lbuff[MAX_PATH];
	tPTR    plbuff = lbuff;
	PR_TRACE_A0( this, "Enter *GET* method GetLongName for property plOBJECT_NATIVE_LONG_NAME" );

	error = GetLongFullName( &lsize, plOBJECT_NATIVE_LONG_FULLNAME, MB(lbuff), sizeof(lbuff) );
	if ( error == errBUFFER_TOO_SMALL ) {
		error = heapAlloc( &plbuff, lsize );
		if ( PR_SUCC(error) )
			error = GetLongFullName( &lsize, plOBJECT_NATIVE_LONG_FULLNAME, MB(plbuff), lsize );
	}

	if ( PR_SUCC(error) ) {
		tPTR slash;
		if ( run_under_nt ) {
			slash = wcsrchr( UNI(plbuff), L'\\' );
			if ( !slash )
				slash = wcsrchr( UNI(plbuff), L'/' );
			if ( slash )
				slash = UNI(slash) + 1;
		}
		else {
			slash = strrchr( MB(plbuff), '\\' );
			if ( !slash )
				slash = strrchr( MB(plbuff), L'/' );
			if ( slash )
				slash = MB(slash) + 1;
		}
		if ( !slash )
			slash = plbuff;
		tUINT len = _toui32(run_under_nt ? UNIS*(1+wcslen(UNI(slash))) : MBS*(1+lstrlen(MB(slash))));
		if ( buffer ) {
			if ( size < len ) {
				*out_size = 0;
				error = errBUFFER_TOO_SMALL;
			}
			else {
				*out_size = len;
				memcpy( buffer, slash, len );
			}
		}
		else
			*out_size = len;
	}

	if ( plbuff && (plbuff != lbuff) )
		heapFree( plbuff );

	PR_TRACE_A2( this, "Leave *GET* method GetLongName for property plOBJECT_NATIVE_LONG_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, _get_security )
// Interface "IO". Method "Get" for property(s):
//  -- SECURITY_ATTRIBUTES
tERROR Win32IOData::_get_security( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method _get_security for property plSECURITY_ATTRIBUTES" );

	*out_size = 0;
	m_win32err = ERROR_SUCCESS;
	if ( fGetFileSecurity ) {
		if ( !buffer != !size ) {
			buffer = 0;
			size = 0;
		}
		if ( !fGetFileSecurity(m_full_name,DACL_SECURITY_INFORMATION,buffer,size,(DWORD*)out_size) ) {
			m_win32err = GetLastError();
			error = PR_WIN32_TO_PRAGUE_ERR(m_win32err);//_ConvertWin32Err( m_win32err );
		}
		else if (!(*out_size))
			error = errNOT_FOUND;
	}
	else
		error = errNOT_SUPPORTED;

	PR_TRACE_A2( this, "Leave *GET* method _get_security for property plSECURITY_ATTRIBUTES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, _set_security )
// Interface "IO". Method "Set" for property(s):
//  -- SECURITY_ATTRIBUTES
tERROR Win32IOData::_set_security( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method _set_security for property plSECURITY_ATTRIBUTES" );

	*out_size = 0;
	m_win32err = ERROR_SUCCESS;
	if ( fSetFileSecurity ) {
		if ( !buffer || !size )
			error = errPARAMETER_INVALID;
		else if ( fSetFileSecurity(m_full_name,DACL_SECURITY_INFORMATION,buffer) )
			*out_size = size;
		else {
			m_win32err = GetLastError();
			error = PR_WIN32_TO_PRAGUE_ERR(m_win32err);//_ConvertWin32Err( m_win32err );
		}
	}
	else
		error = errNOT_SUPPORTED;

	PR_TRACE_A2( this, "Leave *SET* method _set_security for property plSECURITY_ATTRIBUTES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetFileAttributes )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR Win32IOData::GetFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	tDWORD dwAttributes;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgOBJECT_ATTRIBUTES" );

	m_win32err = 0;
	*out_size = 0;
	if (m_full_name == NULL)
		return errOBJECT_NOT_INITIALIZED;

	dwAttributes = fGetFileAttributes(m_full_name);
	if ( dwAttributes == (tDWORD)-1 )	{
		m_win32err = GetLastError();
		error = _ConvertWin32Err(m_win32err);
	}
	else {
		*out_size = sizeof(tDWORD);
		if (size < sizeof(tDWORD) && buffer == NULL)
			error = errBUFFER_TOO_SMALL;
		else if ( buffer )
			*(tDWORD*)buffer = dwAttributes;
	}

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetFileAttributes )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR Win32IOData::SetFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectClose for property pgOBJECT_ATTRIBUTES" );

	m_win32err = 0;
	*out_size = 0;
	if (size != sizeof(tDWORD) || buffer==NULL)
		return errPARAMETER_INVALID;
	if (m_full_name == NULL)
		return errOBJECT_NOT_INITIALIZED;
	if (!(m_access_mode & fACCESS_WRITE))
		return errACCESS_DENIED;
	
	if ( !fSetFileAttributes(m_full_name, *(tDWORD*)buffer) )
	{
		m_win32err = GetLastError();
		error = _ConvertWin32Err(m_win32err);
	}
	
	PR_TRACE_A2( this, "Leave *SET* method ObjectClose for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, iGetFileTime )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR Win32IOData::iGetFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
    FILETIME ft_create, ft_access, ft_write;
	FILETIME* pft;
	tDATETIME dt;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method iGetFileTime" );
	
	m_win32err = 0;
	*out_size = sizeof(tDATETIME);
	if (m_handle == 0)
		return errOBJECT_NOT_INITIALIZED;
	if (size == 0 && buffer==NULL)
		return errOK;
	if (size < sizeof(tDATETIME) && buffer!=NULL)
		return errBUFFER_TOO_SMALL;
	if (!DTSet)
		return errINTERFACE_NOT_LOADED;

	if( !GetFileTime(m_handle,&ft_create,&ft_access,&ft_write) ) {
		m_win32err = GetLastError();
		error = _ConvertWin32Err( m_win32err );
	}
	else {
		switch ( prop ) {
		case pgOBJECT_CREATION_TIME:
			pft = &ft_create;
			break;
		case pgOBJECT_LAST_WRITE_TIME:
			pft = &ft_write;
			break;
		case pgOBJECT_LAST_ACCESS_TIME:
			pft = &ft_access;
			break;
		default:
			*out_size = 0;
			error = errPROPERTY_NOT_IMPLEMENTED;
			break;
		}
		if (PR_SUCC(error)) {
			SYSTEMTIME st;
			if (!FileTimeToSystemTime(pft, &st)) {
				m_win32err = GetLastError();
				error = _ConvertWin32Err( m_win32err );
			}
			else
				error = DTSet(&dt, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds*1000000);
			if ( PR_SUCC(error) ) {
				*out_size = sizeof(tDATETIME);
				if ( (size < sizeof(tDATETIME)) && (buffer == NULL) )
					error = errBUFFER_TOO_SMALL;
				else {
					if (buffer)
						memcpy(buffer, dt, sizeof(tDATETIME));
				}
			}
		}
	}

	PR_TRACE_A2( this, "Leave *GET* multyproperty method iGetFileTime, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, iSetFileTime )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR Win32IOData::iSetFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method iSetFileTime" );
	
	m_win32err = 0;
	*out_size = 0;
	if (m_handle == 0)
		return errOBJECT_NOT_INITIALIZED;
	if (size < sizeof(tDATETIME) || buffer==NULL)
		return errPARAMETER_INVALID;
	if (!DTGet)
		return errINTERFACE_NOT_LOADED;

	FILETIME ft;
	FILETIME *pcreate, *paccess, *pwrite;

	switch ( prop ) {
		case pgOBJECT_CREATION_TIME:
			pcreate = &ft;
			paccess = 0;
			pwrite = 0;
			break;
		case pgOBJECT_LAST_WRITE_TIME:
			pcreate = 0;
			paccess = &ft;
			pwrite = 0;
			break;
		case pgOBJECT_LAST_ACCESS_TIME:
			pcreate = 0;
			paccess = 0;
			pwrite = &ft;
			break;
		default:
			error = errPROPERTY_NOT_IMPLEMENTED;
			break;
	}

	if ( PR_SUCC(error) ) {
		ft = cWinDateTime( cDateTime((tDATETIME*)buffer) );
		if ( PR_SUCC(error) && !SetFileTime(m_handle,pcreate,paccess,pwrite) ) {
			m_win32err = GetLastError();
			error = _ConvertWin32Err( m_win32err );
		}
	}

//	FILETIME ft_create, ft_access, ft_write;
//	FILETIME* pft;
//	tDWORD dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwNanoseconds;
//
//	if( !GetFileTime(m_handle,&ft_create,&ft_access,&ft_write) ) {
//		m_win32err = GetLastError();
//		error = _ConvertWin32Err( m_win32err );
//	}
//
//	if ( PR_SUCC(error) ) {
//		switch ( prop ) {
//			case pgOBJECT_CREATION_TIME:
//				pft = &ft_create;
//				break;
//			case pgOBJECT_LAST_WRITE_TIME:
//				pft = &ft_write;
//				break;
//			case pgOBJECT_LAST_ACCESS_TIME:
//				pft = &ft_access;
//				break;
//			default:
//				error = errPROPERTY_NOT_IMPLEMENTED;
//				break;
//		}
//
//		if ( PR_SUCC(error) )
//			error = DTGet((tDATETIME*)buffer, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMinute, &dwSecond, &dwNanoseconds);
//
//		if ( PR_SUCC(error) ) {
//			SYSTEMTIME st;
//			st.wYear   = (tWORD)dwYear;
//			st.wMonth  = (tWORD)dwMonth;
//			st.wDay    = (tWORD)dwDay;
//			st.wHour   = (tWORD)dwHour;
//			st.wMinute = (tWORD)dwMinute;
//			st.wSecond = (tWORD)dwSecond;
//			st.wMilliseconds = (tWORD)dwNanoseconds / 1000000;
//			if ( !SystemTimeToFileTime(&st,pft) ) {
//				m_win32err = GetLastError();
//				error = _ConvertWin32Err( m_win32err );
//			}
//		}
//
//		if ( PR_SUCC(error) && !SetFileTime(m_handle,&ft_create,&ft_access,&ft_write) ) {
//			m_win32err = GetLastError();
//			error = _ConvertWin32Err( m_win32err );
//		}
//	}

	PR_TRACE_A2( this, "Leave *SET* multyproperty method iSetFileTime, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR Win32IOData::SeekRead( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	tERROR error;
	PR_TRACE_A0( this, "Enter IO::SeekRead method" );
	
	tDWORD sizetmp = size;

	m_win32err = 0;

	if( !size )
	{
		if ( result )
			*result = size;
		return errOK;
	}
	
	if ( !buffer || !size ) {
		size = 0;
		error = errPARAMETER_INVALID;
	}
	else if ( offset > cMAX_LONGLONG ) {
		size = 0;
		error = errOBJECT_SEEK;
	}
	else if ( !m_handle ) {
		size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
	}
	else if ( m_control_size_on_read && (((tLONGLONG)offset) > m_size.v) ) {	// !!! changed to ">=" 16.08.04, changed to ">" 26.11.04
		size = 0;
		error = errOUT_OF_OBJECT;
	}
	else {
		// default return value
		if ( m_control_size_on_read && ((((tLONGLONG)offset) + size) > m_size.v) ) { // !!! changed to ">=" 16.08.04, changed to ">" 26.11.04
			if( !(m_access_mode & fACCESS_FILE_FLAG_NO_BUFFERING) )
				size = (tDWORD)(m_size.v - offset);
			error = errEOF;
		}
		else
			error = errOK;
		
		tQWord& off = *(tQWord*)&offset;
		
		if (
			( (m_cp.v != off.v) || (m_open_mode & fOMODE_DONT_CACHE_POS) ) &&
			(INVALID_FILE_SIZE == (off.l=SetFilePointer(m_handle,off.l,&off.h,FILE_BEGIN))) &&
			(NO_ERROR != (m_win32err=GetLastError()))
		) {
			size = 0;
			error = errOBJECT_SEEK;
		}
		
		else if ( !ReadFile(m_handle,buffer,size,(DWORD*)&size,NULL) ) {
			m_win32err = GetLastError();
			error = errOBJECT_READ;
			m_cp.h = 0;
			m_cp.l = SetFilePointer( m_handle, 0, &m_cp.h, FILE_CURRENT );
		}
		else {
			// if ( !size ) error = errOUT_OF_OBJECT; // Windows tells us: we have reached EOF, commented 26.11.04
			m_cp.v = off.v + size;
		}
	}

	if ( PR_SUCC(error) ) {
		// nothing todo
	}
	else if( error != errOUT_OF_OBJECT )
	{
		if ( (m_access_mode & fACCESS_FORCE_READ && FSDirectRead)  && (error != errOBJECT_SEEK) )
		{
			tERROR errtmp = FSDirectRead(m_handle, offset, buffer, sizetmp, &sizetmp);
			
			if (PR_SUCC(errtmp))
			{
				PR_TRACE(( this, prtIMPORTANT, "nfio\tIO:force read ok. read size %d", errtmp, sizetmp));

				m_win32err = ERROR_SUCCESS;
				error = errOK;
		
				tQWord& off = *(tQWord*)&offset;
				m_cp.v = off.v + sizetmp;

				size = sizetmp;
			}
			else
			{
				PR_TRACE(( this, prtIMPORTANT, "nfio\tIO:force read result %terr (returned %terr), size %d",
					errtmp, error, sizetmp));

				if (errLOCKED == errtmp)
				{
					error = errtmp;
					m_win32err = ERROR_LOCK_VIOLATION;
				}
			}
		}
	}

	if ( result )
		*result = size;

	
	PR_TRACE_A2( this, "Leave IO::SeekRead method, ret tDWORD = %u, %terr", size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR Win32IOData::SeekWrite( tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	tLONGLONG rest;
	tDWORD    written = 0;
	tERROR    error = errOK;
	PR_TRACE_A0( this, "Enter IO::SeekWrite method" );
	
	m_win32err = 0;
	
	if (m_open_mode & fOMODE_DONT_CACHE_POS)
		GetSize((tQWORD*) &m_size.v, IO_SIZE_TYPE_EXPLICIT);
	
	if ( !buffer )
		error = errPARAMETER_INVALID;
	else if ( offset > cMAX_LONGLONG )
		error = errOBJECT_SEEK;
	else if ( !m_handle )
		error = errOBJECT_BAD_INTERNAL_STATE;
	else if ( !(m_access_mode & fACCESS_WRITE) )
		error = errOBJECT_READ_ONLY;
	else if ( 0 == (rest=((tLONGLONG)m_size.v)-((tLONGLONG)offset)) ) { // calc rest of the file
		if ( (m_access_mode & fACCESS_NO_EXTEND) && size )                  // now we are exactly at the end of the file
			error = errOBJECT_RESIZE;                                       // this is an error
		else if ( m_cp.v == ((tLONGLONG)offset) )
			;
		else if ( (INVALID_FILE_SIZE == SetFilePointer(m_handle,0,0,FILE_END)) && (NO_ERROR != (m_win32err=GetLastError())) )
			error = errOBJECT_SEEK;
	}
	else if ( rest < 0 ) {  // have to extend
		if ( m_access_mode & (fACCESS_NO_EXTEND|fACCESS_NO_EXTEND_RAND) )
			error = errOBJECT_RESIZE;
		else if( !(m_access_mode & fACCESS_FILE_FLAG_NO_BUFFERING) )
		{
			if ( PR_SUCC(error=FillUpTo(&offset)) )                      // file size already updated to new value and cp set to eof
				rest = 0;
		}
		else if ( (INVALID_FILE_SIZE == SetFilePointer(m_handle,((tQWord*)&offset)->l,&((tQWord*)&offset)->h,FILE_BEGIN)) && (NO_ERROR != (m_win32err=GetLastError())) )
			error = errOBJECT_SEEK;
		else
		{
			m_size.v = m_cp.v = offset;
			rest = 0;
		}
	}
	else if ( (m_cp.v == (tLONGLONG)offset)  && (!(m_open_mode & fOMODE_DONT_CACHE_POS))) // check current position
		;
	else if ( (INVALID_FILE_SIZE == SetFilePointer(m_handle,((tQWord*)&offset)->l,&((tQWord*)&offset)->h,FILE_BEGIN)) && (NO_ERROR != (m_win32err=GetLastError())) )
		error = errOBJECT_SEEK;
	
	if ( PR_SUCC(error) ) {
		
		if ( !size )
			;
		else {
			tLONGLONG ext;
			if ( size <= rest )
				ext = 0;
			else if ( m_access_mode & fACCESS_NO_EXTEND ) {
				size = (tDWORD)rest;
				ext = 0;
			}
			else
				ext = ((tLONGLONG)size) - rest;
			
			if ( WriteFile(m_handle,buffer,size,(DWORD*)&written,NULL) ) {
				error = errOK;
				if ( ext ) {
					if ( size != written )
						ext += written - size;
					m_size.v += ext;
				}
				if ( written )
					m_written_something = true;
				// 30.01.2006
				//m_cp.h = 0;
				//m_cp.l = SetFilePointer( m_handle, 0, &m_cp.h, FILE_CURRENT );
				m_cp.v = offset + written;
				g_dwTotalWrites += written;
			}
			else {
				m_win32err = GetLastError();
				error = _ConvertWin32Err( m_win32err );
				if ( error == errUNEXPECTED )
					error = errOBJECT_WRITE;
			}
		}
	}
	
	if ( result )
		*result = written;
	
	PR_TRACE_A2( this, "Leave IO::SeekWrite method, ret tDWORD = %u, %terr", written, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR Win32IOData::GetSize( tQWORD* result, IO_SIZE_TYPE type ) {
	PR_TRACE_A0( this, "Enter IO::GetSize method" );
	if ( result ) {
		if (m_handle && (m_open_mode & fOMODE_DONT_CACHE_POS))
			m_size.l = GetFileSize( m_handle, (DWORD*)&m_size.h );
		*result = m_size.v;
	}
	m_win32err = 0;
	PR_TRACE_A1( this, "Leave IO::GetSize method, ret tQWORD = %I64u, errOK", m_size.v );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR Win32IOData::SetSize( tQWORD new_size ) {
	tERROR error;
	PR_TRACE_A0( this, "Enter IO::SetSize method" );
	
	m_win32err = 0;
	if ( !m_handle )
		error = errOBJECT_BAD_INTERNAL_STATE;
	else if ( !(m_access_mode & fACCESS_WRITE) )
		error = errOBJECT_READ_ONLY;
	else if ( new_size > cMAX_LONGLONG )
		error = errOBJECT_SEEK;
	else if ( ((tLONGLONG)new_size) == m_size.v ) // size the same
		error = errOK;
	else if ( ((tLONGLONG)new_size) > m_size.v ) {
		if ( m_access_mode & fACCESS_NO_EXTEND )
			error = errOBJECT_INCOMPATIBLE;
		else
			error = FillUpTo( &new_size );
	}
	else if ( m_access_mode & fACCESS_NO_TRUNCATE ) // truncate case
		error = errOBJECT_INCOMPATIBLE;
	else {
		tQWord tmp;
		tmp.h = ((tQWord*)&new_size)->h;
		tmp.l = SetFilePointer( m_handle, ((tQWord*)&new_size)->l, &tmp.h, FILE_BEGIN );
		
		if ( (tmp.l == INVALID_FILE_SIZE) && (NO_ERROR != (m_win32err=GetLastError())) )
			error = errOBJECT_SEEK;
		else {
			m_cp.v = tmp.v;
			if ( SetEndOfFile(m_handle) ) {
				m_size.v = tmp.v;
				error = errOK;
			}
			else {
				m_win32err = GetLastError();
				error = errOBJECT_RESIZE;
			}
		}
	}
	
	PR_TRACE_A1( this, "Leave IO::SetSize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR Win32IOData::Flush() {
	tERROR error;
	PR_TRACE_A0( this, "Enter IO::Flush method" );
	
	m_win32err = 0;
	if ( !m_handle || FlushFileBuffers(m_handle) )
		error = errOK;
	else {
		m_win32err = GetLastError();
		error = _ConvertWin32Err( m_win32err );
		if ( error == errUNEXPECTED )
			error = errIO_BUFFERING;
	}
	
	PR_TRACE_A1( this, "Leave IO::Flush method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
//const tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
// AVP Prague stamp end



extern "C" tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR Win32IOData::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "win32 input/output interface", 29 )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, Win32IOData, m_origin, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(GetName), FN_CUST(SetName) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(GetPath), FN_CUST(SetPath) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, FN_CUST(GetFullName), FN_CUST(SetName) )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, Win32IOData, m_open_mode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, Win32IOData, m_access_mode, cPROP_BUFFER_READ, NULL, FN_CUST(set_access) )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, Win32IOData, m_del_on_close, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, Win32IOData, m_os, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, FN_CUST(get_REOPEN_DATA), FN_CUST(set_REOPEN_DATA) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(GetFileAttributes), FN_CUST(SetFileAttributes) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(iGetFileTime), FN_CUST(iSetFileTime) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(iGetFileTime), FN_CUST(iSetFileTime) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(iGetFileTime), FN_CUST(iSetFileTime) )
	mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
	mpLOCAL_PROPERTY_BUF( pgNATIVE_ERR, Win32IOData, m_win32err, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( plNATIVE_HANDLE, Win32IOData, m_handle, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( plEXCLUSIVE_ACCESS, FN_CUST(check_exclusive), NULL )
	mpLOCAL_PROPERTY_BUF( plRESTORE_ATTRIBUTES, Win32IOData, m_user_wants_to_restore_attribs, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( plIGNORE_RO_ATTRIBUTE, Win32IOData, m_ignore_ro_attrib, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( plCONTROL_SIZE_ON_READ, Win32IOData, m_control_size_on_read, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plEXTENDED_NAME_PROCESSING, Win32IOData, m_extended_name_processing, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( plIS_REMOTE, FN_CUST(IsRemote), NULL )
	mpLOCAL_PROPERTY_FN( plNATIVE_PATH, FN_CUST(GetNativePath), NULL )
	mpLOCAL_PROPERTY_BUF( plPREPARSE_NAME, Win32IOData, m_preparse_name, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plSAVE_FILE_TIME , Win32IOData, m_user_wants_to_restore_file_times, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( pgOBJECT_NATIVE_PATH, FN_CUST(GetFullName), NULL )
	mpLOCAL_PROPERTY_FN( plOBJECT_NATIVE_SHORT_FULLNAME, FN_CUST(GetShortFullName), NULL )
	mpLOCAL_PROPERTY_FN( plOBJECT_NATIVE_SHORT_NAME, FN_CUST(GetShortName), NULL )
	mpLOCAL_PROPERTY_FN( plOBJECT_NATIVE_LONG_FULLNAME, FN_CUST(GetLongFullName), NULL )
	mpLOCAL_PROPERTY_FN( plOBJECT_NATIVE_LONG_NAME, FN_CUST(GetLongName), NULL )
	mpLOCAL_PROPERTY_BUF( plRESTORE_ACCESS_TIME, Win32IOData, m_user_wants_to_restore_access_time, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plRESTORE_MODIFIED_TIME, Win32IOData, m_user_wants_to_restore_modified_time, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( plSHORT_NAME_4_REOPEN, Win32IOData, m_short_name_4_reopen, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( plSECURITY_ATTRIBUTES, FN_CUST(_get_security), FN_CUST(_set_security) )
mpPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



	#if defined(DEF_NULL)
		#define NULL 0L
	#endif

	if ( run_under_nt ) {
		tDWORD i;
		typedef tDATA trec[6];
		trec* rec = (trec*)IO_PropTable;
		
		for( i=0; i<mPROPERTY_TABLE_SIZE(IO_PropTable); i++,rec++ ) {
			switch( (*rec)[0] ) {
			case pgOBJECT_NAME :
			case pgOBJECT_FULL_NAME :
			//case pgOBJECT_REOPEN_DATA :
				(*rec)[5] = FN_CUST(SetNameW);
				break;
			case pgOBJECT_PATH :
				(*rec)[5] = FN_CUST(SetPathW);
				break;
			}
		}
	}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IO)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IO)
	mEXTERNAL_METHOD(IO, SeekRead)
	mEXTERNAL_METHOD(IO, SeekWrite)
	mEXTERNAL_METHOD(IO, GetSize)
	mEXTERNAL_METHOD(IO, SetSize)
	mEXTERNAL_METHOD(IO, Flush)
mEXTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter IO::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_WIN32_NFIO,                         // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(Win32IOData)-sizeof(cObjImpl),   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"nfio\tIO(IID_IO) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave IO::Register method, ret %terr", error );
// AVP Prague stamp end



	_call_test();

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call IO_Register( hROOT root ) { return Win32IOData::Register(root); }
// AVP Prague stamp end



extern "C" tERROR pr_call cIO_Register( hROOT root ) { return Win32IOData::Register(root); }

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_HASH
// AVP Prague stamp end



// ---
tERROR pr_call Win32IOData::FillUpTo( tQWORD* offset ) {
	
	tPTR   ptr;
	tERROR error;
	tDWORD bsize;
	tDWORD written;
	tQWord tmp;
	tBYTE  def_buff[100];
	
	m_win32err = 0;
	
	tmp.h = 0;
	tmp.l = SetFilePointer( m_handle, 0, &tmp.h, FILE_END ); // set file pointer to the end of the file get it value
	
	if ( (tmp.l == INVALID_FILE_SIZE) && (NO_ERROR != (m_win32err=GetLastError())) )
		return errOBJECT_SEEK;
	
	// *offset > end of file -- MUST be checked before function call
	tmp.v = ((tLONGLONG)*offset) - tmp.v; // have to write to the file
	
	if ( (tmp.v <= (10 * sizeof(def_buff))) || PR_FAIL(heapAlloc(&ptr,bsize=DEF_BUFF_SIZE)) ) {
		ptr = def_buff;
		bsize = sizeof(def_buff);
	}
	memset( ptr, 0, bsize );
	
	error = errOK;
	while( tmp.v > 0 ) {
		if ( tmp.v < bsize )
			written = (tDWORD)tmp.v;
		else
			written = bsize;
		
		if ( WriteFile(m_handle,ptr,written,(DWORD*)&written,0) )
			tmp.v -= written;
		else {
			m_win32err = GetLastError();
			error = errOBJECT_WRITE;
			break;
		}
	}
	
	if ( ptr != def_buff )
		heapFree( ptr );
	
	m_cp.h = 0;
	m_cp.l = SetFilePointer( m_handle, 0, &m_cp.h, FILE_CURRENT );
	m_size.v = m_cp.v;
	
	return error;
}



// ---
tERROR pr_call Win32IOData::iSetName( tSTRING buffer, tDWORD* size ) {
	tUINT path_len;
	tUINT new_size;
	
	while( *size && (0 == *(buffer + *size - 1)) )
		(*size)--;
	
	if ( !*size ) {
		if ( m_pname )
			*(m_pname) = 0;
		return errOK;
	}
	
	cERROR  err;
	cChBuff	buff(this);
	
	if ( extended_name_processing() && PR_FAIL(err=processString(buffer,*size,buff)) )
		return err;
	
	path_len = _toui32(m_pname - m_ppath);
	new_size = path_len + *size + MBS;
	
	if ( new_size > MAX_PATH )
		return errBUFFER_TOO_SMALL;
	
	if ( new_size > m_full_name_buff_len ) {
		err = heapRealloc( (tPTR*)&m_full_name, m_full_name, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		m_ppath = m_full_name;
		m_pname = m_ppath + path_len;
		m_full_name_buff_len = new_size;
	}
	
	memcpy( m_pname, buffer, *size );
	*(MB(m_pname) + *size) = 0;

	check_slashes();
	return errOK;
}




// ---
tERROR pr_call Win32IOData::iSetPath( tSTRING buffer, tDWORD* size ) {
	tUINT name_len;
	tUINT new_size;
	tCHAR symb;
	tUINT add_slash;
	
	while( *size && *(buffer + *size - 1) == 0 )
		(*size)--;
	
	if ( !*size ) {
		if ( !m_pname )
			;
		else if ( m_pname == m_ppath )
			;
		else if ( !*m_pname )
			*(m_ppath = m_pname) = 0;
		else {
			memmove( m_ppath, m_pname, lstrlen(m_pname)+1 );
			m_pname = m_ppath;
		}
		return errOK;
	}
	
	cERROR  err;
	cChBuff	buff(this);
	
	if ( extended_name_processing() && PR_FAIL(err=processString(buffer,*size,buff)) )
		return err;
	
	symb = *(buffer + *size - 1);
	add_slash = ((':' != symb) && ('\\' != symb) && ('/' != symb)); // we have to add slash to the end of the path
	name_len = 1 + (m_pname ? lstrlen(m_pname) : 0);
	new_size = *size + name_len + add_slash;
	
	if ( new_size > MAX_PATH )
		return errBUFFER_TOO_SMALL;
	
	if ( new_size > m_full_name_buff_len ) {
		tSTRING tmp;
		err = heapAlloc( (tPTR*)&tmp, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		
		memcpy( tmp, buffer, *size );
		if ( add_slash ) {
			*(tmp + *size) = PATH_DELIMETER_CHAR;
			(*size) += add_slash;
		}
		
		if ( m_pname )
			memcpy( tmp+*size, m_pname, name_len );
		else
			*(tmp+*size) = 0;
		
		if ( m_full_name )
			heapFree( m_full_name );
		
		m_full_name = m_ppath = tmp;
		m_pname = m_ppath + *size;
		m_full_name_buff_len = new_size;
	}
	else {
		tUINT path_len = _toui32(m_pname - m_ppath);
		if ( (*size+add_slash) != path_len ) {
			memmove( m_ppath+*size+add_slash, m_ppath+path_len, name_len );
			m_pname = m_ppath + (*size+add_slash);
		}
		memcpy( m_ppath, buffer, *size );
		if ( add_slash ) {
			*(m_ppath + *size) = PATH_DELIMETER_CHAR;
			(*size) += add_slash;
		}
	}
	check_slashes();
	return errOK;
}



// ---
tERROR pr_call Win32IOData::iSetFull( tSTRING buffer, tDWORD* size, tDWORD path_len ) {
	tUINT new_size;
	
	while( *size && *(buffer + *size - 1) == 0 )
		(*size)--;
	
	if ( !*size ) {
		if ( 0 != (m_pname=m_ppath) )
			*(m_ppath) = 0;
		return errOK;
	}
	
	cERROR  err;
	cChBuff	buff(this);
	
	if ( extended_name_processing() && PR_FAIL(err=processString(buffer,*size,buff)) )
		return err;
	
	new_size = *size + MBS;
	if ( new_size > MAX_PATH ) {
		*size = 0;
		return errBUFFER_TOO_SMALL;
	}
	
	if ( new_size > m_full_name_buff_len ) {
		tSTRING tmp;
		err = heapAlloc( (tPTR*)&tmp, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		if ( m_full_name )
			heapFree( m_full_name );
		m_ppath = m_full_name = tmp;
		m_full_name_buff_len = new_size;
	}
	
	memcpy( m_ppath, buffer, *size );
	*(m_ppath + *size) = 0;
	m_pname = m_ppath + path_len;
	
	check_slashes();
	return errOK;
}




// ---
tERROR pr_call Win32IOData::iSetNameW( tWSTRING buffer, tDWORD* size ) {
	tUINT path_len;
	tUINT new_size;
	const tWCHAR* prefix;
	tUINT    adv;
	tUINT    prefix_len;
	
	while( *size && (0 == *(UNI(MB(buffer) + *size) - 1)) )
		(*size) -= UNIS;
	
	if ( !*size ) {
		if ( m_pname )
			*UNI(m_pname) = 0;
		return errOK;
	}
	
	cERROR   err;
	cwChBuff buff(this);
	if ( extended_name_processing() && PR_FAIL(err=processStringW(buffer,*size,buff)) )
		return err;
	
	path_len = _toui32(m_pname - m_full_name);
	if ( !path_len && !windows_name_parsing() ) { // path is not set yet
		prefix = get_prefix( buffer, *size, &adv, &prefix_len );
		path_len = prefix_len;
	}
	else {           // prefix already set in the path
		prefix = 0;
		adv = 0;
		prefix_len = 0;
	}
	new_size = path_len + *size + UNIS;
	
	if ( new_size > UNICODE_MAX_NAME ) {
		*size = 0;
		return errBUFFER_TOO_SMALL;
	}
	
	if ( new_size > m_full_name_buff_len ) {
		tUINT  plen = prefix_len ? prefix_len : _toui32(m_ppath - m_full_name);
		err = heapRealloc( (tPTR*)&m_full_name, m_full_name, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		m_ppath = m_full_name + plen;
		m_pname = m_full_name + path_len;
		m_full_name_buff_len = new_size;
	}
	
	if ( prefix )
		memcpy( m_full_name, prefix, prefix_len );
	memcpy( m_pname, buffer, *size );
	*UNI(MB(m_pname) + *size) = 0;
	
	check_slashesW();
	return errOK;
}



// ---
tERROR pr_call Win32IOData::iSetPathW( tWSTRING buffer, tDWORD* size ) {
	tUINT    name_len;
	tUINT    new_size;
	tWCHAR   symb;
	tUINT    add_slash;
	const tWCHAR* prefix;
	tUINT    adv;
	tUINT    prefix_len;
	
	while( *size && *(UNI(MB(buffer) + *size) - 1) == 0 )
		(*size) -= UNIS;
	
	cERROR   err;
	cwChBuff buff(this);
	if ( extended_name_processing() && PR_FAIL(err=processStringW(buffer,*size,buff)) )
		return err;
	
	if ( windows_name_parsing() ) {
		prefix = 0;
		adv = 0;
		prefix_len = 0;
	}
	else
		prefix = get_prefix( buffer, *size, &adv, &prefix_len );
	
	buffer += adv;
	*size -= adv * UNIS;
	
	if ( !*size ) {
		if ( !m_full_name )                 // nothing is set yet
			;
		else if ( m_pname == m_full_name ) // path is not set yet
			;
		else {                                // colapse the path and prefix
			if ( *m_pname )
				memmove( m_ppath=m_full_name, m_pname, (wcslen(UNI(m_pname))+1)*UNIS );
			else
				*UNI(m_ppath=m_full_name) = 0;
			m_pname = m_ppath;
		}
		return errOK;
	}
	
	symb = *(UNI(MB(buffer) + *size) - 1);
	add_slash = UNIS * ((L':' != symb) && (L'\\' != symb) && (L'/' != symb)); // we have to add slash to the end of the path
	name_len = _toui32((1 + (!m_pname ? 0 : wcslen(UNI(m_pname)))) * UNIS);
	new_size = prefix_len + *size + add_slash + name_len;
	
	if ( new_size > UNICODE_MAX_PATH ) {
		*size = 0;
		return errBUFFER_TOO_SMALL;
	}
	
	if ( new_size > m_full_name_buff_len ) {
		tWSTRING tmp;
		err = heapAlloc( (tPTR*)&tmp, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		
		if ( prefix_len )
			memcpy( MB(tmp), prefix, prefix_len );
		memcpy( MB(tmp)+prefix_len, buffer, *size );
		if ( add_slash ) {
			*UNI(MB(tmp) + prefix_len + *size) = PATH_DELIMETER_WIDECHAR;
			(*size) += add_slash;
		}
		
		if ( m_pname )
			memcpy( MB(tmp)+prefix_len+*size, m_pname, name_len );
		else
			*(MB(tmp)+prefix_len+*size) = 0;
		
		if ( m_full_name )
			heapFree( m_full_name );
		
		m_full_name = MB(tmp);
		m_ppath = m_full_name + prefix_len;
		m_pname = m_ppath + *size;
		m_full_name_buff_len = new_size;
	}
	else {
		tUINT path_len = _toui32(m_pname - m_ppath);
		if ( prefix_len )
			memcpy( m_full_name, prefix, prefix_len );
		if ( (*size+add_slash) != path_len ) {
			memmove( m_ppath+(*size+add_slash), m_ppath+path_len, name_len );
			m_pname = m_ppath + (*size + add_slash);
		}
		memcpy( m_ppath, buffer, *size );
		if ( add_slash ) {
			*UNI(MB(m_ppath) + *size) = PATH_DELIMETER_WIDECHAR;
			(*size) += add_slash;
		}
	}
	check_slashesW();
	return errOK;
}



// ---
tERROR pr_call Win32IOData::iSetFullW( tWSTRING buffer, tDWORD* size, tDWORD path_len ) {
	tUINT new_size;
	const tWCHAR* prefix;
	tUINT adv;
	tUINT prefix_len;
	
	while( *size && *(UNI(MB(buffer) + *size) - 1) == 0 )
		(*size) -= UNIS;
	
	cERROR   err;
	cwChBuff buff(this);
	if ( extended_name_processing() && PR_FAIL(err=processStringW(buffer,*size,buff)) )
		return err;
	
	if ( windows_name_parsing() ) {
		tUINT dummy;
		prefix = 0;
		prefix_len = 0;
		const tWCHAR* pr = get_prefix( buffer, *size, &adv, &dummy );
		if ( (pr == g_prefix_unc) && (adv == 2) )
			adv = 0;
	}
	else
		prefix = get_prefix( buffer, *size, &adv, &prefix_len );
	
	buffer += adv;
	path_len -= adv * UNIS;
	*size -= adv * UNIS;
	
	if ( !*size ) {
		if ( 0 != (m_pname=m_ppath) )
			*(m_ppath) = 0;
		return errOK;
	}
	
	new_size = prefix_len + *size + UNIS;
	if ( new_size > UNICODE_MAX_PATH ) {                     
		size = 0;
		return errBUFFER_TOO_SMALL;
	}
	
	if ( new_size > m_full_name_buff_len ) {
		tSTRING tmp;
		
		err = heapAlloc( (tPTR*)&tmp, new_size );
		if ( PR_FAIL(err) ) {
			*size = 0;
			return err;
		}
		
		if ( m_full_name )
			heapFree( m_full_name );
		
		if ( prefix_len )
			memcpy( tmp, prefix, prefix_len );
		m_full_name = tmp;
		m_ppath = m_full_name + prefix_len;
		m_full_name_buff_len = new_size;
	}
	
	tUINT deleted_slash_bytes = 0;
	memcpy( m_ppath, buffer, *size );
	*UNI(m_ppath + *size) = 0;
	m_pname = m_ppath + path_len - deleted_slash_bytes;

	check_slashesW();
	return errOK;
}

extern "C" tERROR pr_call NFIO_RemoveDir(tPTR p_path, tCODEPAGE cp)
{
	cStringObj strPath; strPath.assign(p_path, cp);
	
	tERROR error = errOK;
	cAutoObj<cObjPtr> hPtr;
	error = g_root->sysCreateObjectQuick((hOBJECT*)&hPtr, IID_OBJPTR, PID_NATIVE_FIO);
	if (PR_FAIL(error))
		return error;
	if (PR_FAIL(error = hPtr->ChangeTo(cAutoString(strPath))))
		return error;
	if (PR_FAIL(error = hPtr->StepDown()))
		return error;
	
	while(PR_SUCC(hPtr->Next())) 
	{
		cStringObj name(hPtr, pgOBJECT_FULL_NAME);
		if (hPtr->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_DIRECTORY)
		{
			if( PR_FAIL(error = NFIO_RemoveDir((tPTR)name.data(), cCP_UNICODE)) )
			{
				PR_TRACE((g_root, prtERROR, "nfio\tRemoveDir: subdir %S, %terr", name.data(), error));
				return error;
			}
		}
		else
		{
			hPtr->set_pgOBJECT_ATTRIBUTES(0);
			if( PR_FAIL(error = hPtr->Delete()) )
			{
				PR_TRACE((g_root, prtERROR, "nfio\tRemoveDir: file %S, %terr", name.data(), error));
				return error;
			}
		}
	
		hPtr->Reset(cFALSE);
	};
	
	if (PR_SUCC(error = hPtr->ChangeTo(cAutoString(strPath))))
	{
		hPtr->set_pgOBJECT_ATTRIBUTES( hPtr->get_pgOBJECT_ATTRIBUTES() & fATTRIBUTE_DIRECTORY );
		if( PR_FAIL(error = hPtr->Delete()) )
		{
			PR_TRACE((g_root, prtERROR, "nfio\tRemoveDir: dir %S, %terr", strPath.data(), error));
		}
	}
	else if ( error == errEND_OF_THE_LIST )
		error = errOBJECT_NOT_FOUND;
	
	return error;
}




#if defined( _PERFORM_TEST_ )


// ---
tERROR pr_call Win32IOData::_test_0( Win32IOData* io, cStrObj& obj, const tWCHAR* path ) {
	cERROR err;
	cAutoObj<cObjPtr> ptr;

	obj.clean();
	err = io->sysCreateObject( ptr, IID_OBJPTR, PID_WIN32_NFIO );
	if ( PR_SUCC(err) )
		err = cStrObj(path).copy( ptr, pgOBJECT_PATH );
	if ( PR_SUCC(err) )
		err = ptr->sysCreateObjectDone();
	if ( PR_SUCC(err) )
		err = ptr->Reset( cTRUE );
	if ( PR_SUCC(err) )
		err = ptr->Next();
	if ( PR_SUCC(err) )
		err = obj.assign( ptr, pgOBJECT_FULL_NAME );
	return err;
}



// ---
bool operator == ( FILETIME& ft0, FILETIME& ft1 ) {
	return (ft0.dwLowDateTime == ft1.dwLowDateTime) && (ft0.dwHighDateTime == ft1.dwHighDateTime);
}
bool operator != ( FILETIME& ft0, FILETIME& ft1 ) {
	return (ft0.dwLowDateTime != ft1.dwLowDateTime) || (ft0.dwHighDateTime != ft1.dwHighDateTime);
}

// ---
tERROR pr_call Win32IOData::_test() {
	cERROR  err;
//	cStrObj obj;

//	err = _test_0( obj, L"c:\\windows\\temp" );
//	err = _test_0( obj, L"\\\\kl\\global\\boxes\\petrovitch\\t1\\t2" );
//	err = _test_0( obj, L"\\\\?\\c:\\windows\\temp" );
//	err = _test_0( obj, L"\\\\?\\UNC\\kl\\global\\boxes\\petrovitch\\t1\\t2" );

	WIN32_FIND_DATA fd;
	FILETIME at0, at1;
	HANDLE  fh = FindFirstFile( "C:\\robot\\Client\\client.pl", &fd );
	if ( fh ) {
		at0 = fd.ftLastAccessTime;
		FindClose( fh );
	}
	cAutoObj<cIO> io;
	tBYTE buff[MAX_PATH];
	err = g_root->sysCreateObject( io, IID_IO, PID_WIN32_NFIO );
	if ( PR_SUCC(err) )
		err = cStrObj("C:\\robot\\Client\\client.pl").copy( io, pgOBJECT_FULL_NAME );
	if ( PR_SUCC(err) )
		err = io->set_pgOBJECT_ACCESS_MODE( fACCESS_READ|fACCESS_RESTORE_ACCESS_TIME );
	//if ( PR_SUCC(err) )
	//	err = io->propSetBool( plRESTORE_FILE_TIMES, cTRUE );
	if ( PR_SUCC(err) )
		err = io->sysCreateObjectDone();
	if ( PR_SUCC(err) )
		err = io->SeekRead( 0, 0, buff, sizeof(buff) );
	if ( PR_SUCC(err) )
		err = io->sysCloseObject();
	fh = FindFirstFile( "C:\\robot\\Client\\client.pl", &fd );
	if ( fh ) {
		at1 = fd.ftLastAccessTime;
		FindClose( fh );
	}
	if ( at0 != at1 ) {
		at0.dwLowDateTime = 0;
	}
	return err;
}


#endif

