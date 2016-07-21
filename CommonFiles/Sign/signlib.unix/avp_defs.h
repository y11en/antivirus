#ifndef __avp_defs_h
#define __avp_defs_h

#include <Sign/CT_FUNCS.h>

#define AVP_VERSION_NOW         0x1

#define szKaspUser              "Kaspersky Lab"
#define szKaspUserSN            "Kaspersky Lab SK 000"
#define sizeofKaspUserSN        21
//#define szDistUserSN            "Kaspersky Lab SK 001"

#define szPubFileExt            "pub"
#define szOrderFileExt          "ord"
#define szKeyFileExt            "key"
#define szWalletFileExt         "wal"
#define szAVPTitle              "AVP"

#if defined( _DEBUG )
  #define szAVPKeyAddress       "dbkeys1@avp.ru"
#else
  #define szAVPKeyAddress       "dbkeys@avp.ru"
#endif

#define szAVPPublicKeySubject   "AVP public key file"
#define szAVPRequestSubject     "AVP key request"
#define szAVPTestRequest        "This key must be used for testing purpose only"
#define szDBFDriver             "Microsoft Access Driver (*.mdb)"
//#define szDBFDriver             "Microsoft dBase Driver (*.dbf)"

#define ENCRYPT_KEY             0x4321
#define MAX_NUM_OF_COPIES       10000

#define TCP_PORT_NUM            7770
#define TCP_PORT_NUM_STR        "7770"

// ---
#define WALLET_MAGIC  'fWlK'
#define WALLET_XOR    ((unsigned char)0x19)

enum { khe_empty, khe_ready, khe_created, khe_restricted };

#pragma pack(1)

typedef struct t_KHeadEntry {
  unsigned ok      : 4;    // ключ создан и не использован
  unsigned offset  : 28;   // смещение от начала файла
} KHeadEntry;


#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct _GUID {          // size is 16
    unsigned long   Data1;
    unsigned short  Data2;
    unsigned short  Data3;
    unsigned char   Data4[8];
} GUID;

#endif // !GUID_DEFINED


// ---
typedef struct t_WalletHeader {
  unsigned long magic;          // signature
  unsigned long version;        // version
  unsigned long head_len;       // длина заголовка
  unsigned long cont_len;       // длина файла без подписи
  unsigned long pack_len;       // длина файла без подписи
  unsigned long distr_num;      // проядковый номер дисрибютора
  GUID          order_id;       // id заказа
  unsigned long order_num;      // порядковый номер заказа
  unsigned long key_num;        // первый серийный номер ключа
  unsigned long key_len;        // длина ключа без подписи
  short order_count;    // кол-во ключей в заказе
  short real_count;     // кол-во ключей в файле
  KHeadEntry keys[1];   // заголовки для ключей
} WalletHeader;

#pragma pack()

#endif /* __avp_defs_h */

