/*
*/

#if !defined(__KL_DSKMWRAP_PARLIST_INL)
#define __KL_DSKMWRAP_PARLIST_INL

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
inline ParList::ParList()
	: m_hList(0)
	, m_bOwnHandle(true)
{
	AVP_dword err = ::DSKM_ParList_Create(&m_hList);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
inline ParList::ParList(HDSKMLIST hList, bool bAttach)
	: m_hList(hList)
	, m_bOwnHandle(bAttach)
{
	if (!m_hList)
		throw DskmLibWrapperException(DskmLibWrapperException::InvalidArgument);
}

//////////////////////////////////////////////////////////////////////////
inline ParList::~ParList()
{
	if (m_hList && m_bOwnHandle)
		::DSKM_ParList_Delete(m_hList);
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::Swap(ParList& list)  
{ 
	std::swap(m_hList, list.m_hList); 
	std::swap(m_bOwnHandle, list.m_bOwnHandle); 
}

//////////////////////////////////////////////////////////////////////////
inline HDSKMLIST ParList::GetHandle() const  
{ 
	return m_hList; 
}

//////////////////////////////////////////////////////////////////////////
inline HDSKMLIST ParList::ReleaseHandle()  
{ 
	HDSKMLIST hList = m_hList;
	m_hList = 0;
	return hList;
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddParam(AVP_dword dwParamId, const char_t* pszStrParam)
{ 
	HandleDskmError(::DSKM_ParList_AddStrParam(m_hList, dwParamId, const_cast<AVP_str>(pszStrParam))); 
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddParam(AVP_dword dwParamId, const string_t& sStrParam)
{
	AddParam(dwParamId, const_cast<char*>(sStrParam.c_str()));
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddParam(AVP_dword dwParamId, AVP_dword dwParam)
{ 
	HandleDskmError(::DSKM_ParList_AddDWordParam(m_hList, dwParamId, dwParam)); 
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddParam(AVP_dword dwParamId, void *pvParamPtr, AVP_dword dwBinarySize)
{ 
	HandleDskmError(::DSKM_ParList_AddBinaryPtrParam(m_hList, dwParamId, pvParamPtr, dwBinarySize)); 
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddFileAssociation(
	AVP_dword dwObjectsType, 
	const char_t* pszObjRegFileName, 
	const char_t* pszKeyRegFileName
	)
{
	HandleDskmError(
		::DSKM_ParList_AddFileAssociation(
								m_hList, 
								dwObjectsType, 
								pszObjRegFileName, 
								Utility::get_string_buffer_size(pszObjRegFileName),
								pszKeyRegFileName, 
								Utility::get_string_buffer_size(pszKeyRegFileName)
								)
		); 
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddObjectTypeAssociationFile(const char_t* pszAssocFileName)
{ 
	HandleDskmError(
		::DSKM_ParList_AddObjectTypeAssociationFile(
								m_hList, 
								pszAssocFileName,
								Utility::get_string_buffer_size(pszAssocFileName)
								)
		);
}	

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddNamedObject(AVP_dword dwObjectId, const char_t* pszObjectName)
{ 
	HandleDskmError(
		::DSKM_ParList_AddNamedObject(
								m_hList, 
								dwObjectId, 
								pszObjectName, 
								Utility::get_string_buffer_size(pszObjectName)
								)
		);
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddBufferedObject(
	AVP_dword dwObjectId, 
	void* pvBuffer,
	AVP_dword dwBufferSize,
	pfnDSKM_GetBuffer_CallBack pfnCallBack,
	void* pvCallBackParams
	)
{ 
	HandleDskmError(::DSKM_ParList_AddBufferedObject(m_hList, dwObjectId, pvBuffer, dwBufferSize, pfnCallBack, pvCallBackParams));
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::AddObjectHash(AVP_dword dwObjectId, const Hash& hash)
{ 
	HandleDskmError(::DSKM_ParList_AddObjectHash(m_hList, dwObjectId, const_cast<void*>(hash.Data()), hash.DataSize()));
}

//////////////////////////////////////////////////////////////////////////
inline AVP_dword ParList::GetFirstObjectError(AVP_dword& err_code) const
{
	return ::DSKM_ParList_GetFirstObjectError(m_hList, &err_code);
}

//////////////////////////////////////////////////////////////////////////
inline AVP_dword ParList::GetNextObjectError(AVP_dword object_id, AVP_dword& err_code) const
{
	return ::DSKM_ParList_GetNextObjectError(m_hList, object_id, &err_code);
}

//////////////////////////////////////////////////////////////////////////
inline bool ParList::DoesObjectErrorsExists() const
{
	AVP_dword err_code = 0;
	return (GetFirstObjectError(err_code) != 0);
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::GetAllObjectErrors(
	std::vector<std::pair<AVP_dword, AVP_dword> >& vecObjectErrors
	) const
{
	std::vector<std::pair<AVP_dword, AVP_dword> > vecTemp;
	AVP_dword err_code = 0;
	AVP_dword object_id = GetFirstObjectError(err_code);
	while (object_id)
	{
		vecTemp.push_back(std::pair<AVP_dword, AVP_dword>(object_id, err_code));
		object_id = GetNextObjectError(object_id, err_code);
	}
	vecTemp.swap(vecObjectErrors);
}

//////////////////////////////////////////////////////////////////////////
inline DataBufferAP ParList::GetObject(AVP_dword object_id) const
{
	void* pBuffer = 0;
	AVP_dword nBufferSize = 0;
	HandleDskmError(::DSKM_ParList_GetObject(m_hList, object_id, &pBuffer, &nBufferSize));
	return new DataBuffer(pBuffer, nBufferSize);
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::GetObject(AVP_dword object_id, AVP_dword& value) const
{
	DataBufferAP pBuffer(GetObject(object_id));
	if (pBuffer->DataSize() != sizeof(AVP_dword))
		throw DskmLibWrapperException(DskmLibWrapperException::InvalidValueCast);
	value = *(static_cast<const AVP_dword*>(pBuffer->Data()));
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::GetObject(AVP_dword object_id, string_t& value) const
{
	DataBufferAP pBuffer(GetObject(object_id));
	AVP_dword size = pBuffer->DataSize();
	if (size > 0)
	{
		const char* begin = static_cast<AVP_str>(const_cast<void*>(pBuffer->Data()));  // сложные преобразовани€ дл€ обеспечени€ проверки типов AVP_str и char* на совместимость во врем€ компил€ции
		string_t(begin, begin + size).swap(value);
	}
	else
	{
		string_t().swap(value);
	}
}

//////////////////////////////////////////////////////////////////////////
inline void ParList::HandleDskmError(AVP_dword err)
{
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_PARLIST_INL)
