/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file ft/Errors.h
 * \author Шифетдинов Дамир
 * \date 2004
 * \brief Описание кодов ошибок
 *
 */

#ifndef KLFT_ERRORS_H
#define KLFT_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLFT {

	const wchar_t KLFT_ModuleName[] = L"FT";

	#define KLFT_THROW_ERROR( _code ) \
			KLERR_throwError(KLFT::KLFT_ModuleName, _code, __FILE__, __LINE__, NULL, NULL)

	#define KLFT_THROW_ERROR1( _code, _param1 ) \
			KLERR_throwError(KLFT::KLFT_ModuleName, _code, __FILE__, __LINE__, NULL, _param1)

	#define KLFT_THROW_ERROR2( _code, _param1, _param2 ) \
			KLERR_throwError(KLFT::KLFT_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2)
	

    enum ErrorCodes {
        ERR_WRONG_FILE_ID = KLFTSTART + 1,
		ERR_BAD_FUNCTION_PARAMETERS,
		ERR_WRONG_FILE_FORMAT,
		ERR_BAD_FILE_OBJECT,
		ERR_ERROR_OF_UPLOAD,
		ERR_NOT_UPLOADED,
		ERR_FILE_TRANSFER_NOT_INIT,
		ERR_FILE_OPERATION,
		ERR_FILE_UPLOADING_CANCELED,
		ERR_MAX_UPLOADING_CLIENTS_NUMBER_EXCEEDED,
		ERR_FILE_NOT_FOUND,
		ERR_FILE_IS_ALREADY_UPLOADING
    };

    //! Перечисление кодов ошибок модуля Product Component Instance. 
    //  Любой из методов модуля может вернуть исключение с этим кодом ошибки.
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_WRONG_FILE_ID,				L"Wrong working file ID '%ls'."},
		{ERR_BAD_FUNCTION_PARAMETERS,	L"Bad function parameters."},			
		{ERR_WRONG_FILE_FORMAT,			L"Wrong file format. File name - '%ls'."},		
		{ERR_BAD_FILE_OBJECT,			L"Bad file object. File id - '%ls'."},
		{ERR_ERROR_OF_UPLOAD,			L"Error in upload operation. File id - '%ls'."},		
		{ERR_NOT_UPLOADED,				L"File haven't uploaded yet. File id - '%ls'."},		
		{ERR_FILE_TRANSFER_NOT_INIT,	L"File transfer module not initialized."},
		{ERR_FILE_OPERATION,			L"Error in file operation."},		
		{ERR_FILE_UPLOADING_CANCELED,	L"Uploading operation canceled by server. File id - '%ls'."},		
		{ERR_MAX_UPLOADING_CLIENTS_NUMBER_EXCEEDED,	L"Maximum number of uploading clients was exceeded. File id - '%ls'."},		
		{ERR_FILE_NOT_FOUND,						L"File not found. File name - '%ls'."},
		{ERR_FILE_IS_ALREADY_UPLOADING,	L"File is already uploading. File name - '%ls'."},
        {0,0}
    };

}

#endif // KLFT_ERRORS_H

