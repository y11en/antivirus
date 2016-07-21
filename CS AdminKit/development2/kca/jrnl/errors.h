/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Mikhail Karmazine
 * \date	22.01.2003 15:29:11
 * \brief	Ошибки модуля KLJRNL
 * 
 */

#ifndef __JRNL_ERRORS_H__
#define __JRNL_ERRORS_H__

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLJRNL {

    enum ErrorCodes {
        ERR_OPENED = KLJRNLERRSTART + 1,
        ERR_CLOSED,
        ERR_CANT_OPEN2,
        ERR_JOURNAL_EXISTS1,
        ERR_CHANGE_NOT_ALLOWED,
        ERR_READ_NOT_ALLOWED,
        ERR_FILE_OPERATION2,
        ERR_BAD_JOURNAL_FORMAT1,
        ERR_CANT_READ2,
        ERR_CANT_WRITE2,
        ERR_UNSOPPORTED_VERSION1,
        ERR_CORRUPTED,
        ERR_PREVIOUS_OPERATION_FAILED,
        ERR_CANT_CLOSE2,
        ERR_NO_CURRENT_POS,
        ERR_MAX_PAGE_REC_COUNT1,
        ERR_EMPTY_DATA,
        ERR_UPDATED_DATA_HAS_OTHER_SIZE,
        ERR_UNEXPECTED_END_OF_INDEX1,
        ERR_INVALID_JOURNAL_FILE_NAME1
    };


    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_OPENED,		L"Cannot perform this operation when journal is opened."},
        {ERR_CLOSED,		L"Cannot perform this operation when journal is closed."},
        {ERR_CANT_OPEN2,    L"Cannot open journal (\"%ls\"): %ls"},
        {ERR_JOURNAL_EXISTS1, L"Journal (\"%ls\") already exists."},
        {ERR_CHANGE_NOT_ALLOWED, L"Open mode of the journal doesn't allow changes."},
        {ERR_READ_NOT_ALLOWED, L"Open mode of the journal doesn't allow read operations."},
        {ERR_FILE_OPERATION2,  L"Error during operation on file \"%ls\": %ls."},
        {ERR_BAD_JOURNAL_FORMAT1, L"Bad format of journal (file \"%ls\")."},
        {ERR_CANT_READ2, L"Cannot read from journal file \"%ls\": %ls."},
        {ERR_CANT_WRITE2, L"Cannot perform write operation on journal file \"%ls\": %ls."},
        {ERR_UNSOPPORTED_VERSION1, L"Journal \"%ls\" has unsopported version."},
        {ERR_CORRUPTED, L"Journal is corrupted."},
        {ERR_PREVIOUS_OPERATION_FAILED, L"Previous operation failed, reopen journal to continue work."},
        {ERR_CANT_CLOSE2, L"Cannot close journal file \"%ls\": %ls."},
        {ERR_NO_CURRENT_POS, L"No current position - cannot read."},
        {ERR_MAX_PAGE_REC_COUNT1, L"Maximum record count for journal cannot be less than 1 (journal file \"%ls\")."},
        {ERR_EMPTY_DATA, L"Empty data is not allowed."},
        {ERR_UPDATED_DATA_HAS_OTHER_SIZE, L"Size of new data differs from size of old one."},
        {ERR_UNEXPECTED_END_OF_INDEX1, L"Unexpected end of index file \"%ls\"." },
        {ERR_INVALID_JOURNAL_FILE_NAME1, L"Name \"%ls\" is invalid. Use other file extension." },
		{0,0}
    };
}


#endif //__JRNL_ERRORS_H__

// Local Variables:
// mode: C++
// End:
