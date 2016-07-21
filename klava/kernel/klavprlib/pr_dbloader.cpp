// pr_dbloader.cpp
//
// Prague-based database loader
//

#include <klav_prague.h>

#include <iface/i_buffer.h>
#include <plugin/p_filemapping.h>

#include <string.h>

#ifdef _MSC_VER
# pragma intrinsic(strlen,memcpy)
#endif
/*
KlavPragueDBFragmentLoader::KlavPragueDBFragmentLoader ()
	: m_hParent (0), m_dbFolder (0), m_dbPrefix (0), m_dbSuffix (0), m_namebuf (0), m_namebuflen (0)
{
}

KlavPragueDBFragmentLoader::~KlavPragueDBFragmentLoader ()
{
	deinit ();
}

KLAV_ERR KlavPragueDBFragmentLoader::init (hOBJECT hParent, const char *folder, const char *prefix, const char *suffix)
{
	if (hParent == 0) return KLAV_EINVAL;
	deinit ();

	m_allocator.setObject (hParent);
	m_hParent = hParent;
	if (folder == 0) folder = "";
	if (prefix == 0) prefix = "";
	if (suffix == 0) suffix = "";

	m_dbFolder = folder;
	m_dbPrefix = prefix;
	m_dbSuffix = suffix;

	uint32_t len = (uint32_t)(strlen (m_dbFolder) + 1 + strlen (m_dbPrefix) + strlen (m_dbSuffix) + 16);

	if (! allocbuf (len))
	{
		deinit ();
		return KLAV_ENOMEM;
	}

	m_namebuflen = len;

	return KLAV_OK;
}

void KlavPragueDBFragmentLoader::deinit ()
{
	freebuf ();
	m_dbFolder = 0;
	m_dbPrefix = 0;
	m_dbSuffix = 0;
	m_hParent = 0;
	m_allocator.setObject (0);
}

bool KlavPragueDBFragmentLoader::allocbuf (uint32_t len)
{
	if (m_namebuflen >= len) return true;

	char *nbuf = (char *)m_allocator.alloc (len);
	if (nbuf == 0) return false;

	if (m_namebuf != 0)
	{
		memcpy (nbuf, m_namebuf, m_namebuflen);
		freebuf ();
	}

	m_namebuf = nbuf;
	m_namebuflen = len;
	return true;
}

void KlavPragueDBFragmentLoader::freebuf ()
{
	if (m_namebuf != 0)
	{
		m_allocator.free (m_namebuf);
		m_namebuf = 0;
	}
	m_namebuflen = 0;
}

KLAV_ERR KLAV_CALL KlavPragueDBFragmentLoader::loadFragment (
			const KlavDBFragmentDesc& fragmentDesc,
			KLAV_DB_Fragment *dbFragment
		)
{
	uint32_t len = (uint32_t)(strlen (m_dbFolder) + 1 + strlen (m_dbPrefix) + strlen (m_dbSuffix) + strlen (fragmentDesc.filename));
	if (! allocbuf (len)) return KLAV_ENOMEM;

	char *p = m_namebuf;

	if (m_dbFolder[0] != 0) {
		strcpy (p, m_dbFolder); p += strlen (p);
		*p++ = '/';
	}
	strcpy (p, m_dbPrefix); p += strlen (p);
	strcpy (p, fragmentDesc.filename); p += strlen (p);
	strcpy (p, m_dbSuffix);


	hBUFFER hBuffer = 0;
	tERROR error = m_hParent->sysCreateObject((hOBJECT*)&hBuffer, IID_BUFFER, PID_FILEMAPPING);
	if (PR_FAIL (error)) return KLAV_EUNKNOWN; // TODO: map Prague error code

	tDWORD mapFlags = 0;
	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_READ) != 0) mapFlags |= fACCESS_READ;
	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_WRITE) != 0) mapFlags |= fACCESS_COPY_ON_WRITE;
//	TODO: fragments with EXECUTE permissions must be copied to allocated memory blocks
//	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_EXECUTE) != 0) mapFlags |= fACCESS_EXECUTE;

	if (PR_SUCC (error)) {
		error = hBuffer->propSetDWord(pgOBJECT_ACCESS_MODE, mapFlags);
	}

	if (PR_SUCC (error)) {
		error = hBuffer->propSetStr (NULL, pgOBJECT_FULL_NAME, m_namebuf, 0, KLAV_CP_UTF8);
	}

	if (PR_SUCC (error)) {
		error = hBuffer->sysCreateObjectDone ();
	}

	tQWORD bufSize = 0;
	if (PR_SUCC (error)) {
		error = hBuffer->GetSize (&bufSize, IO_SIZE_TYPE_EXPLICIT);
	}

	tPTR bufData = 0;
	if(PR_SUCC(error)) {
		error = hBuffer->Lock(&bufData);
	}

	if (PR_FAIL (error)) {
		hBuffer->sysCloseObject ();
		return KLAV_EUNKNOWN;
	}

	dbFragment->kdb_handle = (hKLAV_OBJECT)hBuffer;
	dbFragment->kdb_data = (uint8_t *)bufData;
	dbFragment->kdb_size = (uint32_t)bufSize;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KlavPragueDBFragmentLoader::releaseFragment (
			const KLAV_DB_Fragment *dbFragment
		)
{
	hBUFFER hBuffer = (hBUFFER)(dbFragment->kdb_handle);
	if (hBuffer == 0) return KLAV_EINVAL;

	KLAV_ERR err = KLAV_OK;

	tERROR error = hBuffer->sysCloseObject ();
	if (PR_FAIL (error)) err = KLAV_EUNKNOWN;

	return err;
}
*/
