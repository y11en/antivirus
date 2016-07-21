// klavsys.h
//
// OS-specific classes and functions
//

#ifndef klavsys_h_INCLUDED
#define klavsys_h_INCLUDED

#include <klava/klav_io.h>

#ifdef KLAVSYS_EXPORTS
# define KLAVSYS_API KLAV_DLL_EXPORT
#else  // KLAVSYS_EXPORTS
# define KLAVSYS_API
#endif // KLAVSYS_EXPORTS

////////////////////////////////////////////////////////////////
// Error translation

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAV_Translate_System_Error (uint32_t ec);

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAV_Get_System_Error ();

////////////////////////////////////////////////////////////////
// System allocator

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Alloc KLAV_Get_System_Allocator ();

////////////////////////////////////////////////////////////////
// Synchronization objects

////////////////////////////////////////////////////////////////
// Synchronization object: Mutex

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Mutex : public KLDynObject
{
	virtual KLAV_ERR KLAV_CALL lock () = 0;
	virtual KLAV_ERR KLAV_CALL unlock () = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Mutex)
};

typedef KLDynObjectHolder <KLAV_Mutex> KLAV_Mutex_Holder;

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Mutex)
{
	void     (KLAV_CALL *destroy) (struct KLAV_Mutex *);
	KLAV_ERR (KLAV_CALL *lock)    (struct KLAV_Mutex *);
	KLAV_ERR (KLAV_CALL *unlock)  (struct KLAV_Mutex *);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Mutex);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Synchronization object: RWLock (read-write lock)

#ifdef __cplusplus

// read - write lock interface
struct KLAV_NO_VTABLE KLAV_RWLock : public KLDynObject
{
public:
	virtual KLAV_ERR KLAV_CALL lock_read() = 0;
	virtual KLAV_ERR KLAV_CALL lock_write() = 0;
	virtual KLAV_ERR KLAV_CALL unlock() = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_RWLock)
};

typedef KLDynObjectHolder <KLAV_RWLock> KLAV_RWLock_Holder;

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_RWLock)
{
	void     (KLAV_CALL *destroy)    (struct KLAV_RWLock *);
	KLAV_ERR (KLAV_CALL *lock_read)  (struct KLAV_RWLock *);
	KLAV_ERR (KLAV_CALL *lock_write) (struct KLAV_RWLock *);
	KLAV_ERR (KLAV_CALL *unlock)     (struct KLAV_RWLock *);
};

KLAV_DECLARE_C_INTERFACE(KLAV_RWLock);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Synchronization object: CondVar (condition variable)

#ifdef __cplusplus

// read - write lock interface
struct KLAV_NO_VTABLE KLAV_CondVar : public KLDynObject
{
	// Interface compatible with Mutex
	virtual KLAV_ERR KLAV_CALL lock () = 0;
	virtual KLAV_ERR KLAV_CALL unlock () = 0;
	virtual KLAV_ERR KLAV_CALL broadcast() = 0;
	virtual KLAV_ERR KLAV_CALL wait() = 0;
	virtual KLAV_ERR KLAV_CALL timedwait(uint32_t) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_CondVar)
};

typedef KLDynObjectHolder <KLAV_CondVar> KLAV_CondVar_Holder;

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_CondVar)
{
	void     (KLAV_CALL *destroy)    (struct KLAV_CondVar *);
	KLAV_ERR (KLAV_CALL *lock)       (struct KLAV_CondVar *);
	KLAV_ERR (KLAV_CALL *unlock)     (struct KLAV_CondVar *);
	KLAV_ERR (KLAV_CALL *broadcast)  (struct KLAV_CondVar *);
	KLAV_ERR (KLAV_CALL *wait)       (struct KLAV_CondVar *);
	KLAV_ERR (KLAV_CALL *timedwait)  (struct KLAV_CondVar *, uint32_t);
};

KLAV_DECLARE_C_INTERFACE(KLAV_CondVar);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Synchronization object factory

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Sync_Factory
{
	virtual KLAV_Mutex *  KLAV_CALL create_mutex (klav_bool_t recursive) = 0;
	virtual KLAV_RWLock * KLAV_CALL create_rwlock () = 0;
	virtual KLAV_CondVar * KLAV_CALL create_condvar () = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Sync_Factory)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Sync_Factory)
{
	struct KLAV_Mutex *  (KLAV_CALL *create_mutex) (struct KLAV_Sync_Factory *, klav_bool_t recursive);
	struct KLAV_RWLock * (KLAV_CALL *create_rwlock) (struct KLAV_Sync_Factory *);
	struct KLAV_CondVar * (KLAV_CALL *create_condition) (struct KLAV_Sync_Factory *);
};

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Sync_Factory);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// helper class (scope lock guard)

#ifdef __cplusplus

template <class LOCK>
class KLAV_Lock_Holder
{
public:
	KLAV_Lock_Holder (LOCK *lock = 0)
		: m_lock (lock) {}

	~KLAV_Lock_Holder ()
		{ if (m_lock != 0) m_lock->unlock (); }

	inline KLAV_Lock_Holder& operator= (LOCK *lock)
		{ attach (lock); return *this; }

	inline void attach (LOCK *lock)
		{ m_lock = lock; }

	inline void detach ()
		{ m_lock = 0; }

	inline LOCK * operator-> () const
		{ return m_lock; }

	inline void unlock ()
		{ m_lock->unlock (); }

private:
	LOCK * m_lock;
};

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// System synchronization objects

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Mutex KLAV_CALL KLAVSYS_Create_Mutex (klav_bool_t recursive);

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_RWLock KLAV_CALL KLAVSYS_Create_RWLock ();

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_CondVar KLAV_CALL KLAVSYS_Create_CondVar ();

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Sync_Factory KLAV_CALL KLAVSYS_Get_Sync_Factory ();

////////////////////////////////////////////////////////////////
// DSO loading

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_DSO_Load (
			const char * path,
			uint32_t flags,
			hKLAV_Module* phdl
		);

KLAV_EXTERN_C
KLAVSYS_API
void * KLAV_CALL KLAVSYS_DSO_Symbol (
			hKLAV_Module hdl,
			const char * name
		);

KLAV_EXTERN_C
KLAVSYS_API
void KLAV_CALL KLAVSYS_DSO_Unload (
			hKLAV_Module hdl
		);

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_DSO_Loader KLAV_CALL KLAVSYS_Get_DSO_Loader ();

////////////////////////////////////////////////////////////////
// Virtual memory

// virtual memory protection flags
enum
{
	KLAV_MEM_PROT_READ       = 0x01,
	KLAV_MEM_PROT_WRITE      = 0x02,
	KLAV_MEM_PROT_EXEC       = 0x04,
	// mask to cover all bits
	KLAV_MEM_PROT_MASK       = KLAV_MEM_PROT_READ|KLAV_MEM_PROT_WRITE|KLAV_MEM_PROT_EXEC,
};

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Virtual_Memory
{
	// returns system page size
	virtual uint32_t KLAV_CALL vm_pagesize () = 0;
	// allocate virtual memory region (set pref_addr for default behavior)
	virtual KLAV_ERR KLAV_CALL vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr) = 0;
	// deallocate virtual memory region
	virtual KLAV_ERR KLAV_CALL vm_free (void *ptr, uint32_t size) = 0;
	// change memory region protection
	virtual KLAV_ERR KLAV_CALL vm_protect (void *ptr, uint32_t size, uint32_t newprot) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Virtual_Memory)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Virtual_Memory)
{
	uint32_t (KLAV_CALL *vm_pagesize) (struct KLAV_Virtual_Memory *);
	KLAV_ERR (KLAV_CALL *vm_alloc)    (struct KLAV_Virtual_Memory *, void **pptr, uint32_t size, uint32_t prot, void *pref_addr);
	KLAV_ERR (KLAV_CALL *vm_free)     (struct KLAV_Virtual_Memory *, void *ptr, uint32_t size);
	KLAV_ERR (KLAV_CALL *vm_protect)  (struct KLAV_Virtual_Memory *, void *ptr, uint32_t size, uint32_t newprot);
};

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Virtual_Memory);

#endif // __cplusplus

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Virtual_Memory KLAV_CALL KLAVSYS_Get_Virtual_Memory ();

////////////////////////////////////////////////////////////////
// IO library

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Create_File (
			const char * path,
			uint32_t access,
			uint32_t flags,
			hKLAV_Properties props,
			hKLAV_IO_Object *pobject
		);

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Delete_File (
			const char * path,
			uint32_t flags
		);

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Move_File (
			const char * src_path,
			const char * dst_path,
			uint32_t flags
		);

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_IO_Library KLAV_CALL KLAVSYS_Get_IO_Library ();


////////////////////////////////////////////////////////////////
// Temp file management

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAV_Create_Temp_File (
			hKLAV_Properties props,
			hKLAV_IO_Object *ptempobj
		);

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Temp_Object_Manager KLAV_CALL KLAVSYS_Get_Temp_Object_Manager ();

////////////////////////////////////////////////////////////////
// KFB file database loader

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Open_DB_File (
			const char * fname,
			hKLAV_DB_Loader *phdl
		);

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Close_DB_File (
			hKLAV_DB_Loader hdl
		);

////////////////////////////////////////////////////////////////
// Standard procedure resolver implementation

KLAV_EXTERN_C
KLAVSYS_API
hKLAV_Proc_Resolver KLAV_CALL KLAVSYS_Create_Proc_Resolver (
			hKLAV_Proc_Resolver chain
		);

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAV_CALL KLAVSYS_Destroy_Proc_Resolver (
			hKLAV_Proc_Resolver obj
		);

////////////////////////////////////////////////////////////////
// KLAVA module loader implementation

#define KLAV_MODULE_INIT 200
#define KLAV_MODULE_FINI 201

// this struct is passed as LPARAM to module load procedure on INIT and FINI calls
struct KLAV_Module_Init_Struct
{
	uint32_t     mod_init_sizeof;   // sizeof structure
	KLAV_ERR     mod_init_error;    // place to set error code, if nonzero - module will be unloaded
	hKLAV_Alloc  mod_init_alloc;    // ptr to allocator (set by loader)
	void *       mod_init_context;  // place to store context data (will be passed back on FINI)
};

#endif // klavsys_h_INCLUDED

