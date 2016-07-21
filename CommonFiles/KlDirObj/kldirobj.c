#include <memory.h>
#include <stdlib.h>
#include <wchar.h>

#define CONST               const

typedef void                VOID;
typedef char                CHAR;
typedef unsigned char       BYTE;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef unsigned char       UCHAR;
typedef unsigned short      USHORT;

typedef void*               PVOID;
typedef void*               LPVOID;
typedef void*               HANDLE;
typedef HANDLE*             PHANDLE;

#if !defined(WCHAR_DEFINED)
typedef unsigned short      WCHAR;    // wc,   16-bit UNICODE character
#endif
typedef WCHAR *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;
typedef BYTE                BOOLEAN;           
typedef BOOLEAN             BOOL;
typedef long                LONG;
typedef LONG                NTSTATUS;
typedef DWORD               ACCESS_MASK;
typedef ACCESS_MASK*        PACCESS_MASK;

#define DECLSPEC_IMPORT     __declspec(dllimport)
#define NTSYSAPI            DECLSPEC_IMPORT
#define WINBASEAPI          DECLSPEC_IMPORT

#define NTAPI               __stdcall
#define WINAPI              __stdcall


#define HEAP_NO_SERIALIZE               0x00000001      
#define HEAP_GROWABLE                   0x00000002      
#define HEAP_GENERATE_EXCEPTIONS        0x00000004      
#define HEAP_ZERO_MEMORY                0x00000008      
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010      
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020      
#define HEAP_FREE_CHECKING_ENABLED      0x00000040      
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080      
#define HEAP_CREATE_ALIGN_16            0x00010000      
#define HEAP_CREATE_ENABLE_TRACING      0x00020000      
#define HEAP_MAXIMUM_TAG                0x0FFF              
#define HEAP_PSEUDO_TAG_FLAG            0x8000              
#define HEAP_TAG_SHIFT                  18                  
#define HEAP_MAKE_TAG_FLAGS( b, o ) ((DWORD)((b) + ((o) << 18)))  

#define IN
#define OUT
#define OPTIONAL
#define FALSE               0
#define TRUE                1

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_VALID_ATTRIBUTES    0x000001F2L

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

#define EVENT_QUERY_STATE       0x0001
#define EVENT_MODIFY_STATE      0x0002  // winnt
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt

typedef struct _LUID {
	DWORD LowPart;
	LONG  HighPart;
} LUID, *PLUID;

#define ANYSIZE_ARRAY 1       

#include <pshpack4.h>

typedef struct _LUID_AND_ATTRIBUTES {
	LUID Luid;
	DWORD Attributes;
} LUID_AND_ATTRIBUTES, * PLUID_AND_ATTRIBUTES;
typedef LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef LUID_AND_ATTRIBUTES_ARRAY *PLUID_AND_ATTRIBUTES_ARRAY;

#include <poppack.h>

// ---
typedef struct _TOKEN_PRIVILEGES {
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY];
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES;

// ---
typedef struct _SECURITY_ATTRIBUTES {
	ULONG nLength;
	void* lpSecurityDescriptor;
	BOOLEAN bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;


// ---
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;


// ---
typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = 0;               \
    }

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // ntsubauth

typedef enum _EVENT_TYPE {
  NotificationEvent,
  SynchronizationEvent
} EVENT_TYPE;

/*
typedef USHORT SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;

#define SE_OWNER_DEFAULTED               (0x0001)
#define SE_GROUP_DEFAULTED               (0x0002)
#define SE_DACL_PRESENT                  (0x0004)
#define SE_DACL_DEFAULTED                (0x0008)
#define SE_SACL_PRESENT                  (0x0010)
#define SE_SACL_DEFAULTED                (0x0020)
// end_winnt
#define SE_DACL_UNTRUSTED                (0x0040)
#define SE_SERVER_SECURITY               (0x0080)
// begin_winnt
#define SE_SELF_RELATIVE                 (0x8000)

//
//  Where:
//
//      SE_OWNER_DEFAULTED - This boolean flag, when set, indicates that the
//          SID pointed to by the Owner field was provided by a
//          defaulting mechanism rather than explicitly provided by the
//          original provider of the security descriptor.  This may
//          affect the treatment of the SID with respect to inheritence
//          of an owner.
//
//      SE_GROUP_DEFAULTED - This boolean flag, when set, indicates that the
//          SID in the Group field was provided by a defaulting mechanism
//          rather than explicitly provided by the original provider of
//          the security descriptor.  This may affect the treatment of
//          the SID with respect to inheritence of a primary group.
//
//      SE_DACL_PRESENT - This boolean flag, when set, indicates that the
//          security descriptor contains a discretionary ACL.  If this
//          flag is set and the Dacl field of the SECURITY_DESCRIPTOR is
//          null, then a null ACL is explicitly being specified.
//
//      SE_DACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the DaclPresent flag is not set.
//
//      SE_SACL_PRESENT - This boolean flag, when set,  indicates that the
//          security descriptor contains a system ACL pointed to by the
//          Sacl field.  If this flag is set and the Sacl field of the
//          SECURITY_DESCRIPTOR is null, then an empty (but present)
//          ACL is being specified.
//
//      SE_SACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Sacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the SaclPresent flag is not set.
//
// end_winnt
//      SE_DACL_TRUSTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a trusted source
//          and does not require any editing of compound ACEs.  If this flag
//          is not set and a compound ACE is encountered, the system will
//          substitute known valid SIDs for the server SIDs in the ACEs.
//
//      SE_SERVER_SECURITY - This boolean flag, when set, indicates that the
//         caller wishes the system to create a Server ACL based on the
//         input ACL, regardess of its source (explicit or defaulting.
//         This is done by replacing all of the GRANT ACEs with compound
//         ACEs granting the current server.  This flag is only
//         meaningful if the subject is impersonating.
//
// begin_winnt
//      SE_SELF_RELATIVE - This boolean flag, when set, indicates that the
//          security descriptor is in self-relative form.  In this form,
//          all fields of the security descriptor are contiguous in memory
//          and all pointer fields are expressed as offsets from the
//          beginning of the security descriptor.  This form is useful
//          for treating security descriptors as opaque data structures
//          for transmission in communication protocol or for storage on
//          secondary media.
//
//
//
// Pictorially the structure of a security descriptor is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------------------------------------------------------+
//      |            Control            |Reserved1 (SBZ)|   Revision    |
//      +---------------------------------------------------------------+
//      |                            Owner                              |
//      +---------------------------------------------------------------+
//      |                            Group                              |
//      +---------------------------------------------------------------+
//      |                            Sacl                               |
//      +---------------------------------------------------------------+
//      |                            Dacl                               |
//      +---------------------------------------------------------------+
//
// In general, this data structure should be treated opaquely to ensure future
// compatibility.
//
//

typedef PVOID PSID;     // winnt

//
// Allocate the System Luid.  The first 1000 LUIDs are reserved.
// Use #999 here (0x3E7 = 999)
//

#define SYSTEM_LUID                     { 0x3E7, 0x0 }

// This is the *current* ACL revision

#define ACL_REVISION     (2)

// This is the history of ACL revisions.  Add a new one whenever
// ACL_REVISION is updated

#define ACL_REVISION1   (1)
#define ACL_REVISION2   (2)
#define ACL_REVISION3   (3)

typedef struct _ACL {
	UCHAR  AclRevision;
	UCHAR  Sbz1;
	USHORT AclSize;
	USHORT AceCount;
	USHORT Sbz2;
} ACL;
typedef ACL *PACL;


typedef struct _SECURITY_DESCRIPTOR {
	UCHAR Revision;
	UCHAR Sbz1;
	SECURITY_DESCRIPTOR_CONTROL Control;
	PSID Owner;
	PSID Group;
	PACL Sacl;
	PACL Dacl;
} SECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;


// Handle Of Security Attributes
typedef struct tag_HSEC_ATTR {
	PSID				        pSID;
	PACL				        pACL;
	SECURITY_ATTRIBUTES	SA;
	SECURITY_DESCRIPTOR	SD;

} HSEC_ATTR, *PHSEC_ATTR, _sa;


#define _tosa(sa)  (sa ? &((_sa*)sa)->SA : 0)
*/

#define _tosa(sa)  (sa)

// ---
#include "kldirobj.h"


// ---
OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _adjust_priveleges( TOKEN_PRIVILEGES* tp );
OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _release_priveleges( const TOKEN_PRIVILEGES* tp );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_dir( DWORD access );


// ---
PVOID _heap_alloc( size_t size );
VOID  _heap_free( PVOID mem );


// ---
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

NTSYSAPI VOID     NTAPI RtlFreeUnicodeString( PUNICODE_STRING UnicodeString );
NTSYSAPI VOID     NTAPI RtlInitUnicodeString( PUNICODE_STRING DestinationString, PCWSTR SourceString );
NTSYSAPI NTSTATUS NTAPI ZwCreateDirectoryObject( OUT PHANDLE DirectoryHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes );
NTSYSAPI NTSTATUS NTAPI ZwOpenDirectoryObject( OUT PHANDLE DirectoryHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes );
NTSYSAPI NTSTATUS NTAPI ZwOpenEvent( OUT PHANDLE EventHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes );
NTSYSAPI NTSTATUS NTAPI ZwCreateEvent( OUT PHANDLE EventHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN EVENT_TYPE EventType, IN BOOLEAN InitialState );
NTSYSAPI NTSTATUS NTAPI ZwOpenMutant( OUT PHANDLE MutantHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes );
NTSYSAPI NTSTATUS NTAPI ZwCreateMutant( OUT PHANDLE MutantHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN BOOLEAN InitialOwner );
NTSYSAPI NTSTATUS NTAPI ZwOpenSemaphore( OUT PHANDLE SemaphoreHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes );
NTSYSAPI NTSTATUS NTAPI ZwCreateSemaphore( OUT PHANDLE SemaphoreHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN LONG InitialCount, IN LONG MaximumCount );
NTSYSAPI NTSTATUS NTAPI ZwClose( IN HANDLE Handle );

//#define EVENT_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x0003) 
#define MUTANT_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x0001)
#define SEMAPHORE_ALL_ACCESS     (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x0003) 



#define CONVERT_A2W( name )                                   \
	HANDLE ret;                                                 \
	WCHAR  w##name[0x100];                                      \
	PWSTR pw##name;                                             \
	if ( name && *name ) {                                      \
		pw##name = _to_wcs( name, w##name, sizeof(w##name) );     \
		if ( !pw##name )                                          \
			return 0;                                               \
	}                                                           \
	else                                                        \
		pw##name = 0

#define CLEAN_A2W( name )                                     \
	if ( pw##name && (pw##name != w##name) )                    \
		_heap_free( pw##name )


// ---
PWSTR _to_wcs( PCSTR name, PWSTR buff, size_t len ) {
	PWSTR ret = 0;
	size_t outlen2;
	size_t outlen = mbstowcs( buff, name, len );
	if ( outlen < len )
		return buff;
	if ( outlen == (size_t)-1 )
		return 0;
	outlen = mbstowcs( 0, name, 0 );
	if ( outlen < len )
		return 0;
	ret = (PWSTR)_heap_alloc( outlen );
	if ( !ret )
		return 0;
	outlen2 = mbstowcs( ret, name, outlen );
	if ( outlen2 < outlen ) {
		_heap_free( ret );
		return 0;
	}
	return ret;
}


// ---
static HANDLE KL_OBJ_DIR_API _open_dir( PCWSTR name, DWORD access ) {
	NTSTATUS err;
	HANDLE hdir = 0;
	UNICODE_STRING uname;
	OBJECT_ATTRIBUTES obj;

	if ( !access )
		access = KL_DIRECTORY_QUERY;

	RtlInitUnicodeString( &uname, name );
	InitializeObjectAttributes( &obj, &uname, OBJ_CASE_INSENSITIVE, 0, 0 );
	err = ZwOpenDirectoryObject( &hdir, access, &obj );
	if ( err == STATUS_SUCCESS )
		return hdir;
	return 0;
}



// ---
BOOL _int_prepare_obj_attr( OBJECT_ATTRIBUTES** pobj, LPSECURITY_ATTRIBUTES sec_attrs, BOOL inheritable, UNICODE_STRING* puname ) {
	DWORD attrs = OBJ_OPENIF;
	PVOID sec_descriptor = 0;

	if ( sec_attrs ) {
		sec_descriptor = sec_attrs->lpSecurityDescriptor;
		if ( sec_attrs->bInheritHandle )
			attrs |= OBJ_INHERIT;
	}

	if ( inheritable )
		attrs |= OBJ_INHERIT;

	if ( puname || sec_attrs || inheritable ) {
		HANDLE hdir = _open_dir( KL_OBJ_DIR_NAME, KL_DIRECTORY_QUERY );
		if ( !hdir && puname ) {
			ULONG name_len = (ULONG)(sizeof(WCHAR) * wcslen(puname->Buffer));
			PWSTR prefixed_name = _heap_alloc( name_len + sizeof(KL_OBJ_PREFIX) );
			if ( !prefixed_name )
				return FALSE;
			hdir = _open_dir( L"\\BaseNamedObjects", KL_DIRECTORY_QUERY );
			memcpy( prefixed_name,                             KL_OBJ_PREFIX,  KL_OBJ_PREFIX_SIZE );
			memcpy( ((CHAR*)prefixed_name)+KL_OBJ_PREFIX_SIZE, puname->Buffer, name_len+sizeof(WCHAR) );
			RtlInitUnicodeString( puname, prefixed_name );
		}
		InitializeObjectAttributes( *pobj, puname, attrs, hdir, sec_descriptor );
		return TRUE;
	}
	return TRUE;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_dir( DWORD access ) {
	return _open_dir( KL_OBJ_DIR_NAME, access );
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_dir() {
	NTSTATUS err;
	HANDLE hdir = 0;
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES obj;

	RtlInitUnicodeString( &name, KL_OBJ_DIR_NAME );
	InitializeObjectAttributes( &obj, &name, OBJ_CASE_INSENSITIVE, 0, 0 );
	err = ZwCreateDirectoryObject( &hdir, KL_DIRECTORY_ALL_ACCESS, &obj );
	if ( err == STATUS_SUCCESS )
		return hdir;
	return 0;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_event_w( DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	NTSTATUS err;
	HANDLE event = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;

	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,0,bInheritHandle,puname) )
		return 0;
	
	err = ZwOpenEvent( &event, access, pobj );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return event;
	return 0;
}


// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_event_a( DWORD access, BOOL bInheritHandle, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _open_kl_event_w( access, bInheritHandle, pwname );
	CLEAN_A2W( name );
	return ret;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_event_w( SAttr* sa, BOOLEAN manual_reset, BOOL initial_state, PCWSTR name ) {
	NTSTATUS err;
	HANDLE event = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;
	
	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,_tosa(sa),FALSE,puname) )
		return 0;
	
	err = ZwCreateEvent( &event, EVENT_ALL_ACCESS, pobj, !manual_reset, initial_state );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return event;
	return 0;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_event_a( SAttr* sa, BOOLEAN manual_reset, BOOL initial_state, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _create_kl_event_w( sa, manual_reset, initial_state, pwname );
	CLEAN_A2W( name );
	return ret;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_mutex_w( DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	NTSTATUS err;
	HANDLE mutant = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;
	
	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,0,bInheritHandle,puname) )
		return 0;
	
	err = ZwOpenMutant( &mutant, access, pobj );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return mutant;
	return 0;
}




// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_mutex_a( DWORD access, BOOL bInheritHandle, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _open_kl_mutex_w( access, bInheritHandle, pwname );
	CLEAN_A2W( name );
	return ret;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_mutex_w( SAttr* sa, BOOLEAN initial_owner, PCWSTR name ) {
	NTSTATUS err;
	HANDLE mutant = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;
	
	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,_tosa(sa),FALSE,puname) )
		return 0;
	
	err = ZwCreateMutant( &mutant, MUTANT_ALL_ACCESS, pobj, initial_owner );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return mutant;
	return 0;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_mutex_a( SAttr* sa, BOOLEAN initial_owner, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _create_kl_mutex_w( sa, initial_owner, pwname );
	CLEAN_A2W( name );
	return ret;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_semaphore_w( DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	NTSTATUS err;
	HANDLE semaphore = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;
	
	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,0,bInheritHandle,puname) )
		return 0;
	
	err = ZwOpenSemaphore( &semaphore, access, pobj );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return semaphore;
	return 0;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_semaphore_a( DWORD access, BOOL bInheritHandle, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _open_kl_semaphore_w( access, bInheritHandle, pwname );
	CLEAN_A2W( name );
	return ret;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_semaphore_w( SAttr* sa, LONG initial_count, LONG maximum_count, PCWSTR name ) {
	NTSTATUS err;
	HANDLE semaphore = 0;
	OBJECT_ATTRIBUTES obj, *pobj = &obj;
	UNICODE_STRING uname, *puname = 0;
	
	if ( name && *name ) {
		RtlInitUnicodeString( &uname, name );
		puname = &uname;
	}
	
	if ( !_int_prepare_obj_attr(&pobj,_tosa(sa),FALSE,puname) )
		return 0;
	
	err = ZwCreateSemaphore( &semaphore, SEMAPHORE_ALL_ACCESS, pobj, initial_count, maximum_count );
	if ( pobj && pobj->RootDirectory )
		_close_kl_obj( pobj->RootDirectory );
	if ( puname && (puname->Buffer != name) )
		RtlFreeUnicodeString( puname );
	if ( err == STATUS_SUCCESS )
		return semaphore;
	return 0;
}



// ---
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_semaphore_a( SAttr* sa, LONG initial_count, LONG maximum_count, PCSTR name ) {
	CONVERT_A2W( name );
	ret = _create_kl_semaphore_w( sa, initial_count, maximum_count, pwname );
	CLEAN_A2W( name );
	return ret;
}


// ---
OBJ_DIR_IMPEX BOOL KL_OBJ_DIR_API _close_kl_obj( HANDLE hobj ) {
	if ( hobj )
		return (ZwClose(hobj) == STATUS_SUCCESS);
	return 0;
}





// ---
// interface
// ---
static BOOL KL_OBJ_DIR_API init_obj( cKLDirObject* obj ) {
	return obj->inited = TRUE;
}


// ---
static BOOL KL_OBJ_DIR_API deinit_obj( cKLDirObject* obj ) {
	BOOL ret = FALSE;
	if ( !obj->inited )
		return TRUE;
	if ( obj->reserved )
		ret = obj->vtbl->release_priveleges( obj );
	RtlZeroMemory( obj, sizeof(*obj) );
	return ret;
}




// ---
static BOOL KL_OBJ_DIR_API adjust_priveleges( cKLDirObject* obj ) {
	if ( obj->reserved ) {
		BOOL ret = _release_priveleges( (TOKEN_PRIVILEGES*)obj->reserved );
		if ( !ret )
			return ret;
	}
	else {
		obj->reserved = _heap_alloc( sizeof(TOKEN_PRIVILEGES) );
		if ( !obj->reserved )
			return FALSE;
	}
	if ( !_adjust_priveleges((TOKEN_PRIVILEGES*)obj->reserved) ) {
		_heap_free( obj->reserved );
		obj->reserved = 0;
		return FALSE;
	}
	return TRUE;
}




// ---
static BOOL KL_OBJ_DIR_API release_priveleges( cKLDirObject* obj ) {
	BOOL ret;
	if ( !obj->inited || !obj->reserved )
		return TRUE;
	ret = _release_priveleges( (TOKEN_PRIVILEGES*)obj->reserved );
	_heap_free( obj->reserved );
	obj->reserved = 0;
	return ret;
}



// ---
static HANDLE KL_OBJ_DIR_API open_dir( cKLDirObject* _this, DWORD access ) {
	return _open_dir( KL_OBJ_DIR_NAME, access );
}

// ---
static HANDLE KL_OBJ_DIR_API create_dir( cKLDirObject* _this ) {
	return _create_kl_dir();
}

// ---
static HANDLE KL_OBJ_DIR_API open_event_a( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCSTR name ) {
	return _open_kl_event_a( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_event_a( cKLDirObject* _this, SAttr* sa, BOOL manual_reset,  BOOL initial_state, PCSTR name ) {
	return _create_kl_event_a( sa, manual_reset, initial_state, name );
}

// ---
static HANDLE KL_OBJ_DIR_API open_event_w( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	return _open_kl_event_w( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_event_w( cKLDirObject* _this, SAttr* sa, BOOL manual_reset,  BOOL initial_state, PCWSTR name ) {
	return _create_kl_event_w( sa, manual_reset, initial_state, name );
}

// ---
static HANDLE KL_OBJ_DIR_API open_mutex_a( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCSTR name ) {
	return _open_kl_mutex_a( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_mutex_a( cKLDirObject* _this, SAttr* sa, BOOL initial_owner, PCSTR name ) {
	return _create_kl_mutex_a( sa, initial_owner, name );
}

// ---
static HANDLE KL_OBJ_DIR_API open_mutex_w( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	return _open_kl_mutex_w( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_mutex_w( cKLDirObject* _this, SAttr* sa, BOOL initial_owner, PCWSTR name ) {
	return _create_kl_mutex_w( sa, initial_owner, name );
}

// ---
static HANDLE KL_OBJ_DIR_API open_semaphore_a( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCSTR name ) {
	return _open_kl_semaphore_a( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_semaphore_a( cKLDirObject* _this, SAttr* sa, LONG initial_count, LONG maximum_count, PCSTR name ) {
	return _create_kl_semaphore_a( sa, initial_count, maximum_count, name );
}

// ---
static HANDLE KL_OBJ_DIR_API open_semaphore_w( cKLDirObject* _this, DWORD access, BOOL bInheritHandle, PCWSTR name ) {
	return _open_kl_semaphore_w( access, bInheritHandle, name );
}

// ---
static HANDLE KL_OBJ_DIR_API create_semaphore_w( cKLDirObject* _this, SAttr* sa, LONG initial_count, LONG maximum_count, PCWSTR name ) {
	return _create_kl_semaphore_w( sa, initial_count, maximum_count, name );
}

// ---
static BOOL KL_OBJ_DIR_API close_obj( cKLDirObject* _this, HANDLE hobj ) {
	return _close_kl_obj( hobj );
}


tKLDirObjectVtbl g_KLDirObjectVtbl = {
	init_obj,
	deinit_obj,
	adjust_priveleges,
	release_priveleges,

	open_dir,
	create_dir,
	
	open_event_a,
	create_event_a,
	open_event_w,
	create_event_w,
	
	open_mutex_a,
	create_mutex_a,
	open_mutex_w,
	create_mutex_w,

	open_semaphore_a,
	create_semaphore_a,
	open_semaphore_w,
	create_semaphore_w,

	close_obj,
};

// ---
OBJ_DIR_IMPEX BOOL KL_OBJ_DIR_API _init_kl_dir_obj( cKLDirObject* obj ) {
	if ( !obj )
		return FALSE;
	RtlZeroMemory( obj, sizeof(cKLDirObject) );
	obj->vtbl = &g_KLDirObjectVtbl;
	return obj->vtbl->init( obj );
}




