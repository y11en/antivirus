#ifndef __pr_ser_v1_skip_h
#define __pr_ser_v1_skip_h

#include <Prague/prague.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_stream.h>
																		
// ---
struct cSkipFieldAfterV1_Serializer : public cCoolSerializer {
	virtual tERROR StreamDeserialize( tVOID* field, const cSerDescriptorField* field_dsc, cInStream* in ) const {
		if ( 1 == in->ver() )
			return warnFALSE;
		return errNOT_IMPLEMENTED;
	}
};


extern cSkipFieldAfterV1_Serializer g_SkipFieldAfterV1_Serializer;

#endif