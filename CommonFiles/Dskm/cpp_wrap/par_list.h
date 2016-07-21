/*
*/

#if !defined(__KL_DSKMWRAP_PARLIST_H)
#define __KL_DSKMWRAP_PARLIST_H

#include <dskm/dskm.h>
#include "types.h"
#include "exceptions.h"
#include "data_buffer.h"
#include "hash.h"
#include "utility.h"

#include <string>
#include <vector>

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class ParList
{
public:
	//////////////////////////////////////////////////////////////////////////
	ParList();
	ParList(HDSKMLIST hList, bool bAttach = true);
	~ParList();
	void Swap(ParList& list);
	//////////////////////////////////////////////////////////////////////////
	HDSKMLIST GetHandle() const;
	HDSKMLIST ReleaseHandle();
	//////////////////////////////////////////////////////////////////////////
	void AddParam(AVP_dword dwParamId, const char_t* pszStrParam);
	void AddParam(AVP_dword dwParamId, const string_t& sStrParam);
	void AddParam(AVP_dword dwParamId, AVP_dword dwParam);
	void AddParam(AVP_dword dwParamId, void *pvParamPtr, AVP_dword dwBinarySize);
	//////////////////////////////////////////////////////////////////////////
	void AddFileAssociation(AVP_dword dwObjectsType, const char_t* pszObjRegFileName, const char_t* pszKeyRegFileName);
	void AddObjectTypeAssociationFile(const char_t* pszAssocFileName);
	//////////////////////////////////////////////////////////////////////////
	void AddNamedObject(AVP_dword dwObjectId, const char_t* pszObjectName);
	void AddBufferedObject(
				AVP_dword dwObjectId, 
				void* pvBuffer,
				AVP_dword dwBufferSize,
				pfnDSKM_GetBuffer_CallBack pfnCallBack = 0,
				void* pvCallBackParams = 0
				);
	void AddObjectHash(AVP_dword dwObjectId, const Hash& hash);
	//////////////////////////////////////////////////////////////////////////
	AVP_dword GetFirstObjectError(AVP_dword& err_code) const;
	AVP_dword GetNextObjectError(AVP_dword object_id, AVP_dword& err_code) const;
	bool DoesObjectErrorsExists() const;
	void GetAllObjectErrors(std::vector<std::pair<AVP_dword, AVP_dword> >& vecObjectErrors) const;
	//////////////////////////////////////////////////////////////////////////
	DataBufferAP GetObject(AVP_dword object_id) const;
	void GetObject(AVP_dword object_id, AVP_dword& value) const;
	void GetObject(AVP_dword object_id, string_t& value) const;
private:
	ParList(const ParList&);
	ParList& operator = (const ParList&);
private:
	static void HandleDskmError(AVP_dword err);
private:
	HDSKMLIST m_hList;
	bool	  m_bOwnHandle;
};

}  // namespace DSKM
}  // namespace KL

#include "par_list.inl"

#endif  // !defined(__KL_DSKMWRAP_PARLIST_H)
