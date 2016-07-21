#ifndef _SIGNGET_H_
#define _SIGNGET_H_

#include <ctype.h>
#include "sign.h"

#define DEF_SIGN_BUFFER 58
typedef unsigned char TSignBuffer[DEF_SIGN_BUFFER + 1];

// 1. Проверяет подпись в szFileName посредством вызова sign_check_file
// 2. В случае успеха 1. возвращает подпись vSign из файла szFileName
// Возвращаемые значения
//     SIGN_OK - успешное завершение
//     SIGN_BAD_FILE - ошибка 
int SIGNAPI sign_get_file( const char* szFileName, 
                           TSignBuffer& vSign );

// 1. Проверяет подпись в wszFileName посредством вызова sign_check_fileW
// 2. В случае успеха 1. возвращает подпись vSign из файла wszFileName
// Возвращаемые значения
//     SIGN_OK - успешное завершение
//     SIGN_BAD_FILE - ошибка 
int SIGNAPI sign_get_file( const wchar_t* wszFileName, 
                           TSignBuffer& vSign );

#endif