/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Andrew Kazachkov
 * \date	23.10.2006 11:03:29
 * \brief	
 * 
 */

#ifndef KLSTDCONF_ERRORS_H
#define KLSTDCONF_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLSTDCONF
{
    enum
    {
        FCONF_INTERNAL_ERROR = KLSTDCONFSTART,
        FCONF_UNEXPECTED_EOF,
        FCONF_INVALID_SRC_FILE,
        FCONF_INVALID_DST_FILE,
        FCONF_ERROR_READING_SRC,
        FCONF_ERROR_WRITING_DST,
        FCONF_ERROR_INVALID_ARGUMENTS,
        FCONF_UNKNOWN_STATEMENT,
        FCONF_UNEXPECTED_TOKEN,
        FCONF_EXPECTED_OTHER,
        FCONF_INTEGER_OVERFLOW,
        FCONF_INVALID_VALUE,
        FCONF_BAD_CHAR_IN_NUMBER,
        FCONF_NEW_LINE_IN_CONSTANT,
        FCONF_UNCLOSED_STRING,
        FCONF_UNSUPPORTED,
        FCONF_NOT_IMPLEMENTED,
        FCONF_ALREADY_DECLARED,
        FCONF_UNDECLARED,
        FCONF_UNPAIRED_BRACKETS
    };

    const KLERR::ErrorDescription c_errorDescriptions[]=
    {
        {FCONF_INTERNAL_ERROR,    L"Internal error: %ls"},
        {FCONF_UNEXPECTED_EOF,    L"Unexpected end of file"},
        {FCONF_INVALID_SRC_FILE,  L"Cannot open input file: %ls"},
        {FCONF_INVALID_DST_FILE,  L"Cannot open output file: %ls"},
        {FCONF_ERROR_READING_SRC, L"Failed reading input file %ls"},
        {FCONF_ERROR_WRITING_DST, L"Failed writing output file %ls"},
        {FCONF_ERROR_INVALID_ARGUMENTS, L"Invalid arguments: %ls"},
        {FCONF_UNKNOWN_STATEMENT, L"Unknown statement '%ls'"},
        {FCONF_UNEXPECTED_TOKEN, L"Unexpected symbol '%ls'"},
        {FCONF_EXPECTED_OTHER, L"Other symbol expected: unexpected symbol '%ls', %ls expected"},
        {FCONF_INTEGER_OVERFLOW, L"Integer overflow occured for '%ls'"},
        {FCONF_INVALID_VALUE, L"Invalid value used in '%ls'"},
        {FCONF_BAD_CHAR_IN_NUMBER, L"Invalid character in numer '%ls'"},
        {FCONF_NEW_LINE_IN_CONSTANT, L"New line in constant '%ls'"},
        {FCONF_UNCLOSED_STRING, L"Unclosed string '%ls'"},
        {FCONF_UNSUPPORTED, L"Symbol %ls is unsupported"},
        {FCONF_NOT_IMPLEMENTED, L"Not implemented yet '%ls'"},
        {FCONF_ALREADY_DECLARED, L"Symbol %ls is already declared"},
        {FCONF_UNDECLARED, L"Undefined symbol %ls"},
        {FCONF_UNPAIRED_BRACKETS, L"Unpaired brackets %ls"}
    };
}

#endif // KLSTDCONF_ERRORS_H
