#ifndef _SIGN_STRUCTURES_H__
#define _SIGN_STRUCTURES_H__

/*
      Описание формата "новой" подписи

      Подпись добавляется в конец файла согласно следующему алгоритму :
      
      1. добавляется SIGN_HEADER_SIGNATURE
      2. добавляется общий размер всего, чего добавляем.
      3. добавляем последовательно структуры SIGN_STRUCT ( их может быть несколько )
      4. добвляем общий размер всего, чего добавляем. ( то же число, что и в п.2 )
      5. SIGN_FOOTER_SIGNATURE
*/

#define SIGN_HEADER_SIGNATURE 'oRuR'
#define SIGN_FOOTER_SIGNATURE 'RuRo'

#pragma pack ( push, 1 )


typedef struct _SIGN_HEADER {
    unsigned long  Tag;
    unsigned long  TotalSize;

} SIGN_HEADER, *PSIGN_HEADER;

typedef struct _SIGN_FOOTER {
    unsigned long  TotalSize;
    unsigned long  Tag;
} SIGN_FOOTER, *PSIGN_FOOTER;

#define ALG_UNDEFINED   0x0
#define ALG_CRYPTOC     'gcpc'  // CryPtoC Gost

typedef struct _SIGN_STRUCT {
    unsigned long AlgID;		// ID используемого алгоритма
    unsigned long Size;			// Размер структуры

    char  Sign[0];		// подпись
} SIGN_STRUCT, *PSIGN_STRUCT;

// сумма размеров + размер заголовка == Size ( см. определение SIGN_STRUCT ) 
typedef struct _CRYPTOC_SIGN {
    unsigned long Key1Size;    // сначала в буфере лежит Key1
    unsigned long Key2Size;    // потом в буфере лежит Key2
    unsigned long RegSize;     // потом в буфере лежит реестр.

    char  Buffer[0];   // Буффер, в котором последовательно лежат два ключа и реестр.
} CRYPTOC_SIGN, *PCRYPTOC_SIGN;

#pragma pack ( pop )

#endif