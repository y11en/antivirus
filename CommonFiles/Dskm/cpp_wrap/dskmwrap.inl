/*
*/

#if !defined(__KL_DSKMWRAP_INL)
#define __KL_DSKMWRAP_INL

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline DskmLibWrapper<_A>::DskmLibWrapper(bool bInitUnderlayingLibraries)
	: m_hDSKM(::DSKM_InitLibrary(
						&Allocator::Allocate, 
						&Allocator::Deallocate, 
						(bInitUnderlayingLibraries ? 1 : 0)
						))
	, m_bOwnHandle(true)
	, m_bInitUnderlayingLibraries(bInitUnderlayingLibraries)
{
	if (!m_hDSKM)
		throw DskmLibWrapperException(DskmLibWrapperException::DskmLibInitFailed);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline DskmLibWrapper<_A>::DskmLibWrapper(HDSKM hDSKM)
	: m_hDSKM(hDSKM)
	, m_bOwnHandle(false)
	, m_bInitUnderlayingLibraries(false)
{
	if (!m_hDSKM)
		throw DskmLibWrapperException(DskmLibWrapperException::InvalidArgument);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline DskmLibWrapper<_A>::~DskmLibWrapper()
{
	if (m_bOwnHandle)
		::DSKM_DeInitLibrary(m_hDSKM, (m_bInitUnderlayingLibraries ? 1 : 0));
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
KeyPairAP DskmLibWrapper<_A>::GenerateKeyPair() const
{
	void* pPrivateKeyData = 0;
	AVP_dword dwPrivateKeySize = 0;
	void* pPublicKeyData = 0;
	AVP_dword dwPublicKeySize = 0;
	AVP_dword err = ::DSKM_GenerateKeyPair(
									m_hDSKM,
									&pPrivateKeyData,
									&dwPrivateKeySize,
									&pPublicKeyData,
									&dwPublicKeySize
									);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new KeyPair(pPrivateKeyData, dwPrivateKeySize, pPublicKeyData, dwPublicKeySize);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SaveKeyToKeyReg(
	KeyType::Values key_type, 
	const Key& key, 
	const ObjectType& obj_type, 
	const ParList& obj_params, 
	const char_t* pszKeyRegFileName
	) const
{
	AVP_dword err = ::DSKM_SaveKeyToKeyReg(
									m_hDSKM,
									obj_params.GetHandle(), 
									obj_type, 
									key_type, 
									const_cast<void*>(key.Data()),
									key.DataSize(), 
									pszKeyRegFileName
									);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::RemoveKeyFromKeyReg( 
	KeyType::Values key_type,
	const ObjectType& obj_type, 
	const ParList& obj_params, 
	const char_t* pszKeyRegFileName 
	) const
{
	AVP_dword err = ::DSKM_RemoveKeyFromKeyReg( 
										m_hDSKM,
										obj_params.GetHandle(), 
										obj_type, 
										key_type, 
										pszKeyRegFileName
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
HashAP DskmLibWrapper<_A>::HashObjectByName(const char_t* pszObjectFileName) const
{
	void* pHashData = 0;
	AVP_dword dwHashSize = 0;
	AVP_dword err = ::DSKM_HashObjectByName(
										m_hDSKM, 
										const_cast<char_t*>(pszObjectFileName),
										&pHashData, 
										&dwHashSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new Hash(pHashData, dwHashSize);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
HashAP DskmLibWrapper<_A>::HashObjectByBuffer(
	void* pvBuffer, 
	AVP_dword dwBufferSize, 
	pfnDSKM_GetBuffer_CallBack pfnCallBack, 
	void* pvCallBackParams
	) const
{
	void* pHashData = 0;
	AVP_dword dwHashSize = 0;
	AVP_dword err = ::DSKM_HashObjectByBuffer(
										m_hDSKM, 
										pvBuffer,
										dwBufferSize,
										pfnCallBack,
										pvCallBackParams,
										&pHashData, 
										&dwHashSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new Hash(pHashData, dwHashSize);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::HashObjectsToRegBuffer(
	const ParList& objects, 
	const ObjectType& obj_type, 
	bool bSpecialReg
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_HashObjectsToRegBuffer(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										bSpecialReg ? 1 : 0,
										&pBuffer, 
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignObjectsToRegFileByKey(
	const ParList& objects, 
	const ObjectType& obj_type, 
	bool bSpecialReg, 
	const Key& private_key,
	const char_t* pszObjsRegFileName
	) const
{
	AVP_dword err = ::DSKM_SignObjectsToRegFileByKey(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										bSpecialReg ? 1 : 0,
										const_cast<void*>(private_key.Data()), 
										private_key.DataSize(), 
										const_cast<char_t*>(pszObjsRegFileName)
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignObjectsToRegFileByKeyReg(
	const ParList& objects, 
	const ObjectType& obj_type, 
	bool bSpecialReg, 
	const char_t* pszKeyRegFileName, 
	const char_t* pszObjsRegFileName 
	) const
{
	AVP_dword err = ::DSKM_SignObjectsToRegFileByKeyReg(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										bSpecialReg ? 1 : 0,
										const_cast<char_t*>(pszKeyRegFileName), 
										const_cast<char_t*>(pszObjsRegFileName)
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::SignObjectsToRegBufferByKey(
	const ParList& objects, 
	const ObjectType& obj_type, 
	bool bSpecialReg, 
	const Key& private_key
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_SignObjectsToRegBufferByKey(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										bSpecialReg ? 1 : 0,
										const_cast<void*>(private_key.Data()), 
										private_key.DataSize(), 
										&pBuffer,
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::SignObjectsToRegBufferByKeyReg(
	const ParList& objects, 
	const ObjectType& obj_type, 
	bool bSpecialReg, 
	const char_t* pszKeyRegFileName
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_SignObjectsToRegBufferByKeyReg(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										bSpecialReg ? 1 : 0,
										const_cast<char_t*>(pszKeyRegFileName), 
										&pBuffer,
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignOneObjectToObjectByKey(
	ParList& objects, 
	const ObjectType& obj_type, 
	const Key& private_key
	) const
{
	AVP_dword err = ::DSKM_SignOneObjectToObjectByKey(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										const_cast<void*>(private_key.Data()),
										private_key.DataSize()
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignOneObjectToObjectByKeyReg( 
	ParList& objects, 
	const ObjectType& obj_type, 
	const char_t* pszKeyRegFileName 
	) const
{
	AVP_dword err = ::DSKM_SignOneObjectToObjectByKeyReg(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										const_cast<char_t*>(pszKeyRegFileName)
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::SignOneObjectToSignBufferByKey(
	const ParList& objects, 
	const ObjectType& obj_type, 
	const Key& private_key
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_SignOneObjectToSignBufferByKey(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										const_cast<void*>(private_key.Data()),
										private_key.DataSize(),
										&pBuffer,
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::SignOneObjectToSignBufferByKeyReg( 
	const ParList& objects, 
	const ObjectType& obj_type, 
	const char_t* pszKeyRegFileName
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_SignOneObjectToSignBufferByKeyReg(
										m_hDSKM, 
										objects.GetHandle(),
										obj_type,
										const_cast<char_t*>(pszKeyRegFileName),
										&pBuffer,
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
DataBufferAP DskmLibWrapper<_A>::SignOneObjectToBufferBySignBuffer( 
	const ParList& objects, 
	const DataBuffer& sign_buffer
	) const
{
	void* pBuffer = 0;
	AVP_dword dwBufferSize = 0;
	AVP_dword err = ::DSKM_SignOneObjectToObjectBySignBuffer(
										m_hDSKM, 
										objects.GetHandle(),
										const_cast<void*>(sign_buffer.Data()),
										sign_buffer.DataSize(),
										&pBuffer,
										&dwBufferSize
										);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
	return new DataBuffer(pBuffer, dwBufferSize, false);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignRegFileIntoItself(
	const char_t* reg_file_name,
	const ObjectType& obj_type,
	const Key& private_key
	)
{
	ParList list;
	list.AddNamedObject(0, reg_file_name);
	SignOneObjectToObjectByKey(list, obj_type, private_key);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SignRegFileIntoItself(
	const char_t* reg_file_name,
	const ObjectType& obj_type,
	const char_t* key_reg_file_name
	)
{
	ParList list;
	list.AddNamedObject(0, reg_file_name);
	SignOneObjectToObjectByKeyReg(list, obj_type, key_reg_file_name);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
bool DskmLibWrapper<_A>::HandleCheckObjectsFuncResult( 
	AVP_dword dwFuncResult,
	const ParList& objects
	)
{
	if (objects.DoesObjectErrorsExists())  // существует ошибочный объект
	{	
		return false;
	}
	else  // ошибочных объектов нет
	{
		if (DSKM_OK(dwFuncResult))
			return true;
		else
			throw DskmLibException(dwFuncResult);
	}
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingHashRegsFolder(
	ParList& objects, 
	const ObjectType& obj_type, 
	const char_t* pszRegFilesFolder 
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingHashRegsFolder(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type, 
												const_cast<char_t*>(pszRegFilesFolder)
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingHashRegFile( 
	ParList& objects, 
	const ObjectType& obj_type, 
	const char_t* pszRegFileName 
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingHashRegFile(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type, 
												const_cast<char_t*>(pszRegFileName)
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingHashRegsAssoc( 
	ParList& objects, 
	const ObjectType& obj_type 
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingHashRegsAssoc(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingInsideSignAndKeysFolder( 
	ParList& objects,
	const ObjectType& obj_type,
	const char_t* pszKeysFolder
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingInsideSignAndKeysFolder(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type,
												const_cast<char_t*>(pszKeysFolder)
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingInsideSignAndKeysAssoc(
	ParList& objects,
	const ObjectType& obj_type
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingInsideSignAndKeysAssoc(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckObjectsUsingInsideSignAndKeyReg(
	ParList& objects,
	const ObjectType& obj_type,
	const char_t* pszKeyRegFileName
	) const
{
	AVP_dword err = ::DSKM_CheckObjectsUsingInsideSignAndKeyReg(
												m_hDSKM, 
												objects.GetHandle(),
												obj_type,
												const_cast<char_t*>(pszKeyRegFileName)
												);
	return HandleCheckObjectsFuncResult(err, objects);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline bool DskmLibWrapper<_A>::CheckRegFileUsingInsideSign(
	const char_t* reg_file_name,
	const ObjectType& obj_type,
	const char_t* checking_by_key_reg_name
	)
{
	ParList list;
	list.AddNamedObject(0, reg_file_name);
	return CheckObjectsUsingInsideSignAndKeyReg(
										list, 
										obj_type, 
										checking_by_key_reg_name
										);
}

//////////////////////////////////////////////////////////////////////////
template <typename _A>
inline void DskmLibWrapper<_A>::SaveTypeAssociationToFile(
	const ObjectType& obj_type,
	const ParList& params,
	const char_t* pszAssocFileName
	)
{
	AVP_dword err = ::DSKM_SaveTypeAssociationToFile(  
												m_hDSKM, 
												params.GetHandle(), 
												obj_type, 
												const_cast<AVP_str>(pszAssocFileName)
												);
	if (DSKM_NOT_OK(err))
		throw DskmLibException(err);
}

}  // namespace DskmLibWrapper
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_INL)
