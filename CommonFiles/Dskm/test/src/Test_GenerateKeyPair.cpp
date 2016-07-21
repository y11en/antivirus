#include "stdafx.h"
#include "Test_GenerateKeyPair.h"
#include "FileUtils.h"

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_GenerateKeyPair_Common2(
	const char* working_folder,
	DskmLibWrapper<DefaultAllocator>& dskm_wrapper,
	ParList& objects, 
	ObjectType object_type, 
	const char_t* szObjRegFileName,
	bool bSpecialReg,
	std::vector<std::string>& errors
	)
{
	FolderGuard grd(working_folder, true);
	KeyPairAP pKeyPair(dskm_wrapper.GenerateKeyPair());
	dskm_wrapper.SignObjectsToRegFileByKey(
									objects, 
									object_type, 
									bSpecialReg, 
									pKeyPair->PrivateKey(), 
									szObjRegFileName
									);
	if (dskm_wrapper.CheckObjectsUsingHashRegFile(objects, object_type, szObjRegFileName))
	{
		return true;
	}
	else
	{
		errors.push_back("Check object failed");
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////
bool Test_GenerateKeyPair_SignRegFile(std::vector<std::string>& errors)
{
	const char_t* szWorkingFolder = "GenerateKeyPair_SignRegFile";
	const char_t* szKLMasterPrivate = "../master.prv";
	const char_t* szKLPublic = "kl_public.rgk";
	const char_t* szKLOperablePublic = "kl_operable_public.rgk";

	FolderGuard grd(szWorkingFolder, true);

	DeleteFile(szKLPublic);
	DeleteFile(szKLOperablePublic);

	DskmLibWrapper<DefaultAllocator> dskm_wrapper(true);
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
	// создаем операбельный ключ
	KeyPairAP pOperKeyPair(dskm_wrapper.GenerateKeyPair());
	dskm_wrapper.SaveKeyToKeyReg(
							KeyType::OperablePublic, 
							pOperKeyPair->PublicKey(), 
							1, 
							ParList(), 
							szKLOperablePublic
							);
	// подписываем операбельный ключ ключом ЛК
	dskm_wrapper.SignRegFileIntoItself(
							szKLOperablePublic, 
							ObjectType::KeyOperablePublic,
							pKLKeyPair->PrivateKey()
							);
	// проверяем подпись созданного ранее файла
	if (!dskm_wrapper.CheckRegFileUsingInsideSign(
										szKLOperablePublic, 
										ObjectType::KeyOperablePublic, 
										szKLPublic
										))
	{
		errors.push_back("Check correct object sign failed");
		return false;
	}

	// портим содержимое файла
	{
		char data[] = "\0\0";
		if (!WriteDataToFile("kl_operable_public.rgk", 10, data, 2))
		{
			errors.push_back("Writing data into file failed");
			return false;
		}
	}

	// проверяем подпись испорченного файла
	if (dskm_wrapper.CheckRegFileUsingInsideSign(
										szKLOperablePublic, 
										ObjectType::KeyOperablePublic, 
										szKLPublic
										))
	{
		errors.push_back("Check incorrect object sign returns ok");
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
bool Test_GenerateKeyPair_SomeCase(std::vector<std::string>& errors)
{
	DskmLibWrapper<DefaultAllocator> dskm_wrapper(true);
	char obj_data[] = "abcdefghijklmnopqrstu";
	ParList objects;
	objects.AddBufferedObject(1, obj_data, sizeof(obj_data) / sizeof(obj_data[0]));
	return Test_GenerateKeyPair_Common2(
								"GenerateKeyPair_SomeCase",
								dskm_wrapper, 
								objects, 
								1, 
								"somecase.rgo", 
								false, 
								errors
								);
}

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

