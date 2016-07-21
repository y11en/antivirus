#if !defined(_kldirobj_h_)
#define _kldirobj_h_

#ifdef KLDIROBJ_EXPORTS
	#define OBJ_DIR_IMPEX __declspec(dllexport)
#else
	#include <windows.h>
	#define OBJ_DIR_IMPEX __declspec(dllimport)
#endif

#if defined( __cplusplus )
	extern "C" {
#endif


#define KL_OBJ_PREFIX         L"KLObj_"
#define KL_OBJ_DIR_NAME       L"\\KLObj"
#define KL_OBJ_PREFIX_SIZE	  (sizeof(KL_OBJ_PREFIX)-sizeof(WCHAR))
		
#define KL_OBJ_PREFIX_A       "KLObj_"
#define KL_OBJ_DIR_NAME_A     "\\KLObj"
#define KL_OBJ_PREFIX_A_SIZE	(sizeof(KL_OBJ_PREFIX_A)-sizeof(CHAR))

#define GLOBAL_PREFIX         L"Global\\"
#define GLOBAL_PREFIX_SIZE    (sizeof(GLOBAL_PREFIX) - sizeof(tWCHAR))

#define GLOBAL_PREFIX_A       "Global\\"
#define GLOBAL_PREFIX_A_SIZE  (sizeof(GLOBAL_PREFIX_A) - sizeof(tCHAR))

#define KL_MAX_PREFIX_SIZE    (GLOBAL_PREFIX_SIZE + KL_OBJ_PREFIX_SIZE)
#define KL_MAX_PREFIX_A_SIZE  (GLOBAL_PREFIX_A_SIZE + KL_OBJ_PREFIX_A_SIZE)



// ---		
typedef struct _SECURITY_ATTRIBUTES SAttr;


#define KL_OBJ_DIR_API __cdecl


#define KL_DIRECTORY_QUERY                 (0x0001)
#define KL_DIRECTORY_ENUMERATE             (0x0002)
#define KL_DIRECTORY_CREATE_OBJECT         (0x0004)
#define KL_DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define KL_DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

#if defined( __cplusplus )

	// ---
	struct cKLDirObject {
		BOOL   inited;
		PVOID  reserved;

		operator bool () const { return !!inited; }

		virtual BOOL   KL_OBJ_DIR_API init              ();
		virtual BOOL   KL_OBJ_DIR_API deinit            ();
		virtual BOOL   KL_OBJ_DIR_API adjust_priveleges ();
		virtual BOOL   KL_OBJ_DIR_API release_priveleges();

		virtual HANDLE KL_OBJ_DIR_API open_dir          ( DWORD access );
		virtual HANDLE KL_OBJ_DIR_API create_dir        ();

		virtual HANDLE KL_OBJ_DIR_API open_event_a      ( DWORD  access, BOOL bInheritHandle, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_event_a    ( SAttr* secatt, BOOL manual_reset,  BOOL initial_state, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API open_event_w      ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_event_w    ( SAttr* secatt, BOOL manual_reset,  BOOL initial_state, PCWSTR name );
		
		virtual HANDLE KL_OBJ_DIR_API open_mutex_a      ( DWORD  access, BOOL bInheritHandle, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_mutex_a    ( SAttr* secatt, BOOL initial_owner, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API open_mutex_w      ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_mutex_w    ( SAttr* secatt, BOOL initial_owner, PCWSTR name );
		
		virtual HANDLE KL_OBJ_DIR_API open_semaphore_a  ( DWORD  access, BOOL bInheritHandle, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_semaphore_a( SAttr* secatt, LONG initial_count, LONG maximum_count, PCSTR name );
		virtual HANDLE KL_OBJ_DIR_API open_semaphore_w  ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
		virtual HANDLE KL_OBJ_DIR_API create_semaphore_w( SAttr* secatt, LONG initial_count, LONG maximum_count, PCWSTR name );

		virtual BOOL   KL_OBJ_DIR_API close_obj         ( HANDLE hobj );
	};

#else

	// ---
	typedef struct tag_KLDirObject cKLDirObject;
	typedef struct tag_KLDirObjectVtbl {
		BOOL   (KL_OBJ_DIR_API* init)              ( cKLDirObject* _this );
		BOOL   (KL_OBJ_DIR_API* deinit)            ( cKLDirObject* _this );
		BOOL   (KL_OBJ_DIR_API* adjust_priveleges) ( cKLDirObject* _this );
		BOOL   (KL_OBJ_DIR_API* release_priveleges)( cKLDirObject* _this );
		HANDLE (KL_OBJ_DIR_API* open_dir)          ( cKLDirObject* _this, DWORD access );
		HANDLE (KL_OBJ_DIR_API* create_dir)        ( cKLDirObject* _this );
		
		HANDLE (KL_OBJ_DIR_API* open_event_a)      ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* create_event_a)    ( cKLDirObject* _this, SAttr* secatt, BOOL manual_reset,  BOOL initial_state, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* open_event_w)      ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCWSTR name );
		HANDLE (KL_OBJ_DIR_API* create_event_w)    ( cKLDirObject* _this, SAttr* secatt, BOOL manual_reset,  BOOL initial_state, PCWSTR name );

		HANDLE (KL_OBJ_DIR_API* open_mutex_a)      ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* create_mutex_a)    ( cKLDirObject* _this, SAttr* secatt, BOOL initial_owner, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* open_mutex_w)      ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCWSTR name );
		HANDLE (KL_OBJ_DIR_API* create_mutex_w)    ( cKLDirObject* _this, SAttr* secatt, BOOL initial_owner, PCWSTR name );
		
		HANDLE (KL_OBJ_DIR_API* open_semaphore_a)  ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* create_semaphore_a)( cKLDirObject* _this, SAttr* secatt, LONG initial_count, LONG maximum_count, PCSTR name );
		HANDLE (KL_OBJ_DIR_API* open_semaphore_w)  ( cKLDirObject* _this, DWORD  access, BOOL bInheritHandle, PCWSTR name );
		HANDLE (KL_OBJ_DIR_API* create_semaphore_w)( cKLDirObject* _this, SAttr* secatt, LONG initial_count, LONG maximum_count, PCWSTR name );

		BOOL   (KL_OBJ_DIR_API* close_obj)         ( cKLDirObject* _this, HANDLE hobj );
	} tKLDirObjectVtbl;

	typedef struct tag_KLDirObject {
		tKLDirObjectVtbl* vtbl;
		BOOL              inited;
		PVOID             reserved;
	} cKLDirObject;

#endif

OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _adjust_priveleges( TOKEN_PRIVILEGES* old );
OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _release_priveleges( const TOKEN_PRIVILEGES* old );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_dir( DWORD access );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_dir();

OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_event_a      ( DWORD  access, BOOL bInheritHandle, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_event_a    ( SAttr* secatt, BOOL manual_reset, BOOL initial_state, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_event_w      ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_event_w    ( SAttr* secatt, BOOL manual_reset, BOOL initial_state, PCWSTR name );

OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_mutex_a      ( DWORD  access, BOOL bInheritHandle, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_mutex_a    ( SAttr* secatt, BOOL initial_owner, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_mutex_w      ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_mutex_w    ( SAttr* secatt, BOOL initial_owner, PCWSTR name );

OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_semaphore_a  ( DWORD  access, BOOL bInheritHandle, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_semaphore_a( SAttr* secatt, LONG initial_count, LONG maximum_count, PCSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _open_kl_semaphore_w  ( DWORD  access, BOOL bInheritHandle, PCWSTR name );
OBJ_DIR_IMPEX HANDLE KL_OBJ_DIR_API _create_kl_semaphore_w( SAttr* secatt, LONG initial_count, LONG maximum_count, PCWSTR name );

OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _close_kl_obj( HANDLE obj );

OBJ_DIR_IMPEX BOOL   KL_OBJ_DIR_API _init_kl_dir_obj( cKLDirObject* obj );
typedef OBJ_DIR_IMPEX BOOL (KL_OBJ_DIR_API * pfKLDirObjectInit)( cKLDirObject* obj );

#if defined( __cplusplus )
	}
#endif


#define KLDirObjInitProc "_init_kl_dir_obj"

#endif // _kldirobj_h_