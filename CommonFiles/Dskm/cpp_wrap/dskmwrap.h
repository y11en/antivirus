/*
*/

#if !defined(__KL_DSKMWRAP_H)
#define __KL_DSKMWRAP_H

#include <dskm/dskm.h>
#include "types.h"
#include "allocators.h"
#include "exceptions.h"
#include "data_buffer.h"
#include "key.h"
#include "hash.h"
#include "par_list.h"

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
template <typename _A = DefaultAllocator>
class DskmLibWrapper
{
public:

	typedef _A Allocator;

public:
	
	//////////////////////////////////////////////////////////////////////////
	DskmLibWrapper(bool bInitUnderlayingLibraries = false);
	DskmLibWrapper(HDSKM hDSKM);
	~DskmLibWrapper();

	//////////////////////////////////////////////////////////////////////////
	KeyPairAP GenerateKeyPair() const;
	void SaveKeyToKeyReg(
					KeyType::Values key_type, 
					const Key& key, 
					const ObjectType& obj_type, 
					const ParList& obj_params, 
					const char_t* pszKeyRegFileName
					) const;
	void RemoveKeyFromKeyReg( 
					KeyType::Values key_type,
					const ObjectType& obj_type, 
					const ParList& obj_params, 
					const char_t* pszKeyRegFileName 
					) const;

	//////////////////////////////////////////////////////////////////////////
	HashAP HashObjectByName(const char_t* pszObjectFileName) const;
	HashAP HashObjectByBuffer(
						void* pvBuffer, 
						AVP_dword dwBufferSize, 
						pfnDSKM_GetBuffer_CallBack pfnCallBack = 0, 
						void* pvCallBackParams = 0
						) const;
	DataBufferAP HashObjectsToRegBuffer(
									const ParList& objects, 
									const ObjectType& obj_type, 
									bool bSpecialReg
									) const;

	//////////////////////////////////////////////////////////////////////////
	void SignObjectsToRegFileByKey(
						const ParList& objects, 
						const ObjectType& obj_type, 
						bool bSpecialReg, 
						const Key& private_key,
						const char_t* pszObjsRegFileName
						) const;
	void SignObjectsToRegFileByKeyReg(
						const ParList& objects, 
						const ObjectType& obj_type, 
						bool bSpecialReg, 
						const char_t* pszKeyRegFileName, 
						const char_t* pszObjsRegFileName 
						) const;
	DataBufferAP SignObjectsToRegBufferByKey(
						const ParList& objects, 
						const ObjectType& obj_type, 
						bool bSpecialReg, 
						const Key& private_key
						) const;
	DataBufferAP SignObjectsToRegBufferByKeyReg(
						const ParList& objects, 
						const ObjectType& obj_type, 
						bool bSpecialReg, 
						const char_t* pszKeyRegFileName
						) const;

	//////////////////////////////////////////////////////////////////////////
	void SignOneObjectToObjectByKey(
						ParList& objects, 
						const ObjectType& obj_type, 
						const Key& private_key
						) const;
	void SignOneObjectToObjectByKeyReg( 
						ParList& objects, 
						const ObjectType& obj_type, 
						const char_t* pszKeyRegFileName 
						) const;
	DataBufferAP SignOneObjectToSignBufferByKey(
						const ParList& objects, 
						const ObjectType& obj_type, 
						const Key& private_key
						) const;
	DataBufferAP SignOneObjectToSignBufferByKeyReg( 
						const ParList& objects, 
						const ObjectType& obj_type, 
						const char_t* pszKeyRegFileName
						) const;
	DataBufferAP SignOneObjectToBufferBySignBuffer( 
						const ParList& objects, 
						const DataBuffer& sign_buffer
						) const;

	//////////////////////////////////////////////////////////////////////////
	void SignRegFileIntoItself(
					const char_t* reg_file_name,
					const ObjectType& obj_type,
					const Key& private_key
					);
	void SignRegFileIntoItself(
					const char_t* reg_file_name,
					const ObjectType& obj_type,
					const char_t* key_reg_file_name
					);

	//////////////////////////////////////////////////////////////////////////
	bool CheckObjectsUsingHashRegsFolder(
						ParList& objects, 
						const ObjectType& obj_type, 
						const char_t* pszRegFilesFolder 
						) const;
	bool CheckObjectsUsingHashRegFile( 
						ParList& objects, 
						const ObjectType& obj_type, 
						const char_t* pszRegFileName 
						) const;
	bool CheckObjectsUsingHashRegsAssoc( 
						ParList& objects, 
						const ObjectType& obj_type 
						) const;
	bool CheckObjectsUsingInsideSignAndKeysFolder( 
						ParList& objects,
						const ObjectType& obj_type,
						const char_t* pszKeysFolder
						) const;
	bool CheckObjectsUsingInsideSignAndKeysAssoc(
						ParList& objects,
						const ObjectType& obj_type
						) const;
	bool CheckObjectsUsingInsideSignAndKeyReg(
						ParList& objects,
						const ObjectType& obj_type,
						const char_t* pszKeyRegFileName
						) const;

	//////////////////////////////////////////////////////////////////////////
	bool CheckRegFileUsingInsideSign(
						const char_t* reg_file_name,
						const ObjectType& obj_type,
						const char_t* checking_by_key_reg_name
						);
//	нет поддержки в dskm.lib
//	bool CheckRegUsingInsideSign(
//						const char_t* reg_name,
//						const ObjectType& obj_type,
//						const Key& public_key
//						);

	//////////////////////////////////////////////////////////////////////////
	void SaveTypeAssociationToFile(
						const ObjectType& obj_type,
						const ParList& params,
						const char_t* pszAssocFileName
						);

private:

	static bool HandleCheckObjectsFuncResult(
								AVP_dword dwFuncResult, 
								const ParList& objects
								);

private:

	DskmLibWrapper(const DskmLibWrapper&);
	DskmLibWrapper& operator = (const DskmLibWrapper&);

private:

	HDSKM m_hDSKM;
	bool  m_bOwnHandle;
	bool  m_bInitUnderlayingLibraries;

};

}  // namespace DSKM
}  // namespace KL

#include "dskmwrap.inl"

#endif  // !defined(__KL_DSKMWRAP_H)
