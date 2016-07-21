#include "npr_ichecker2.h"
#include "calc_file_hash.h"
#include <string.h>

tERROR IChecker_Create( hIChecker *_this, char * szDBFileName, tDWORD dwUserDataSize, tDWORD dwUserVersion )
{
	tERROR error;

	*_this = (hIChecker)malloc(sizeof(struct tag_IChecker));

	strcpy((*_this)->szDBFileName,szDBFileName);
	(*_this)->dwUserDataSize = dwUserDataSize;
	(*_this)->dwUserVersion = dwUserVersion;

	error = CALL_SecureFileDatabase_Create(&(*_this)->hDataBase);

	error = CALL_SecureFileDatabase_SetSerializeOptions( (*_this)->hDataBase , cSFDB_SERIALIZE_CHECKSUM | cSFDB_SERIALIZE_CRYPT );

	if (((*_this)->dwUserDataSize % 4) != 0)
	{
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	if (PR_SUCC(error = CALL_SecureFileDatabase_Load((*_this)->hDataBase, (*_this)->szDBFileName)))
	{
		tDWORD dwUserVersion; // AKA RecordIdAlg
		tDWORD dwRecordIDSize;
		tDWORD dwRecordDataSize;
		if (PR_SUCC(error = CALL_SecureFileDatabase_GetDBInfo((*_this)->hDataBase, &dwUserVersion, &dwRecordIDSize, &dwRecordDataSize)))
		{
			if (dwUserVersion != (*_this)->dwUserVersion ||
				dwRecordIDSize != 8 ||
				dwRecordDataSize != (*_this)->dwUserDataSize)
			error = errOBJECT_ALREADY_EXISTS;
		}
	}
	if(PR_FAIL(error)){
		error = CALL_SecureFileDatabase_InitNew((*_this)->hDataBase, (*_this)->dwUserVersion, 8, (*_this)->dwUserDataSize, 0);
	}
	if (PR_FAIL(error))
		CALL_SecureFileDatabase_Close((*_this)->hDataBase);

	return error;
}



tERROR IChecker_Close( hIChecker _this )
{
	tERROR error = errOK;

	if (_this->hDataBase)
		error = CALL_SecureFileDatabase_Save(_this->hDataBase, _this->szDBFileName);

	CALL_SecureFileDatabase_Close(_this->hDataBase);

	if(PR_SUCC(error))
		free(_this);

	return error;
}

tERROR IChecker_GetStatus( hIChecker _this, tDWORD* result, tQWORD* pHash, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	return IChecker_GetStatusEx( _this, result, pHash, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize, 0 );
}

tERROR IChecker_GetStatusEx( hIChecker _this, tDWORD* result, tQWORD* pHash, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags )
{
	tERROR error = errNOT_IMPLEMENTED;

	// GetFormatType
	// CountCRC

	if (dwStatusBufferSize != _this->dwUserDataSize)
		return errPARAMETER_INVALID;

	error = GetFileHash(io, (tBYTE*)pHash, 8, pData, dwDataSize, dwFlags);
	if(PR_SUCC(error)){
		
		// Find In database
		error = CALL_SecureFileDatabase_GetRecord(_this->hDataBase, pHash, pStatusBuffer);
		if (error == errNOT_FOUND)
		{
			// Return Status Found
			error = errOK;
		}
		

	}

	if ( result )
		*result = _this->dwUserDataSize;
	
	return error;
}



tERROR IChecker_UpdateStatus( hIChecker _this, tQWORD* pHash, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize )
{
	tERROR error;

	if (dwStatusBufferSize != _this->dwUserDataSize)
		return errPARAMETER_INVALID;

	// Update in DataBase
	error = CALL_SecureFileDatabase_UpdateRecord(_this->hDataBase, pHash, pStatusBuffer);

	return error;
}



tERROR IChecker_DeleteStatus( hIChecker _this, tQWORD* pHash )
{
	tERROR error;

	// Delete in DataBase
	error = CALL_SecureFileDatabase_DeleteRecord(_this->hDataBase, pHash);

	return error;
}



tERROR IChecker_FlushStatusDB( hIChecker _this )
{
	tERROR error = errNOT_IMPLEMENTED;

	if (!_this->hDataBase)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_SecureFileDatabase_Save(_this->hDataBase, _this->szDBFileName);

	return error;
}



tERROR IChecker_ClearStatusDB( hIChecker _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	return error;
}



