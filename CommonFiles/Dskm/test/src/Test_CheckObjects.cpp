#include "stdafx.h"
#include "Test_CheckObjects.h"
#include "FileUtils.h"

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

////////////////////////////////////////////////////////////////////////////
bool Test_CheckObjects_Main(std::vector<std::string>& errors)
{
	const char_t* szWorkingFolder = "CheckObjects_Main";
	const char_t* szAssoc = "main.ass";
	const char_t* szKLMasterPrivate = "../master.prv";
	const char_t* szKLPublic = "kl_public.rgk";
	const char_t* szKLOperablePublic = "kl_operable_public.rgk";
	const char_t* szObjHashReg1 = "objects1.rgo";
	const char_t* szObjHashReg1_special = "objects1_special.rgo";
	const char_t* szObjHashReg2 = "objects2.rgo";
	const char_t* szLicenseKey = "license.key";

	FolderGuard grd(szWorkingFolder, true);

	DeleteFile(szAssoc);
	DeleteFile(szKLPublic);
	DeleteFile(szKLOperablePublic);
	DeleteFile(szObjHashReg1);
	DeleteFile(szObjHashReg1_special);
	DeleteFile(szObjHashReg2);
	DeleteFile(szLicenseKey);

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
	// подписываем рееестр операльных ключей ключом ЛК
	dskm_wrapper.SignRegFileIntoItself(
							szKLOperablePublic,
							ObjectType::KeyOperablePublic,
							pKLKeyPair->PrivateKey()
							);

	// создаем объекты для проверки
	ParList object1_ok, 
			object1_ok_special,
			object1_ok_hash,
			object1_invalid,
			object2_ok, 
			object2_invalid,
			object2_invalid_param_value, 
			object2_missed_params, 
			object2_another_params,
			object1_partial_params,
			object1_partial_invalid_params;
	{
		char buffer[] = "abcdefgh";
		object1_ok.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object1_ok.AddParam(1, 1);
		object1_ok.AddParam(2, 2);
		{
			HashAP pHash = dskm_wrapper.HashObjectByBuffer(buffer, sizeof(buffer) / sizeof(buffer[0]));
			object1_ok_hash.AddObjectHash(1, *(pHash.get()));
			object1_ok_hash.AddParam(1, 1);
			object1_ok_hash.AddParam(2, 2);
		}
		object1_invalid.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]) - 1);
		object1_invalid.AddParam(1, 1);
		object1_invalid.AddParam(2, 2);
		object1_partial_params.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object1_partial_params.AddParam(1, 1);
		object1_partial_invalid_params.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object1_partial_invalid_params.AddParam(1, 1);
		object1_partial_invalid_params.AddParam(2, 1);
	}
	{
		char buffer[] = "ABCDEFGH";
		object1_ok_special.AddBufferedObject(1, buffer, sizeof(buffer) / sizeof(buffer[0]));
	}
	{
		char buffer[] = "abcdefghijklmno";
		object2_ok.AddBufferedObject(2, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object2_ok.AddParam(3, 3);
		object2_invalid.AddBufferedObject(2, &buffer[0] + 1, sizeof(buffer) / sizeof(buffer[0]) - 1);
		object2_invalid.AddParam(3, 3);
		object2_invalid_param_value.AddBufferedObject(2, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object2_invalid_param_value.AddParam(3, 4);
		object2_missed_params.AddBufferedObject(2, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object2_another_params.AddBufferedObject(2, buffer, sizeof(buffer) / sizeof(buffer[0]));
		object2_another_params.AddParam(5, 5);
	}

	// создаем реестр хэшей объектов
	dskm_wrapper.SignObjectsToRegFileByKey(object1_ok, 1, false, pOperKeyPair1->PrivateKey(), szObjHashReg1);
	dskm_wrapper.SignObjectsToRegFileByKey(object1_ok_special, 1, true, pOperKeyPair1->PrivateKey(), szObjHashReg1_special);
	dskm_wrapper.SignObjectsToRegFileByKey(object2_ok, 2, false, pOperKeyPair2->PrivateKey(), szObjHashReg2);

	// создаем файл псевдо-лицензионного ключа
	{
		char buf[] = "License key file";
		CreateFileAndWriteData(szLicenseKey, buf, sizeof(buf) / sizeof(buf[0]));
	}

	// подписываем файл псевдо-лицензионного ключа
	dskm_wrapper.SignRegFileIntoItself(
							szLicenseKey,
							ObjectType::LicenseKey,
							pOperKeyPair1->PrivateKey()
							);

	bool bResult = true;

	// проверяем объекты
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok, 1, "."))
	{
		errors.push_back("Object1 checking failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok_special, 1, "."))
	{
		errors.push_back("Special Object1 checking failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok_hash, 1, "."))
	{
		errors.push_back("Object1 hash checking failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_ok, 2, "."))
	{
		errors.push_back("Object2 checking failed");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_invalid, 1, "."))
	{
		errors.push_back("Invalid Object1 checking returns ok");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_invalid, 2, "."))
	{
		errors.push_back("Invalid Object2 checking returns ok");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok, 3, "."))
	{
		errors.push_back("Checking object with incorrect type returned ok");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_invalid_param_value, 2, "."))
	{
		errors.push_back("Checking object with invalid parameter value returned ok");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_missed_params, 2, "."))
	{
		errors.push_back("Checking object with missed parameters failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_another_params, 2, "."))
	{
		errors.push_back("Checking object with another parameters failed");
		bResult = false;
	}
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_partial_params, 1, "."))
	{
		errors.push_back("Checking object with incomplete parameters list failed");
		bResult = false;
	}
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_partial_invalid_params, 1, "."))
	{
		errors.push_back("Checking object with partial invalid parameters values returns ok");
		bResult = false;
	}

	if (!dskm_wrapper.CheckRegFileUsingInsideSign(
										szLicenseKey,
										ObjectType::LicenseKey,
										szKLOperablePublic
										))
	{
		errors.push_back("License key checking failed");
	}

	// портим файл с публичным ключом ЛК
	const int data_size = 5;
	char data_orig[data_size];
	ReadDataFromFile(szKLPublic, 10, data_orig, data_size*sizeof(char));
	char data[data_size];
	std::fill(data, data + data_size, '\0');
	WriteDataToFile(szKLPublic, 10, data, data_size*sizeof(char));

	// проверяем объекты после восстановления файла (на всякий случай)
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok, 1, ".") ||
		dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_ok, 2, "."))
	{
		errors.push_back("Object checking returns ok while KL public key file is invalid");
		bResult = false;
	}

	if (dskm_wrapper.CheckRegFileUsingInsideSign(
										szLicenseKey,
										ObjectType::LicenseKey,
										szKLOperablePublic
										))
	{
		errors.push_back("License key checking returns ok while KL public key file is invalid");
		bResult = false;
	}

	// восстанавливаем файл с публичным ключом ЛК
	WriteDataToFile(szKLPublic, 10, data_orig, data_size*sizeof(char));

	// проверяем объекты
	if (!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok, 1, ".") ||
		!dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_ok, 2, "."))
	{
		errors.push_back("Object checking failed with edited KL public key file");
		bResult = false;
	}

	// портим файл с публичным операбельным ключом
	ReadDataFromFile(szKLOperablePublic, 10, data_orig, data_size*sizeof(char));
	WriteDataToFile(szKLOperablePublic, 10, data, data_size*sizeof(char));

	// проверяем объекты
	if (dskm_wrapper.CheckObjectsUsingHashRegsFolder(object1_ok, 1, ".") ||
		dskm_wrapper.CheckObjectsUsingHashRegsFolder(object2_ok, 2, "."))
	{
		errors.push_back("Object checking returns ok while KL operable public key file is invalid");
		bResult = false;
	}

	// портим файл лицензионного ключа
	{
		char ch = '\0';
		WriteDataToFile(szLicenseKey, 0, &ch, 1);
	}

	if (dskm_wrapper.CheckRegFileUsingInsideSign(
										szLicenseKey,
										ObjectType::LicenseKey,
										szKLOperablePublic
										))
	{
		errors.push_back("Checking corrupted license key file returned ok");
		bResult = false;
	}

	return bResult;
}

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

