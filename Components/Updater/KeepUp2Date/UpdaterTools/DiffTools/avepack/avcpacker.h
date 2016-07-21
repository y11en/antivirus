#ifndef _AVC_PACKER_H_
#define _AVC_PACKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_SUCCESS         0
#define ERR_INVALID_PARAMS  1
#define ERR_BAD_FORMAT      2
#define ERR_OUTOFMEMORY     3
#define ERR_FILE_IO_ERROR   4
#define ERR_FILE_CORRUPTED  5
#define ERR_AVP_BLOCKHEADER_CORRUPTED  20
#define ERR_GENERAL_ERROR   100

/*
Функция UnpackAvc распаковывает avc-файл в несжатый формат ave2
Передаваемые параметры:
    pszAvcFileName - полный путь к avc-файлу подлежащему распаковке.
    ppAveBuffer -   адрес, по которому будет возвращен указатель на буфер,
                    содержащий распакованные данный.
                    Буфер выделяется внутри данной функции и после использования
                    его необходимо освободить с помощью FreeBuffer.
    pdwAveBufferSize - адрес, по которому будет возвращен размер буфера,
                       содержащего распакованные данные.
Возвращаемое значение:
    В случае успеха - 0.
    В случае ошибки - код ошибки
        1 - invalid parameters
        2 - not avc file
        3 - out of memory
        4 - file i/o error
        5 - file corrupted
        20 - неправилно вычищены LINK16, остался мусор. с такими данными не работаем!!!
*/
int UnpackAvc(const char * pszAvcFileName, void ** ppAveBuffer, unsigned long * pdwAveBufferSize);

/*
Функция PackInAvc упаковывает данные в несжатом формате ave2(only!!!) в формат avc.
Передаваемые параметры:
    pAveBuffer - указатель на буфер, содержащий неупакованные данные.
    dwAveBufferSize - размер буфера, содержащего неупакованные данные.
    pszAvcFileName - полный путь к файлу, в который следует записать
                     упакованные данные.
Возвращаемое значение:
    В случае успеха - 0.
    В случае ошибки - код ошибки
        1 - invalid parameters
        2 - unknown format (not ave2)
        3 - out of memory
        4 - file i/o error
*/
int PackInAvc( void * pAveBuffer, unsigned long dwAveBufferSize, const char * pszAvcFileName );

/*
Функция PackInAvc упаковывает данные в несжатом формате ave(only!!!) в формат avc.
*/
int PackAveInAvc( void *pAveBuffer, unsigned long dwAveBufferSize, const char *pszAvcFileName );

/*
Функция FreeBuffer освобождает буффер, выделенный в функции UnpackAvc.
Передаваемое значение:
    pBuffer - указатель на буфер, который необходимо освободить.
Возвращаемое значение:
    В случае успеха - 0.
    В случае ошибки - код ошибки
        1 - нулевой указатель
        2 - указатель на буфер, который не был выделен функцией UnpackAvc
*/
int FreeBuffer( void * pBuffer );

#ifdef __cplusplus
}
#endif

#endif /*AVC_PACKER_H_*/
