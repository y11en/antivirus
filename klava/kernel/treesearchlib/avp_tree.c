// avp_tree.c
//
//

#include "avp_tree.h"

#define TREE_STATIC_ONLY
#define TREE_STATIC_API

#include "search.c"
#include "crc32.c"
#include "loadstat.c"
#include "rtnodes.c"
#include "treemain.c"


hAVP_TREE AVP_TREE_CALL AVP_Tree_Create (
		const AVP_Tree_Alloc * allocator,  // memory allocator
		const void *           cfg_data,   // configuration data
		unsigned int           cfg_size    // configuration data size
	)
{
	sRT_CTX * tree_ctx = 0;

	if (TREE_FAILED (TreeInit ((sHEAP_CTX *) allocator, &tree_ctx)))
		return 0;

	if (TREE_FAILED (TreeLoad_StaticOnMap (tree_ctx, (void *) cfg_data, cfg_size)))
	{
		TreeDone (tree_ctx);
		return 0;
	}

	return (hAVP_TREE) tree_ctx;
}

void AVP_TREE_CALL AVP_Tree_Destroy (
		hAVP_TREE h_tree
	)
{
	if (h_tree != 0)
	{
		sRT_CTX * tree_ctx = (sRT_CTX *) h_tree;
		TreeDone (tree_ctx);
	}
}

struct AVP_Tree_Callback_Parms
{
	AVP_Tree_Callback   callback;
	void *              callback_ctx;
	int                 retval;
};

static TREE_ERR _TREE_CALL sig_found_callback (
		const void  * ctx,
		tSIGNATURE_ID sig_id,
		int32_t       pos,
		uint64_t      lpos,
		uint32_t      sig_len
	)
{
	struct AVP_Tree_Callback_Parms * cb_parms = (struct AVP_Tree_Callback_Parms *) ctx;

	cb_parms->retval = cb_parms->callback (cb_parms->callback_ctx, sig_id, pos, sig_len);
	
	return (cb_parms->retval == 0) ? 0 : TREE_ECANCEL;
}

// zero return - search completed
// nonzero return - value from the callback, processing was canceled
int AVP_TREE_CALL AVP_Tree_Search (
		hAVP_TREE         h_tree,          // tree context
		const void *      data,            // data to search in
		unsigned int      data_size,       // size of data
		AVP_Tree_Callback callback,        // callback function
		void *            callback_ctx     // user context for callback function
	)
{
	struct AVP_Tree_Callback_Parms cb_parms;
	sSEARCHER_RT_CTX rt_ctx;

	if (h_tree == 0 || callback == 0 || data == 0 || data_size == 0)
		return 0;

	TreeInitSearcherContext ((sRT_CTX *) h_tree, 0, &rt_ctx);

	cb_parms.callback = callback;
	cb_parms.callback_ctx = callback_ctx;
	cb_parms.retval = 0;

	TreeSearchData (& rt_ctx, (const uint8_t *)data, data_size, 0, data_size, sig_found_callback, &cb_parms);

	ResetRTContext (& rt_ctx, rt_ctx.pHeapCtx);

	return cb_parms.retval;
}

