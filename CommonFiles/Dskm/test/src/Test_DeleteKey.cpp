#include "stdafx.h"
#include "Test_DeleteKey.h"
#include "FileUtils.h"

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_DeleteKey_Main(std::vector<std::string>& errors)
{
	const char_t* szWorkingFolder = "DeleteKey_Main";
	const char_t* szAssoc = "main.ass";
	const char_t* szKLMasterPrivate = "../master.prv";
	const char_t* szKLPublic = "kl_public.rgk";
	const char_t* szKLOperablePublic = "kl_operable_public.rgk";
	const char_t* szObjHashReg1 = "objects1.rgo";
	const char_t* szObjHashReg2 = "objects2.rgo";

	FolderGuard grd(szWorkingFolder, true);

	DeleteFile(szAssoc);
	DeleteFile(szKLPublic);
	DeleteFile(szKLOperablePublic);
	DeleteFile(szObjHashReg1);
	DeleteFile(szObjHashReg2);

	DskmLibWrapper<DefaultAllocator> dskm_wrapper(true);

	// создаем файл ассоциаций
	{
		ParList list1;
		list1.AddParam(1, 1);
		list1.AddParam(2, 2);
		dskm_wrapper.SaveTypeAssociationToFile(1, list1, szAssoc);
		ParList list2;
		list2.AddParam(1, 3);
		dskm_wrapper.SaveTypeAssociationToFile(2, list2, szAssoc);
	}

	// создаем ключ ЛК
	KeyPairAP pKLKeyPair(dskm_wrapper.GenerateKeyPair());
	dskm_wrapper.SaveKeyToKeyReg(
							KeyType::KLPublic, 
							pKLKeyPair->PublicKey(), 
							ObjectType::KeyOperablePublic, 
							ParList(), 
							szKLPublic
							);
	// подписываем публичный ключ ЛК мастер-ключом
	dskm_wrapper.SignRegFileIntoItself(
							szKLPublic, 
							ObjectType::KeyKLPublic,
							szKLMasterPrivate
							);
	// создаем операбельный ключ (1)
	KeyPairAP pOperKeyPair1(dskm_wrapper.GenerateKeyPair());
	{
		ParList params;
		params.AddParam(1, 1);
		params.AddParam(2, 2);
		dskm_wrapper.SaveKeyToKeyReg(
								KeyType::OperablePublic, 
								pOperKeyPair1->PublicKey(), 
								1, 
								params, 
								szKLOperablePublic
								);
	}
	// создаем операбельный ключ (2)
	KeyPairAP pOperKeyPair2(dskm_wrapper.GenerateKeyPair());
	{
		ParList params;
		params.AddParam(3, 3);
		dskm_wrapper.SaveKeyToKeyReg(
								KeyType::OperablePublic, 
								pOperKeyPair2->PublicKey(), 
								2, 
								params, 
								szKLOperablePublic
								);
	}
	// подписываем рееестр операбельных ключей ключом ЛК
	dskm_wrapper.SignRegFileIntoItself(
							szKLOperablePublic,
							ObjectType::KeyOperablePublic,
							pKLKeyPair->PrivateKey()
							);

	// создаем объекты для проверки
	ParList object1, object2;
	{
		char buffer[] = "abcdefgh";
		object1.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object1.AddParam(1, 1);
		object1.AddParam(2, 2);
	}
	{
		char buffer[] = "abcdefghijklmno";
		object2.AddBufferedObject(2, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object2.AddParam(3, 3);
	}

	// создаем реестр хэшей объектов
	dskm_wrapper.SignObjectsToRegFileByKey(object1, 1, false, pOperKeyPair1->PrivateKey(), szObjHashReg1);
	dskm_wrapper.SignObjectsToRegFileByKey(object2, 2, false, pOperKeyPair2->PrivateKey(), szObjHashReg2);

	bool bResult = true;

	// проверяем объекты
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1, 1, "."))
	{
		errors.push_back("Object1 checking before key deletion failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2, 2, "."))
	{
		errors.push_back("Object2 checking before key deletion failed");
		bResult = false;
	}

	// удаляем второй операбельный ключ из реестра ключей
	{
		ParList params;
		params.AddParam(3, 3);
		dskm_wrapper.RemoveKeyFromKeyReg(KeyType::OperablePublic, 2, params, szKLOperablePublic);
	}

	// проверяем объекты (реестр операбельных ключей не подписан)
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1, 1, "."))
	{
		errors.push_back("Object1 checking return ok while operable key registry is not signed");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2, 2, "."))
	{
		errors.push_back("Object2 checking return ok while operable key registry is not signed");
		bResult = false;
	}

	// подписываем реестр операбельных ключей
	dskm_wrapper.SignRegFileIntoItself(
							szKLOperablePublic,
							ObjectType::KeyOperablePublic,
							pKLKeyPair->PrivateKey()
							);

	// проверяем объекты
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1, 1, "."))
	{
		errors.push_back("Object1 checking failed with modified operable key registry");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2, 2, "."))
	{
		errors.push_back("Object2 checking returns ok with modified operable key registry");
		bResult = false;
	}

	return bResult;
}

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

