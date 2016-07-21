// std_dbloader.cpp
//
// multi-file dbloader implementation
//

#include <klava/klav_utils.h> 
#include <klava/klavdb.h>
#include <klava/kdb_utils.h>

#include <string.h>

#ifdef _MSC_VER
# pragma intrinsic(memset,memcpy)
#endif

// mask to extract protection flags
#define FRAG_PROT_MASK  KLAV_MEM_PROT_MASK

// flag to indicate that fragment was loaded to VM
#define FRAG_F_VM       0x1000

////////////////////////////////////////////////////////////////
// Loader implementation

class MultiFile_DB_Loader_Impl
{
public:
	MultiFile_DB_Loader_Impl (KLAV_Alloc *alloc, KLAV_DB_Fragment_Loader *loader, KLAV_Virtual_Memory *vm);
	~MultiFile_DB_Loader_Impl ();

	void destroy ()
		{ KLAV_DELETE (this, m_allocator); }

	KLAV_ERR open (bool checkSignature);
	KLAV_ERR close ();

	KLAV_ERR load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags, KLAV_DB_Fragment *dbFragment);
	KLAV_ERR protect_fragment (KLAV_DB_Fragment *dbFragment, uint32_t new_prot);
	KLAV_ERR release_fragment (KLAV_DB_Fragment *dbFragment);
	uint32_t get_engine_parm (uint32_t id);

private:
	struct FragmentDesc;

	typedef KdbManifestKernelParm KernelParm;

	KLAV_Alloc *            m_allocator;
	KLAV_DB_Fragment_Loader *  m_fragmentLoader;
	KLAV_Virtual_Memory  *  m_vm;
	KernelParm *            m_kernelParmTable;
	uint32_t                m_kernelParmCount;
	FragmentDesc *          m_fragmentTable;
	uint32_t                m_fragmentCount;

	struct FragmentDesc
	{
		uint32_t         sectionType;
		uint32_t         fragmentID;
		uint32_t         flags;
		uint32_t         loadCount;  // 0 if not loaded
		uint8_t *        signature;  // KLAV_Alloc-allocated
		uint32_t         signatureLen;
		KLAV_DB_Fragment dbFragment;

		void clearData ()
		{
			flags = 0;
			dbFragment.clear ();
		}
	};

	KLAV_ERR load_kernel_parms (KdbManifestReader& mftReader);
	KLAV_ERR load_section_table (KdbManifestReader& mftReader, bool checkSignature);

	KLAV_ERR do_load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags,
			const uint8_t *signature, uint32_t signatureLen, KLAV_DB_Fragment *dbFragment);

	KLAV_ERR do_protect_fragment (FragmentDesc& fragment, uint32_t new_prot);
	KLAV_ERR do_release_fragment (FragmentDesc& fragment);
};

MultiFile_DB_Loader_Impl::MultiFile_DB_Loader_Impl (
		KLAV_Alloc *alloc,
		KLAV_DB_Fragment_Loader *loader,
		KLAV_Virtual_Memory *vm
	)
		: m_allocator (alloc), m_fragmentLoader (loader), m_vm (vm),
		  m_kernelParmTable (0), m_kernelParmCount (0),
		  m_fragmentTable (0), m_fragmentCount (0)
{
}

MultiFile_DB_Loader_Impl::~MultiFile_DB_Loader_Impl ()
{
	close ();
}

KLAV_ERR MultiFile_DB_Loader_Impl::open (bool checkSignature)
{
	if (m_fragmentCount != 0) return KLAV_EALREADY;

	// read manifest
	KLAV_DB_Fragment mftFragment;
	KLAV_ERR err = do_load_fragment (KDBID_MANIFEST, 0, KLAV_DBLOADER_FLAG_READ, 0, 0, &mftFragment);
	if (err != KLAV_OK) return err;

	// TODO: check manifest signature

	KdbManifestReader mftReader;
	err = mftReader.init (mftFragment.kdb_data, mftFragment.kdb_size);
	if (err != KLAV_OK) {
		m_fragmentLoader->release_fragment (&mftFragment);
		return err;
	}

	err = load_kernel_parms (mftReader);

	if (err == KLAV_OK)
	{
		err = load_section_table (mftReader, checkSignature);
	}

	m_fragmentLoader->release_fragment (&mftFragment);

	if (err != KLAV_OK)
	{
		close ();
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::load_kernel_parms (KdbManifestReader& mftReader)
{
	// first pass: calculate number of kernel parameters
	KdbManifestReader::KernelParmEnum parmEnum = mftReader.enumKernelParms ();
	KdbManifestReader::KernelParm parm;

	uint32_t parmCount = 0;
	for (;;++parmCount) {
		KLAV_ERR err = parmEnum.getNext(parm);
		if (err == KLAV_EOF) break;
		if (err != KLAV_OK) return err;
	}

	// second pass: allocate and read parameters
	if (parmCount != 0)
	{
		m_kernelParmTable = (KernelParm *)m_allocator->alloc (parmCount * sizeof (KernelParm));
		if (m_kernelParmTable == 0) return KLAV_ENOMEM;
		memset (m_kernelParmTable, 0, parmCount * sizeof (KernelParm));
	}

	parmEnum = mftReader.enumKernelParms ();

	for (m_kernelParmCount=0; m_kernelParmCount < parmCount; ++m_kernelParmCount)
	{
		if (parmEnum.getNext(parm) != KLAV_OK) return KLAV_EUNKNOWN;
		m_kernelParmTable [m_kernelParmCount] = parm;
	}

	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::load_section_table (KdbManifestReader& mftReader, bool checkSignature)
{
	// first pass: calculate number of fragments
	KdbManifestReader::SectionEnum sen = mftReader.enumSections ();
	KdbManifestReader::SectionInfo sectInfo;

	uint32_t fragmentCount = 0;
	for (;;++fragmentCount) {
		KLAV_ERR err = sen.getNextSection (sectInfo);
		if (err == KLAV_EOF) break;
		if (err != KLAV_OK) return err;
	}

	// second pass: allocate and read fragment table
	if (fragmentCount != 0)
	{
		m_fragmentTable = (FragmentDesc *)m_allocator->alloc (fragmentCount * sizeof (FragmentDesc));

		if (m_fragmentTable == 0)
			return KLAV_ENOMEM;

		::memset (m_fragmentTable, 0, fragmentCount * sizeof (FragmentDesc));
	}

	sen = mftReader.enumSections ();
	for (m_fragmentCount=0; m_fragmentCount<fragmentCount; ++m_fragmentCount)
	{
		if (sen.getNextSection (sectInfo) != KLAV_OK) return KLAV_EUNKNOWN;
		FragmentDesc& fragmentDesc = m_fragmentTable[m_fragmentCount];

		fragmentDesc.sectionType = sectInfo.sectionType;
		fragmentDesc.fragmentID = sectInfo.fragmentID;

		if (checkSignature)
		{
			if (sectInfo.signatureLen != 0)
			{
				fragmentDesc.signature = (uint8_t *)m_allocator->alloc (sectInfo.signatureLen);
				if (fragmentDesc.signature == 0) return KLAV_ENOMEM;

				memcpy (fragmentDesc.signature, sectInfo.signature, sectInfo.signatureLen);
				fragmentDesc.signatureLen = sectInfo.signatureLen;
			}
		}
	}

	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::close ()
{
	KLAV_ERR err = KLAV_OK;

	for (uint32_t i=0; i<m_fragmentCount; ++i)
	{
		FragmentDesc& fragment = m_fragmentTable[i];
		if (fragment.loadCount != 0)
		{
			KLAV_ERR e = do_release_fragment (fragment);
			if (e != KLAV_OK && err == KLAV_OK) err = e;
		}

		if (fragment.signature != 0)
		{
			m_allocator->free (fragment.signature);
		}

		fragment.signature = 0;
		fragment.signatureLen = 0;
	}

	if (m_fragmentTable != 0)
	{
		m_allocator->free (m_fragmentTable);
		m_fragmentTable = 0;
	}

	m_fragmentCount = 0;

	if (m_kernelParmTable != 0)
	{
		m_allocator->free (m_kernelParmTable);
		m_kernelParmTable = 0;
	}
	m_kernelParmCount = 0;

	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags, KLAV_DB_Fragment *dbFragment)
{
	if (flags == 0 || dbFragment == 0) return KLAV_EINVAL;
	FragmentDesc *fragmentDesc = 0;

	uint32_t idx = 0;
	for (;  idx < m_fragmentCount; ++idx) {	
		FragmentDesc& desc = m_fragmentTable[idx];
		if (desc.sectionType == sectionType && desc.fragmentID == fragmentID) {
			fragmentDesc = &desc;
			break;
		}
	}
	if (fragmentDesc == 0) return KLAV_ENOTFOUND; // no such fragment in database

	if (fragmentDesc->loadCount != 0)
	{
		// already loaded, check flags
		if (fragmentDesc->flags != flags) return KLAV_ECONFLICT;
	}
	else
	{
		KLAV_ERR err = do_load_fragment (sectionType, fragmentID, KLAV_DBLOADER_FLAG_READ,
							fragmentDesc->signature, fragmentDesc->signatureLen,
							&(fragmentDesc->dbFragment));
		if (err != KLAV_OK)
		{
			fragmentDesc->clearData ();

			// KLAV_ENOTFOUND can be returned only if fragment not found in table
			// otherwise it must be translated to some other error code
			if (err == KLAV_ENOTFOUND)
				err = KLAV_EREAD;

			return err;
		}
	
		fragmentDesc->flags = KLAV_DBLOADER_FLAG_READ;

		uint32_t new_prot = flags & FRAG_PROT_MASK;

		if (new_prot != flags)
		{
			// change protection attributes
			err = do_protect_fragment (*fragmentDesc, new_prot);
			if (err != KLAV_OK)
			{
				do_release_fragment (*fragmentDesc);
				return err;
			}
		}
	}

	// increment load count
	fragmentDesc->loadCount ++;

	dbFragment->kdb_handle = (void*)(uintptr_t)idx;
	dbFragment->kdb_data = fragmentDesc->dbFragment.kdb_data;
	dbFragment->kdb_size = fragmentDesc->dbFragment.kdb_size;

	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::release_fragment (KLAV_DB_Fragment *dbFragment)
{
	if (dbFragment == 0) return KLAV_EINVAL;

	uint32_t idx = (uint32_t)(uintptr_t)dbFragment->kdb_handle;
	if (idx >= m_fragmentCount) return KLAV_EINVAL;

	FragmentDesc& fragment = m_fragmentTable [idx];
	if (fragment.loadCount == 0) return KLAV_EALREADY;

	KLAV_ERR err = KLAV_OK;

	if (--(fragment.loadCount) == 0)
	{
		err = do_release_fragment (fragment);
	}

	dbFragment->clear ();
	return KLAV_OK;
}

KLAV_ERR MultiFile_DB_Loader_Impl::protect_fragment (KLAV_DB_Fragment *dbFragment, uint32_t new_prot)
{
	if (dbFragment == 0) return KLAV_EINVAL;

	uint32_t idx = (uint32_t)(uintptr_t)dbFragment->kdb_handle;
	if (idx >= m_fragmentCount) return KLAV_EINVAL;

	FragmentDesc& fragmentDesc = m_fragmentTable [idx];
	if (fragmentDesc.loadCount == 0) return KLAV_EINVAL;

	KLAV_ERR err = do_protect_fragment (fragmentDesc, new_prot);
	return err;
}

KLAV_ERR MultiFile_DB_Loader_Impl::do_load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags,
			const uint8_t *signature, uint32_t signatureLen, KLAV_DB_Fragment *dbFragment)
{
	// format fragment file name
	char namebuf [KDB_FILENAME_MAXSIZE];

	size_t namelen = KDBFormatFilename (sectionType, fragmentID, namebuf);
	if (namelen == 0) return KLAV_EINVAL;

	namebuf[namelen] = 0;

	KLAV_DB_Fragment_Desc fragmentDesc;
	fragmentDesc.filename = namebuf;
	fragmentDesc.section_type = sectionType;
	fragmentDesc.fragment_id = fragmentID;
	fragmentDesc.flags = flags;
	fragmentDesc.signature = signature;
	fragmentDesc.signature_len = signatureLen;

	KLAV_ERR err = m_fragmentLoader->load_fragment (fragmentDesc, dbFragment);

	return err;
}

uint32_t MultiFile_DB_Loader_Impl::get_engine_parm (uint32_t id)
{
	for (uint32_t i = 0; i < m_kernelParmCount; ++i)
	{
		const KernelParm& parm = m_kernelParmTable [i];
		if (parm.id == id) return parm.value;
	}
	return 0;
}

KLAV_ERR MultiFile_DB_Loader_Impl::do_release_fragment (FragmentDesc& fragment)
{
	KLAV_ERR err = KLAV_OK;

	// loaded to VM ?
	if ((fragment.flags & FRAG_F_VM) != 0)
	{
		err = m_vm->vm_free ((void *)fragment.dbFragment.kdb_data, fragment.dbFragment.kdb_size);
	}
	else
	{
		err = m_fragmentLoader->release_fragment (&(fragment.dbFragment));
	}

	fragment.clearData ();
	fragment.loadCount = 0;

	return err;
}

KLAV_ERR MultiFile_DB_Loader_Impl::do_protect_fragment (FragmentDesc& fragment, uint32_t new_prot)
{
	KLAV_ERR err = KLAV_OK;

	new_prot &= FRAG_PROT_MASK;

	if (new_prot == (fragment.flags & FRAG_PROT_MASK))
		return KLAV_OK;

	// already loaded to VM ?
	if ((fragment.flags & FRAG_F_VM) != 0)
	{
		err = m_vm->vm_protect ((void *)fragment.dbFragment.kdb_data, fragment.dbFragment.kdb_size, new_prot);
		if (err != KLAV_OK)
			return err;
	}
	else // copy to VM, release file
	{
		uint32_t frag_size = fragment.dbFragment.kdb_size;

		if (m_vm != 0 && frag_size != 0)
		{
			uint8_t * vmdata = 0;
			err = m_vm->vm_alloc ((void **)&vmdata, frag_size, new_prot, 0);
			if (err != KLAV_OK)
				return err;

			::memcpy (vmdata, fragment.dbFragment.kdb_data, frag_size);

			m_fragmentLoader->release_fragment (&(fragment.dbFragment));
			fragment.dbFragment.clear ();

			fragment.dbFragment.kdb_data = vmdata;
			fragment.dbFragment.kdb_size = frag_size;

			fragment.flags |= FRAG_F_VM;
		}
	}

	// update protection flags
	fragment.flags = (fragment.flags & ~FRAG_PROT_MASK) | new_prot;

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_MultiFile_DB_Loader::KLAV_MultiFile_DB_Loader ()
	: m_pimpl (0)
{
}

KLAV_MultiFile_DB_Loader::~KLAV_MultiFile_DB_Loader ()
{
	close ();
}

KLAV_ERR KLAV_MultiFile_DB_Loader::open (
			KLAV_Alloc *alloc,
			KLAV_DB_Fragment_Loader *loader,
			KLAV_Virtual_Memory *vm,
			bool check_signature
		)
{
	if (m_pimpl != 0) return KLAV_EALREADY;
	if (alloc == 0 || loader == 0) return KLAV_EINVAL;

	m_pimpl = KLAV_NEW (alloc) MultiFile_DB_Loader_Impl (alloc, loader, vm);
	if (m_pimpl == 0) return KLAV_ENOMEM;

	KLAV_ERR err = m_pimpl->open (check_signature);
	if (err != KLAV_OK) {
		m_pimpl->destroy ();
		m_pimpl = 0;
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_MultiFile_DB_Loader::close ()
{
	if (m_pimpl == 0) return KLAV_EALREADY;

	KLAV_ERR err = m_pimpl->close ();

	m_pimpl->destroy ();
	m_pimpl = 0;

	return err;
}

KLAV_ERR KLAV_CALL KLAV_MultiFile_DB_Loader::load_fragment (
		uint32_t sect_type,
		uint32_t frag_id,
		uint32_t flags,
		KLAV_DB_Fragment* frag
	)
{
	if (m_pimpl == 0)
		return KLAV_ENOINIT;

	return m_pimpl->load_fragment (sect_type, frag_id, flags, frag);
}

KLAV_ERR KLAV_CALL KLAV_MultiFile_DB_Loader::release_fragment (KLAV_DB_Fragment *frag)
{
	if (m_pimpl == 0)
		return KLAV_ENOINIT;

	return m_pimpl->release_fragment (frag);
}

KLAV_ERR KLAV_CALL KLAV_MultiFile_DB_Loader::protect_fragment (KLAV_DB_Fragment *frag, uint32_t new_prot)
{
	if (m_pimpl == 0)
		return KLAV_ENOINIT;

	return m_pimpl->protect_fragment (frag, new_prot);
}

uint32_t KLAV_CALL KLAV_MultiFile_DB_Loader::get_engine_parm (uint32_t id)
{
	if (m_pimpl == 0)
		return 0;

	return m_pimpl->get_engine_parm (id);
}

