// pr_kfb_dbloader.cpp
//
// Prague-based single-file (KFB) runtime database loader
//

#include <klava/klav_prague.h>

#include <iface/i_buffer.h>
#include <plugin/p_filemapping.h>


KLAV_Pr_KFB_Loader::KLAV_Pr_KFB_Loader ()
	: m_hBuffer (0)
{
}

KLAV_Pr_KFB_Loader::~KLAV_Pr_KFB_Loader ()
{
}

KLAV_ERR KLAV_Pr_KFB_Loader::open (hOBJECT hParent, const char *filepath)
{
	if (m_hBuffer != 0) close ();

	hBUFFER hBuffer = 0;
	tERROR error = hParent->sysCreateObject((hOBJECT*)&hBuffer, IID_BUFFER, PID_FILEMAPPING);
	if (PR_FAIL (error)) return error;

	tDWORD mapFlags = fACCESS_READ;

	if (PR_SUCC (error))
	{
		error = hBuffer->propSetDWord(pgOBJECT_ACCESS_MODE, mapFlags);
	}

	if (PR_SUCC (error))
	{
		error = hBuffer->propSetStr (NULL, pgOBJECT_FULL_NAME, (char *)filepath, 0, KLAV_CP_UTF8);
	}

	if (PR_SUCC (error))
	{
		error = hBuffer->sysCreateObjectDone ();
	}

	tQWORD bufSize = 0;
	if (PR_SUCC (error))
	{
		error = hBuffer->GetSize (&bufSize, IO_SIZE_TYPE_EXPLICIT);
	}

	tPTR bufData = 0;
	if(PR_SUCC(error))
	{
		error = hBuffer->Lock(&bufData);
	}

	if (PR_FAIL (error))
	{
		hBuffer->sysCloseObject ();
		return error;
	}

	KLAV_ERR err = KLAV_KFB_Loader::open (bufData, (uint32_t)bufSize);
	if (err != KLAV_OK)
	{
		hBuffer->sysCloseObject ();
		return err;
	}

	m_hBuffer = (hOBJECT)hBuffer;

	return KLAV_OK;
}

KLAV_ERR KLAV_Pr_KFB_Loader::close ()
{
	if (m_hBuffer == 0) return KLAV_EALREADY;

	KLAV_ERR err = KLAV_KFB_Loader::close ();

	tERROR error = m_hBuffer->sysCloseObject ();
	if (err == KLAV_OK && PR_FAIL (error)) err = error;

	m_hBuffer = 0;

	return err;
}

