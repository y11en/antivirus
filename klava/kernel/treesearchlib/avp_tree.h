// avp_tree.h
//
//

#ifndef avp_tree_h_INCLUDED
#define avp_tree_h_INCLUDED

#include <dlink3.h>

#ifdef _M_IX86
#define AVP_TREE_CALL __cdecl
#else
#define AVP_TREE_CALL
#endif

// this structure must be kept binary compatible with sHEAP_CTX
typedef struct tag_AVP_Tree_Alloc
{
	void * p_heap;
	void * (AVP_TREE_CALL * alloc)  (void * p_heap, unsigned int size); // allocated memory must be zeroed
	void * (AVP_TREE_CALL * realloc)(void * p_heap, void * p_mem, unsigned int new_size);
	void   (AVP_TREE_CALL * free)   (void * p_heap, void * p_mem);
} AVP_Tree_Alloc;

typedef struct AVP_Tree * hAVP_TREE;

// nonzero return - cancel processing
typedef int (AVP_TREE_CALL * AVP_Tree_Callback)(
		void *       callback_ctx,  // user context
		unsigned int sig_id,        // signature ID
		int          sig_offset,    // offset of signature in buffer
		unsigned int sig_len        // signature length
	);

hAVP_TREE AVP_TREE_CALL AVP_Tree_Create (
		const AVP_Tree_Alloc * allocator,  // memory allocator
		const void *           cfg_data,   // configuration data
		unsigned int           cfg_size    // configuration data size
	);

void AVP_TREE_CALL AVP_Tree_Destroy (
		hAVP_TREE h_tree
	);

// zero return - search completed
// nonzero return - value from the callback, processing was canceled
int AVP_TREE_CALL AVP_Tree_Search (
		hAVP_TREE         h_tree,          // tree context
		const void *      data,            // data to search in
		unsigned int      data_size,       // size of data
		AVP_Tree_Callback callback,        // callback function
		void *            callback_ctx     // user context for callback function
	);

#endif // avp_tree_h_INCLUDED

