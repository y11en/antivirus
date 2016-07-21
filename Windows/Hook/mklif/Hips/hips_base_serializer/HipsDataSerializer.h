#ifndef _HIPS_DATA_SERIALIZER_
#define _HIPS_DATA_SERIALIZER_


#include <prague.h>
#include <pr_serializable.h>
#include <pr_vector.h>
#include "CPrague.h"
#include <plugin/p_propertiesmap.h>
#include "hips_structs.h"

#define HIPS_SER_PARAM_TYPE_STR "ParamType"
#define HIPS_SER_PARAM_VALUE_NUM_STR "ParamValueNum"
#define HIPS_SER_PARAM_VALUE_STR_STR "ParamValueStr"

#define HIPS_SER_RES_ID	"ResID"
#define HIPS_SER_RES_TYPE "ResType"
#define HIPS_SER_RES_DESC "ResDesc"
#define HIPS_SER_PARAM_LIST "ParamList"

class CHipsDataSerializer
{
public:
	CHipsDataSerializer(void);
	~CHipsDataSerializer(void);
private:
	bool SerializeParam(hREGISTRY pReg, tRegKey hKey, CHipsResourceParam * pParam, tDWORD ParamID);
	bool SerializeResource(hREGISTRY pReg, tRegKey hKey, CHipsResourceItem * pItem);
};

#endif //_HIPS_DATA_SERIALIZER_