#ifndef __WKS5_SS_IMPL__
#define __WKS5_SS_IMPL__

#include <wchar.h>
typedef struct _xnode* pxnode;

#if defined(XTREE_MEM_WINDOWS)
#include <windows.h>
static __inline void* xtree_alloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}
static __inline void* xtree_realloc(void* mem, size_t size)
{
	return HeapReAlloc(GetProcessHeap(), 0, mem, size);
}
static __inline void xtree_free(void* mem)
{
	HeapFree(GetProcessHeap(), 0, mem);
}

#elif defined(XTREE_MEM_PRAGUE)
#include <prague.h>
#include <iface/i_root.h>
static __inline void* xtree_alloc(size_t size)
{
	tPTR ptr = NULL;
	g_root->heapAlloc(&ptr, size);
	return ptr;
}
static __inline void* xtree_realloc(void* mem, size_t size)
{
	tPTR ptr = NULL;
	g_root->heapRealloc(&ptr, mem, size);
	return ptr;
}
static __inline void xtree_free(void* mem)
{
	g_root->heapFree(mem);
}

#elif defined(XTREE_MEM_CRT)
#include <malloc.h>
static __inline void* xtree_alloc(size_t size)
{
	return malloc(size);
}
static __inline void* xtree_realloc(void* mem, size_t size)
{
	return realloc(mem, size);
}
static __inline void xtree_free(void* mem)
{
	free(mem);
}

#else
#error "xtree allocator not defined"
#endif

typedef enum
{
    SBVT_NULL      = 0,
    SBVT_EMPTY     = 1,
    SBVT_STRING    = 2,
    SBVT_BOOL      = 3,
    SBVT_INT       = 4,
    SBVT_LONG      = 5,
    SBVT_DATE_TIME = 6,
    SBVT_DATE      = 7,
    SBVT_BINARY    = 8,
    SBVT_FLOAT     = 9,
    SBVT_DOUBLE    = 10,
    SBVT_PARAMS    = 11,
    SBVT_ARRAY     = 12,
} serbinval_t;

#define KLPAR_SIGN_SIZE 16
static const char c_szSignBIN[KLPAR_SIGN_SIZE]= { '<','!','-','-','K','L','P','A','R','B','I','N',' ','-','-','>' };

/*!
    Header for binary format
    1.  c_szSignBIN     16 bytes
    2.  offset of body  4  bytes
    3.  version         4  bytes
    4.  flags           4  bytes
    5.  body
*/

typedef struct _xmlbin_hdr {
    char c_szSignBIN[KLPAR_SIGN_SIZE]; // 16 bytes
    unsigned long body_offset; // 4  bytes
    unsigned long version;     // 4  bytes
    unsigned long flags;       // 4  bytes
} xmlbin_hdr;

typedef struct _xnode {
	wchar_t* name;
	pxnode parent;
	pxnode next;
	pxnode first_child;
	serbinval_t type;
	unsigned int data_size;
	void* pdata;
	union {
		int _int;
		__int64 _int64d;
		float _float;
		double _double;
	} data;
} *pxnode;

pxnode xtree_create_node(wchar_t* name, serbinval_t type);
void xtree_free_node(pxnode node);
bool xtree_load(unsigned char* data, unsigned int size, pxnode* root);

//#ifdef UNICODE
//#define xtree_load_file xtree_load_fileW
//#else
//#define xtree_load_file xtree_load_fileA
//#endif
//bool xtree_load_fileA(char* filename, pxnode* root);
//bool xtree_load_fileW(wchar_t* filename, pxnode* root);

pxnode xtree_get_node(pxnode root, wchar_t* path);
void * xtree_query_value_p(pxnode root, wchar_t* path, serbinval_t* ptype, size_t* pdata_size);
bool   xtree_query_value(pxnode root, wchar_t* path, serbinval_t* ptype, void* buffer, size_t buff_size, size_t* pdata_size);
pxnode xtree_find_node(pxnode root, wchar_t* name, bool recursively);
pxnode xtree_find_str_node(pxnode root, wchar_t* name, wchar_t* value, bool recursively);

typedef int (__cdecl *t_output_func)(void* output, char* format, ...);
void xtree_dump(t_output_func output_func, void* output, pxnode root, int level);
void xtree_dump_xml(t_output_func output_func, void* output, pxnode root, int level);

bool xtree_serialize(pxnode root, void** ppdata, size_t* psize);
void xtree_free_serialized(void* pdata);
bool xtree_deserialize(pxnode* root, void* pdata, size_t size);
void xtree_xor_buffer(void* buffer, size_t size);

extern int  (__cdecl *_pfKLCSPWD_UnprotectData)(const void* pProtectedData, size_t nProtectedData, void*& pData, size_t& nData);

#if !defined(_SS_NO_PRAGUE)
#include <pr_cpp.h>
bool   xtree_query_val(pxnode root, wchar_t* path, cStrObj& val);
bool   xtree_query_val(pxnode root, wchar_t* path, tBYTE& val);
bool   xtree_query_val(pxnode root, wchar_t* path, tWORD& val);
bool   xtree_query_val(pxnode root, wchar_t* path, tDWORD& val);
bool   xtree_query_val(pxnode root, wchar_t* path, tQWORD& val);
#endif //_SS_NO_PRAGUE


#endif // __WKS5_SS_IMPL__