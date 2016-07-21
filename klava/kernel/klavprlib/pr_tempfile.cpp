// pr_temppfile.cpp
//
// Prague-based KLAV_Temp_Object_Manager implementation
//

#include <klava/klav_prague.h>

#include <plugin/p_nfio.h>
#include <pr_pid.h>

#ifdef _MSC_VER
# pragma intrinsic (memset)
# pragma intrinsic (memcpy)
# pragma intrinsic (strlen)
#endif // _MSC_VER

// TODO: remove this in favor of KLAV_Context_If property access
// Quick-and-dirty context definition

struct KLAV_Context
{
  void * slot[1]; // slot[]
};

#define KLAV_CONTEXT_SLOT_OBJECT(CTX) ((KLAV_Context *)(CTX)->slot [0x20])

////////////////////////////////////////////////////////////////
// Temp object manager

class KLAV_Pr_Temp_Object_Manager :
		public KLAV_IFACE_IMPL(KLAV_Temp_Object_Manager)
{
public:
	KLAV_Pr_Temp_Object_Manager (hOBJECT hparent);
	virtual ~KLAV_Pr_Temp_Object_Manager ();

	virtual KLAV_ERR KLAV_CALL create_temp_object (
				KLAV_CONTEXT_TYPE context,
				KLAV_Properties * props,
				KLAV_IO_Object **ptempobj
			);

	KLAV_Alloc * allocator ()
		{ return & m_allocator; }

private:
	hOBJECT       m_parent;
	KLAV_Pr_Alloc m_allocator;
};

////////////////////////////////////////////////////////////////

KLAV_Pr_Temp_Object_Manager::KLAV_Pr_Temp_Object_Manager (hOBJECT hparent)
	: m_parent (hparent), m_allocator (hparent)
{
}

KLAV_Pr_Temp_Object_Manager::~KLAV_Pr_Temp_Object_Manager ()
{
}

KLAV_ERR KLAV_CALL KLAV_Pr_Temp_Object_Manager::create_temp_object (
			KLAV_CONTEXT_TYPE ctx,
			KLAV_Properties * props,
			KLAV_IO_Object **ptempobj
		)
{
	*ptempobj = 0;

	hOBJECT parent = m_parent;
	KLAV_IO_Object* io_object = 0;
	
	// dirty hack
	KLAV_Context * ctx_impl = (KLAV_Context *) ctx;
	void ** ctx_vtbl = (void **) ctx_impl->slot [0];
	if (ctx_vtbl [3] == ctx_vtbl [0]) // proc 3,4 (property access) not implemented, old engine
	{
		// access context slot directly
		io_object = (KLAV_IO_Object *) KLAV_CONTEXT_SLOT_OBJECT(ctx);
	}
	else
	{
		// use context property access methods
		KLAV_Context_If *ctx_if = (KLAV_Context_If *) ctx;
		
		KLAV_Prop_Val val;
		ctx_if->get_property (KLAV_PROPID_OBJECT, 0, &val.value ());

		io_object = (KLAV_IO_Object *) val.value ().p;
	}

	if( io_object )
	{
		hOBJECT hobj = (hOBJECT)io_object->get_io_iface(KLAV_IFIO_PR_HOBJECT);
		if( hobj )
			parent = hobj;
	}

	hIO hio = 0;
	tERROR error = parent->sysCreateObjectQuick ((hOBJECT *)& hio, IID_IO, PID_TMPFILE, SUBTYPE_ANY);
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	KLAV_IO_Object *io_obj = 0;
	error = KLAV_Pr_Create_IO_Object (parent, hio, true, & io_obj);
	if (PR_FAIL (error))
	{
		hio->sysCloseObject ();
		return error;
	}

	*ptempobj = io_obj;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
hKLAV_Temp_Object_Manager KLAV_CALL
KLAV_Pr_Temp_Object_Manager_Create (hOBJECT parent)
{
	KLAV_Pr_Alloc allocator (parent);
	return KLAV_NEW (& allocator) KLAV_Pr_Temp_Object_Manager (parent);
}

KLAV_EXTERN_C
void KLAV_CALL
KLAV_Pr_Temp_Object_Manager_Destroy (hKLAV_Temp_Object_Manager h)
{
	if (h != 0)
	{
		KLAV_Pr_Temp_Object_Manager * impl = static_cast <KLAV_Pr_Temp_Object_Manager *> (h);
		KLAV_DELETE (impl, impl->allocator ());
	}
}

