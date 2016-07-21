// tree.h
//

#ifndef tree_h_INCLUDED
#define tree_h_INCLUDED

#include "treedefs.h"


// --------------------------------------------------------------------------------------------
//   ИНИЦИАЛИЗАЦИЯ / ДЕИНИЦИАЛИЗАЦИЯ
// --------------------------------------------------------------------------------------------

// Начальная инициализация
// pStorageHeapCtx	- указатель на проинициализированную структуру с CALLBACK функциями системы распределения памяти
// ppRTCtx			- принимает контекст дерева (Runtime Tree Context)
EXTERN_C TREE_ERR _TREE_CALL TreeInit(IN sHEAP_CTX* pStorageHeapCtx, OUT sRT_CTX** ppRTCtx);

// Закрывает контекст дерева
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
EXTERN_C TREE_ERR _TREE_CALL TreeDone(IN sRT_CTX* pRTCtx);

// Получить информацию о дереве
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pTreeInfo		- получает информацию о дереве в структуру TREE_INFO
EXTERN_C TREE_ERR _TREE_CALL TreeGetInfo(IN sRT_CTX* pRTCtx, OUT TREE_INFO* pTreeInfo);


// --------------------------------------------------------------------------------------------
//   ЗАГРУЗКА / ВЫГРУЗКА
// --------------------------------------------------------------------------------------------

// Все TreeLoad_xxx фукции предварительно освобождают любое загруженное дерево, если оно было загружено в контекст.

// Загружает статическое дерево.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pIOCtx			- указатель на проинициализированную структуру с CALLBACK функциями ввода-вывода
EXTERN_C TREE_ERR _TREE_CALL TreeLoad_Static(IN sRT_CTX* pRTCtx, IN sIO_CTX* pIOCtx);

// Загружает статическое дерево на загруженных (отмапированных данных).
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pMappedData		- указатель на загруженные (отмапированные) данные дерева
// dwMappedDataSize - размер загруженных (отмапированных) данных в байтах
EXTERN_C TREE_ERR _TREE_CALL TreeLoad_StaticOnMap(IN sRT_CTX* pRTCtx, IN void* pMappedData, IN uint32_t dwMappedDataSize);

// Выгружает статическое дерево
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
EXTERN_C TREE_ERR _TREE_CALL TreeUnload_Static(IN sRT_CTX* pRTCtx);

// Загружает динамическое дерево.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pIOCtx			- указатель на проинициализированную структуру с CALLBACK функциями ввода-вывода
EXTERN_C TREE_ERR _TREE_CALL TreeLoad_Dynamic(IN sRT_CTX* pRTCtx, IN sIO_CTX* pIOCtx);

// Выгружает динамическое дерево
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
EXTERN_C TREE_ERR _TREE_CALL TreeUnload_Dynamic(IN sRT_CTX* pRTCtx);

// Загружает статическое или динамическое дерево.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pIOCtx			- указатель на проинициализированную структуру с CALLBACK функциями ввода-вывода
EXTERN_C TREE_ERR _TREE_CALL TreeLoad_Any(IN sRT_CTX* pRTCtx, IN sIO_CTX* pIOCtx);

// Выгружает статическое или динамическое дерево
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
EXTERN_C TREE_ERR _TREE_CALL TreeUnload_Any(IN sRT_CTX* pRTCtx);

// Сохраняет статическое дерево из динамического, дерево в памяти не меняется и остается динамическим
// pRTCtx		    - контекст дерева ранее полученный из TreeInit, должен содержать динамическое дерево
// pIOCtx			- указатель на проинициализированную структуру с CALLBACK функциями ввода-вывода
EXTERN_C TREE_ERR _TREE_CALL TreeSave_Static(IN sRT_CTX* pRTCtx, IN sIO_CTX* pIOCtx, IN uint32_t dwHashLevel, IN uint32_t dwHashTableSizeKb);

// Сохраняет динамическое дерево
// pRTCtx		    - контекст дерева ранее полученный из TreeInit, должен содержать динамическое дерево
// pIOCtx			- указатель на проинициализированную структуру с CALLBACK функциями ввода-вывода
EXTERN_C TREE_ERR _TREE_CALL TreeSave_Dynamic(IN sRT_CTX* pRTCtx, IN sIO_CTX* pIOCtx);


// --------------------------------------------------------------------------------------------
//   ПОИСК
// --------------------------------------------------------------------------------------------

// Получает контекст для поиска, фактически - создает инстанс объекта поиска.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pSeacherHeapCtx	- указатель на проинициализированную структуру с CALLBACK функциями системы распределения памяти
//					  может совпадать с pStorageHeapCtx
// ppSeacherRTCtx	- принимает контест поиска
EXTERN_C TREE_ERR _TREE_CALL TreeGetSeacherContext(IN sRT_CTX* pRTCtx, IN sHEAP_CTX* pSeacherHeapCtx, OUT sSEARCHER_RT_CTX** ppSeacherRTCtx);

// Закрывает контекст поиска, фактически - деструктор истанса объекта поиска.
// pSeacherRTCtx	- контест поиска, ранее полученный из TreeGetSeacherContext
EXTERN_C TREE_ERR _TREE_CALL TreeDoneSeacherContext(IN sSEARCHER_RT_CTX* pSeacherRTCtx);

// прототип CALLBACK функции, вызываемой при поиске для найденных сигнатур
// pCallbackCtx     - контекст, переданный при вызове TreeSearchData
// SignatureID      - идентификатор найденной сигнатуры
// dwPosInBuffer    - позиция начала сигнатуры в буфере поиска. Если сигнатура началась в предыдущем блоке, значение будет отрицательное.
// qwPosLinear      - линейная позиция от начала поиска независимо от кол-ва блоков
// dwSignatureLen   - длина сигнатуры
// Возвращаемые значения:
//    errOPERATION_CANCELED - остановить поиск и выйти из TreeSearchData
//    другие                - продолжать поиск
typedef TREE_ERR (_TREE_CALL* tTreeSignatureFoundCallbackFunc)(IN CONST void* pCallbackCtx, IN tSIGNATURE_ID SignatureID, IN int32_t dwPosInBuffer, IN uint64_t qwPosLinear, IN uint32_t dwSignatureLen);

// Выполняет поиск по дереву. Поиск по потоку данных может выполняеться блоками любой длины, для этого 
// нужно просто последовательно вызывать TreeSearchData указывая тот-же контекст поиска.
// pSeacherRTCtx	- контест поиска, ранее полученный из TreeGetSeacherContext
// pbData			- данные, в которых производится поиск
// dwSize			- размер данных в байтах
// dwStartPos		- стартовая позиция в данных, с которой начинается поиск
// dwEndPos			- конечная позиция в данных, до которой производится поиск. 
//                    (!) Сигнатуры начинающиеся до dwEndPos, но заканчивающиеся после нее будут найдены.
//                    (!) Если dwStartPos равно dwEndPos, то будут найдены сигнатуры начинающиеся 
//                    только с этой позиции.
// TreeSignatureFoundCallbackFunc - указатель на CALLBACK функцию, которая будет вызываться при нахождении сигнатуры.
// pCallbackCtx		- контекст, с которым будет вызываться CALLBACK функция.
EXTERN_C TREE_ERR _TREE_CALL TreeSearchData(IN sSEARCHER_RT_CTX* pSearcherRTCtx,
							   IN CONST uint8_t* pbData,
							   IN uint32_t dwSize,
							   IN uint32_t dwStartPos,
							   IN uint32_t dwEndPos,
							   IN tTreeSignatureFoundCallbackFunc TreeSignatureFoundCallbackFunc,
							   IN CONST void* pCallbackCtx);

// Сбрасывает контекст поиска для выполнения нового поиска
// pSearcherRTCtx	- контест поиска, ранее полученный из TreeGetSeacherContext
EXTERN_C TREE_ERR _TREE_CALL TreeNewSearch(IN sSEARCHER_RT_CTX* pSearcherRTCtx);


// --------------------------------------------------------------------------------------------
//   МОДИФИКАЦИЯ ДЕРЕВА
// --------------------------------------------------------------------------------------------

// Все опереции по модификации выполняются только для динамического дерева

// Добавить сигнатуру в дерево.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pSignature		- сигнатура в формате TreeSearch
// dwSignatureSize	- длина сигнатуры в формате TreeSearch
// SignatureID		- идентификатор, который должна получить сигнатура при добавлении в дерево
// pResultSignatureID - в случае успеха получает идентификатор добавленной сигнатуры. Если добавляемая 
//					  сигнатура уже присутствовала в дереве, функция возвращает errOBJECT_ALREADY_EXIST
//                    а pResultSignatureID получает идентификатор имеющейся сигнатуры.
EXTERN_C TREE_ERR _TREE_CALL TreeAddSignature(IN sRT_CTX* pRTCtx, IN CONST uint8_t* pSignature, IN uint32_t dwSignatureSize, IN tSIGNATURE_ID SignatureID, OUT tSIGNATURE_ID* pResultSignatureID);

// Удалить сигнатуру из дерева.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pSignature		- сигнатура в формате TreeSearch
// dwSignatureSize	- длина сигнатуры в формате TreeSearch
EXTERN_C TREE_ERR _TREE_CALL TreeDeleteSignature(IN sRT_CTX* pRTCtx, IN CONST uint8_t* pSignature, IN uint32_t dwSignatureSize);

// Проверить наличие сигнатуры в дереве. В случае успеха возвращает идентификатор сигнатуры
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pSignature		- сигнатура в формате TreeSearch
// dwSignatureSize	- длина сигнатуры в формате TreeSearch
// pSignatureID		- в случае успеха получает идентификатор сигнатуры
EXTERN_C TREE_ERR _TREE_CALL TreeIsSignature(IN sRT_CTX* pRTCtx, IN CONST uint8_t* pSignature, IN uint32_t dwSignatureSize, OUT tSIGNATURE_ID* pSignatureID);

// Преобразует сигнатуру из текстового HEX представления в формат TreeSearch
// pHexMask			- текстовое HEX представление сигнатуры
// dwHexMaskSize	- размер текстового HEX представления в байтах
// pSignature		- буфер, получающий сигнатуру в формате TreeSearch
// dwBufferSize		- размер буфера в байтах
// pdwSignatureSize	- в случае успеха, кол-во байт помещенных в буфер. Если размер буфера 
//					  недостаточен для помещения сигнатуры, функция возвращает errBUFFER_TOO_SMALL 
//                    а pdwSignatureSize получает необходимый размер буфера.
EXTERN_C TREE_ERR _TREE_CALL TreeConvertHex2Signature(IN CONST uint8_t* pHexMask, IN uint32_t dwHexMaskSize, OUT uint8_t* pSignature, IN uint32_t dwBufferSize, OUT uint32_t* pdwSignatureSize);

// Преобразует сигнатуру из бинарного RAW представления в формат TreeSearch
// pRawData			- бинарное RAW представление сигнатуры
// dwRawDataSize	- размер бинарного RAW представления в байтах
// pSignature		- буфер, получающий сигнатуру в формате TreeSearch
// dwBufferSize		- размер буфера в байтах
// pdwSignatureSize	- в случае успеха, кол-во байт помещенных в буфер. Если размер буфера 
//					  недостаточен для помещения сигнатуры, функция возвращает errBUFFER_TOO_SMALL 
//                    а pdwSignatureSize получает необходимый размер буфера.
EXTERN_C TREE_ERR _TREE_CALL TreeConvertRaw2Signature(IN CONST uint8_t* pRawData, IN uint32_t dwRawDataSize, OUT uint8_t* pSignature, IN uint32_t dwBufferSize, OUT uint32_t* pdwSignatureSize);
EXTERN_C TREE_ERR _TREE_CALL TreeConvertSignature2Raw(IN CONST uint8_t* pSignature, IN uint32_t dwSignatureSize, OUT uint8_t* pRawData, IN uint32_t dwBufferSize, OUT uint32_t* pdwRawDataSize);

// --------------------------------------------------------------------------------------------
//   ЕНУМЕРАЦИЯ
// --------------------------------------------------------------------------------------------

// прототип CALLBACK функции, вызываемой при енумерации сигнатур
// pCallbackCtx     - контекст, переданный при вызове TreeEnumSignatures
// SignatureID      - идентификатор сигнатуры
// pSignature		- сигнатура в формате TreeSearch
// dwSignatureSize	- длина сигнатуры в формате TreeSearch
// Возвращаемые значения:
//    errOPERATION_CANCELED - остановить енумерацию и выйти из TreeEnumSignatures
//    другие                - продолжать енумерацию
typedef TREE_ERR (_TREE_CALL *tTreeEnumCallbackFunc)(CONST void* pCallbackCtx, tSIGNATURE_ID SignatureID, CONST uint8_t* pSignature, uint32_t dwSignatureSize);

// Енумерирует все сигнатуры в дереве и вызывает CALLBACK функцию  
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// TreeEnumCallbackFunc - указатель на CALLBACK функцию, которая будет вызываться для каждой сигнатуры.
// pCallbackCtx		- контекст, с которым будет вызываться CALLBACK функция.
EXTERN_C TREE_ERR _TREE_CALL TreeEnumSignatures(sRT_CTX* pRTCtx, tTreeEnumCallbackFunc TreeEnumCallbackFunc, CONST void* pCallbackCtx);


// --------------------------------------------------------------------------------------------
//   ОТЛАДОЧНЫЕ ФУНКЦИИ
// --------------------------------------------------------------------------------------------

// Выводит в STDOUT дерево в текстовом виде. Не поддерживает статический формат дерева.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
EXTERN_C TREE_ERR _TREE_CALL TreeDump(IN sRT_CTX* pRTCtx);

// Получает информацию об узле дерева во время отладки. Используется в Watch окне.
// pRTCtx		    - контекст дерева ранее полученный из TreeInit
// pNode			- узел дерева, о котором нужно получить информацию 
EXTERN_C char * _TREE_CALL DbgNodeInfo(IN sRT_CTX* pRTCtx, IN NODE* pNode);

#endif // tree_h_INCLUDED

