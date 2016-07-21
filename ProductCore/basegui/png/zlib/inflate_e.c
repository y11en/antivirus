#include "zutil.h"
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"
#include <prague.h>
#include <iface\i_root.h>
#include <iface\i_transformer.h>
#include <plugin\p_inflate.h>

extern hROOT g_root;

int ZEXPORT inflateInit_(strm, version, stream_size)
z_streamp strm;
const char *version;
int stream_size;
{
	memset(strm, 0, sizeof(z_stream));
	if( PR_FAIL(CALL_SYS_ObjectCreate(g_root, (hOBJECT*)&strm->state, IID_TRANSFORMER, PID_INFLATE, SUBTYPE_ANY)) )
		return Z_ERRNO;

	CALL_SYS_PropertySetDWord((hOBJECT)strm->state, plINFLATE_WINDOW_MULTIPIER, 1);
	
	if( PR_FAIL(CALL_SYS_ObjectCreateDone((hOBJECT)strm->state)) )
		return Z_ERRNO;
	return Z_OK;
}

int ZEXPORT inflate(strm, flush)
z_streamp strm;
int flush;
{
	tQWORD written = 0, readed = 0;
	tERROR err;

	if( !strm->state )
		return Z_ERRNO;
	
	CALL_SYS_PropertySetPtr((hOBJECT)strm->state,   pgINPUT_BUFF,       strm->next_in);
	CALL_SYS_PropertySetDWord((hOBJECT)strm->state, pgINPUT_BUFF_SIZE,  strm->avail_in);
	CALL_SYS_PropertySetPtr((hOBJECT)strm->state,   pgOUTPUT_BUFF,      strm->next_out);
	CALL_SYS_PropertySetDWord((hOBJECT)strm->state, pgOUTPUT_BUFF_SIZE, strm->avail_out);

	err = CALL_InflateTransformer_ProcessEx((hInflateTransformer)strm->state, &written, &readed);
	if( PR_FAIL(err) )
		return Z_ERRNO;
	
	strm->next_in   += readed;
	strm->avail_in  -= (uInt)readed;
	strm->next_out  += written;
	strm->avail_out -= (uInt)written;
	return err == errEOF ? Z_STREAM_END : Z_OK;
}

int ZEXPORT inflateEnd(strm)
z_streamp strm;
{
	if( strm->state )
		CALL_SYS_ObjectClose((hTRANSFORMER)strm->state);
	return Z_OK;
}

int ZEXPORT inflateReset(strm)
z_streamp strm;
{
	if( !strm->state )
		return Z_ERRNO;
	CALL_Transformer_Reset((hTRANSFORMER)strm->state, INFLATE_RESET_STATE);
	return Z_OK;
}
