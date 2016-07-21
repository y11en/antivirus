/*
    ct_defs.h

    Definitions for the Cryptographic Toolkit.
    Version 2.1

    Last changes: 20.11.96 15:04

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#ifndef __CT_DEFINES__
#define __CT_DEFINES__

#define CT_LEN_EXPIRATION_LIMIT     1
#define CT_LEN_FF_HASH_DEGREE       9
#define CT_LEN_FF_HASH_MASK         ((1 << CT_LEN_FF_HASH_DEGREE)-1)
#define CT_LEN_FF_HASH_TABLE        (1 << CT_LEN_FF_HASH_DEGREE)
#define CT_LEN_MAXFILENAME          256
#define CT_FLAGS_MULTI_VAULT        0x8000

#define CT_STR_TOUCH_MEMORY         "TM-512 Key"
#define CT_TYP_TOUCH_CRYPTO         0x00000001UL
#define CT_TYP_TOUCH_SIGN           0x00000002UL

#define CT_V_MET_VESTA_2M           0x00000001UL
#define CT_V_MET_GOST_RF_28147_C    0x00000002UL
#define CT_V_MET_GOST_RF_28147_G    0x00000004UL
#define CT_V_MET_GOST_RF_28147_GB   0x00000008UL
#define CT_V_MET_DES_ECB            0x00000010UL
#define CT_V_MET_DES_CBC            0x00000020UL
#define CT_V_MET_DES_CFB            0x00000040UL
#define CT_V_MET_DES_OFB            0x00000080UL
#define CT_V_MET_TRIPLE_DES         0x00000100UL
#define CT_V_MET_FEAL               0x00000200UL
#define CT_V_MET_SAFER64            0x00000400UL
#define CT_V_MET_WICKER_ECB_128		0x00000800UL
#define CT_V_MET_WICKER_ECB_192		0x00001000UL
#define CT_V_MET_WICKER_ECB_256		0x00002000UL
#define CT_V_MET_WICKER_CBC_128		0x00004000UL
#define CT_V_MET_WICKER_CBC_192		0x00008000UL
#define CT_V_MET_WICKER_CBC_256		0x00010000UL

#define CT_V_STR_VERSION            "VESTA-512 Ver. 2.35  Copyright (C) \"LAN Crypto\", Moscow 1996"
#define CT_V_STR_PRIVATEKEY         "Private Key (Vesta-512)"
#define CT_V_STR_PUBLICKEY          "Public Key (Vesta-512)"
#define CT_V_STR_VAULT              "Public Keys Vault (Vesta-512)"
#define CT_V_STR_DEF_PRIVATEKEYNAME "priv0000.000"
#define CT_V_STR_DEF_PUBLICKEYNAME  "pub0000.ves"
#define CT_V_STR_DEF_VAULTNAME      "ves512.vlt"

#define CT_V_CRY_BEGINOFMESSAGE     "=================== START OF ENCRYPTED MESSAGE ==================="
#define CT_V_CRY_ENDOFMESSAGE       "==================== END OF ENCRYPTED MESSAGE ===================="

#define CT_V_PUB_BEGINOFPUBLICKEY  "========= START OF PUBLIC KEY. VESTA-512 ========="
#define CT_V_PUB_ENDOFPUBLICKEY    "========== END OF PUBLIC KEY. VESTA-512 =========="

#define CT_V_PUB_HEX                "A9"
#define CT_V_PUB_PRN                "AK"

#define CT_V_WGK_HEX                "F9"
#define CT_V_WGK_PRN                "WK"

#define CT_V_BLK_LEN_VESTA_2M       	1
#define CT_V_BLK_LEN_GOST_RF_C      	8
#define CT_V_BLK_LEN_GOST_RF_G      	8
#define CT_V_BLK_LEN_GOST_RF_GB     	8
#define CT_V_BLK_LEN_DES_ECB        	8
#define CT_V_BLK_LEN_DES_CBC        	8
#define CT_V_BLK_LEN_DES_CFB        	8
#define CT_V_BLK_LEN_DES_OFB        	8
#define CT_V_BLK_LEN_TRIPLE_DES     	8
#define CT_V_BLK_LEN_FEAL           	8
#define CT_V_BLK_LEN_SAFER64        	8
#define CT_V_BLK_LEN_WICKER         	16
#define CT_V_BLK_LEN_MAXBLOCK       	16

#define CT_V_CRY_BIN                "\xBA\x43T\x09"
#define CT_V_CRY_HEX                "BA"
#define CT_V_CRY_PRN                ">"

#define CT_V_LEN_ONLYSN             20
#define CT_V_LEN_SN                 22
#define CT_V_LEN_USERNAME           128
#define CT_V_LEN_ONLYSHORTNAME      8
#define CT_V_LEN_SHORTNAME          10
#define CT_V_LEN_XKEY               20
#define CT_V_LEN_MAXXKEY            32
#define CT_V_LEN_KEY                512
#define CT_V_LEN_PUBLICKEY          (CT_V_LEN_KEY/8)
#define CT_V_LEN_DISK_ADDITION_KEY  (CT_V_LEN_KEY/8)
#define CT_V_LEN_TOUCH_ADDITION_KEY CT_V_LEN_MAXXKEY
#define CT_V_LEN_ADDITION_KEY       (CT_V_LEN_KEY/8)
#define CT_V_LEN_PRIVATEKEYFILE     (sizeof(CT_V_STR_PRIVATEKEY)+2*CT_V_LEN_ONLYSN+1+1+2+CT_V_LEN_USERNAME+CT_V_LEN_ONLYSHORTNAME+2+2*(CT_V_LEN_MAXXKEY+CT_V_LEN_PUBLICKEY)+4+4+4+4+2)
#define CT_V_LEN_PUBLICKEYRECORD    (CT_V_LEN_ONLYSN+4+CT_V_LEN_ONLYSHORTNAME+CT_V_LEN_USERNAME+CT_V_LEN_PUBLICKEY+4)

#define CT_V_TYP_CRYPTO_BINARY      0x0001
#define CT_V_TYP_CRYPTO_PRINTABLE   0x0002
#define CT_V_TYP_CRYPTO_HEXDECIMAL  0x0003
#define CT_V_TYP_CRYPTO_PICTURE     0x0004
#define CT_V_TYP_CRYPTO             0x000F

#define CT_V_TYP_DOS_STYLE          0x0000
#define CT_V_ADD_PUBLIC_KEY         0x0010
#define CT_V_ADD_FILE_NAME          0x0020
#define CT_V_WRITE_OVERWRITE        0x0040

#define CT_V_COMPRESS_NONE          0x0000
#define CT_V_COMPRESS_LZSS          0x0100
#define CT_V_COMPRESS_ASC           0x0200
#define CT_V_COMPRESS               (CT_V_COMPRESS_LZSS|CT_V_COMPRESS_ASC)

#define CT_V_WIPE_ORIGINAL_FILE     0x1000
#define CT_V_SELF_DECRYPTO          0x2000
#define CT_V_WORKGROUP              0x4000

#define CT_V_MAXLEN_WORKBUFFER      2048

#define CT_N_MET_GOST_RF_3410       0x00000002UL
#define CT_N_MET_LANS               0x00000004UL
#define CT_N_MET_DSA                0x00000100UL

#define CT_N_STR_VERSION            "NOTARY-512 Ver. 2.35  Copyright (C) \"LAN Crypto\", Moscow 1996"
#define CT_N_STR_PRIVATEKEY         "Private Key (Notary-512)"
#define CT_N_STR_PUBLICKEY          "Public Key (Notary-512)"
#define CT_N_STR_FPUBLICKEY         "Full Public Key (Notary-512)"
#define CT_N_STR_VAULT              "Public Keys Vault (Notary-512)"
#define CT_N_STR_DEF_PRIVATEKEYNAME "sign0000.000"
#define CT_N_STR_DEF_PUBLICKEYNAME  "pub0000.not"
#define CT_N_STR_DEF_FPUBLICKEYNAME "fpk0000.not"
#define CT_N_STR_DEF_VAULTNAME      "not512.vlt"

#define CT_N_NOT_BEGINGOSTRF        "\r\n*** START OF DIGITAL SIGNATURE(S). NOTARY-512. ***\r\n"
#define CT_N_NOT_ENDGOSTRF          "**** END OF DIGITAL SIGNATURE(S). NOTARY-512. ****\r\n"
#define CT_N_NOT_BEGINDSA           "\r\n=== START OF DIGITAL SIGNATURE(S). NOTARY-512. ===\r\n"
#define CT_N_NOT_ENDDSA             "==== END OF DIGITAL SIGNATURE(S). NOTARY-512. ====\r\n"
#define CT_N_NOT_BEGINLANS          "\r\n--- START OF DIGITAL SIGNATURE(S). NOTARY-144. ---\r\n"
#define CT_N_NOT_ENDLANS            "---- END OF DIGITAL SIGNATURE(S). NOTARY-144. ----\r\n"

#define CT_N_PUB_BEGINOFPUBLICKEY  "======== START OF PUBLIC KEY. NOTARY-512. ========"
#define CT_N_PUB_ENDOFPUBLICKEY    "========= END OF PUBLIC KEY. NOTARY-512. ========="

#define CT_N_PUB_BEGINOFFPUBLICKEY "===== START OF FULL PUBLIC KEY. NOTARY-512. ======"
#define CT_N_PUB_ENDOFFPUBLICKEY   "====== END OF FULL PUBLIC KEY. NOTARY-512. ======="

#define CT_N_PUB_HEX                "D9"
#define CT_N_PUB_PRN                "PK"

#define CT_N_HASH_LEN_DSA           20
#define CT_N_HASH_LEN_GOST_RF       32
#define CT_N_HASH_LEN_LANS          9

#define CT_N_XKEY_LEN_DSA           20
#define CT_N_XKEY_LEN_GOST_RF       32
#define CT_N_XKEY_LEN_LANS          18

#define CT_N_RS_LEN_DSA             20
#define CT_N_RS_LEN_GOST_RF         32
#define CT_N_RS_LEN_LANS            18

#define CT_N_SIGN_LEN_DSA           20
#define CT_N_SIGN_LEN_GOST_RF       32
#define CT_N_SIGN_LEN_LANS          27

#define CT_N_NOT_BIN_DSA            "BS"
#define CT_N_NOT_BIN_GOST_RF        "BG"
#define CT_N_NOT_BIN_LANS           "BH"

#define CT_N_NOT_HEX_DSA            "D9"
#define CT_N_NOT_HEX_GOST_RF        "D0"
#define CT_N_NOT_HEX_LANS           "D1"

#define CT_N_NOT_PRN_DSA            "PS"
#define CT_N_NOT_PRN_GOST_RF        "PG"
#define CT_N_NOT_PRN_LANS           "PH"

#define CT_N_LEN_ONLYSN             20
#define CT_N_LEN_SN                 22
#define CT_N_LEN_USERNAME           128
#define CT_N_LEN_MAXXKEY            CT_N_XKEY_LEN_GOST_RF
#define CT_N_LEN_MAXHASH            CT_N_HASH_LEN_GOST_RF
#define CT_N_LEN_MAXRS              CT_N_RS_LEN_GOST_RF
#define CT_N_LEN_KEY                512
#define CT_N_LEN_SIGN               (CT_N_LEN_KEY/8)
#define CT_N_LEN_KKEY               (CT_N_LEN_KEY/8)
#define CT_N_LEN_DISK_ADDITION_KEY  (CT_N_LEN_KEY/8)
#define CT_N_LEN_TOUCH_ADDITION_KEY CT_N_LEN_MAXXKEY
#define CT_N_LEN_ADDITION_KEY       (CT_N_LEN_KEY/8)
#define CT_N_LEN_PRIVATEKEYFILE     (sizeof(CT_N_STR_PRIVATEKEY)+2*CT_N_LEN_ONLYSN+1+1+2+CT_N_LEN_USERNAME+CT_N_LEN_MAXXKEY+2+CT_N_LEN_KKEY+4+4+4+4+2)
#define CT_N_LEN_PUBLICKEYRECORD    (CT_N_LEN_ONLYSN+4+CT_N_LEN_USERNAME+4*CT_N_LEN_SIGN+4)

#define CT_N_TYP_SIGN_BINARY        0x0001
#define CT_N_TYP_SIGN_PRINTABLE     0x0002
#define CT_N_TYP_SIGN_HEXDECIMAL    0x0003

#define CT_N_TYP_DOS_STYLE          0x0000

#define CT_N_MAXLEN_WORKBUFFER      2048

#define CT_PRV_FLAGS_ADDITION_KEY_DISK              0x0001
#define CT_PRV_FLAGS_ADDITION_TOUCH_MEMORY          0x0002

#define CT_PUB_ACCESS_READ                          0x0000
#define CT_PUB_ACCESS_READ_WRITE                    0x0001

#define CT_PUB_WRITE_ADD                            0x0000
#define CT_PUB_WRITE_OVERWRITE                      0x0001

#define CT_FLAGS_PRIVATEKEYREAD                     0x0001
#define CT_FLAGS_PRIVATEKEYFILLED                   0x0002
#define CT_FLAGS_SERVICEINFOREAD                    0x0004
#define CT_FLAGS_PUBLICKEYREAD                      0x0008
#define CT_FLAGS_KEYDISKREAD                        0x0010
#define CT_FLAGS_TOUCHREAD                          0x0020
#define CT_FLAGS_FINDNEXT_ALLOWED                   0x0040

#define CT_FLAGS_INITHASHDONE                       0x0100
#define CT_FLAGS_HASHDONE                           0x0200

#define CT_FLAGS_CRYPTOINITDONE                     0x1000
#define CT_FLAGS_DECRYPTOINITDONE                   0x2000

#define CT_ERR_OK                                   0
#define CT_ERR_FILE_NOT_FOUND                       (-1)
#define CT_ERR_PASSWORD                             (-2)
#define CT_ERR_KEY_DISK                             (-3)
#define CT_ERR_TOUCH_MEMORY                         (-4)
#define CT_ERR_BAD_FILE                             (-5)
#define CT_ERR_UNKNOWN_USER                         (-6)
#define CT_ERR_READ_ERROR                           (-7)
#define CT_ERR_WRITE_ERROR                          (-8)
#define CT_ERR_NO_PRIVATE_KEY                       (-9)
#define CT_ERR_BREAK_SIGNALED                       (-10)
#define CT_ERR_VAULT_NOT_FOUND                      (-11)
#define CT_ERR_NO_MEMORY                            (-12)
#define CT_ERR_CREATE_TEMP_FILE                     (-13)
#define CT_ERR_CREATE_PRIVATE_KEY_FILE              (-14)
#define CT_ERR_CREATE_PUBLIC_KEY_FILE               (-15)
#define CT_ERR_CREATE_VAULT_FILE                    (-16)
#define CT_ERR_CREATE_NETWORK_FILE                  (-17)
#define CT_ERR_USER_ALREADY_EXIST                   (-18)
#define CT_ERR_CLIENT_VERSION_LIMITED               (-19)
#define CT_ERR_READ_BEFORE_SERVICE                  (-20)
#define CT_ERR_DISK_ADDITION_KEY_NEED               (-21)
#define CT_ERR_TOUCH_ADDITION_KEY_NEED              (-22)
#define CT_ERR_BAD_CRC                              (-23)
#define CT_ERR_PUBLIC_KEY_NOT_FOUND                 (-24)
#define CT_ERR_VAULT_IS_EMPTY                       (-25)
#define CT_ERR_EXPIRED_TIME                         (-26)
#define CT_ERR_ABOUT_EXPIRATION_TIME                (-27)
#define CT_ERR_TOO_LONG_VAULT_FILE_NAME             (-28)
#define CT_ERR_NO_TOUCH_MEMORY_PRESENT              (-29)
#define CT_ERR_NEED_TOUCH_MEMORY_CONNECT            (-30)
#define CT_ERR_BAD_TOUCH_MEMORY_INFO                (-31)
#define CT_ERR_TOUCH_OLD_VERSION                    (-32)
#define CT_ERR_WRONG_TOUCH_MEMORY                   (-33)
#define CT_ERR_NO_ADDITIONAL_KEY                    (-34)
#define CT_ERR_UNSUPPORTED_TOUCH_MEMORY             (-35)
#define CT_ERR_BAD_NUMBER_COMPORT                   (-36)
#define CT_ERR_CONNECTION_FAILAD                    (-37)

#define CT_ERR_N_BAD_SIGNATURE                      (-100)
#define CT_ERR_N_SIGNED_BY_ANOTHER_USER             (-101)
#define CT_ERR_N_SIGNATURE_NOT_FOUND                (-102)
#define CT_ERR_N_TOO_MANY_SIGNATURES                (-103)
#define CT_ERR_N_CAN_NOT_SIGN_FILE                  (-104)
#define CT_ERR_N_OLD_VERSION                        (-105)

#define CT_ERR_V_OK_BY_OLD_KEY                      (-200)
#define CT_ERR_V_CREATE_KEY                         (-201)
#define CT_ERR_V_NOT_FOR_YOU                        (-202)
#define CT_ERR_V_FILE_IS_NOT_ENCRYPTED              (-203)
#define CT_ERR_V_INVALID_VERSION                    (-204)
#define CT_ERR_V_UNKNOWN_COMPRESSION_METHOD         (-205)
#define CT_ERR_V_ANOTHER_RECEIVER                   (-206)
#define CT_ERR_V_ANOTHER_SENDER                     (-207)
#define CT_ERR_V_BAD_SHORTNAME                      (-208)
#define CT_ERR_V_SHORTNAME_ALREADY_EXIST            (-209)
#define CT_ERR_V_CREATE_OUTPUT_FILE                 (-210)
#define CT_ERR_V_UNKNOWN_TYPE                       (-211)
#define CT_ERR_V_OUTPUT_FILE_ALREADY_EXIST          (-212)
#define CT_ERR_V_WGROUP_KEY_NOT_FOUND               (-213)
#define CT_ERR_V_DIFFERENT_METHODS                  (-214)
#define CT_ERR_V_DIFFERENT_PUBLIC_KEY               (-215)

#define CT_ERR_TREAT_BEFORE_READ                    (-300)
#define CT_ERR_TREAT_BEFORE_OPEN                    (-301)
#define CT_ERR_INVALID_PARAMETER                    (-302)
#define CT_ERR_HASH_BEFORE_INIT                     (-303)
#define CT_ERR_SIGN_BEFORE_HASH_OR_KEY              (-304)
#define CT_ERR_NOTARY_BEFORE_HASH_OR_KEY            (-305)
#define CT_ERR_CREATE_BEFORE_KEY                    (-306)
#define CT_ERR_UNKNOWN_SIGNATURE_METHOD             (-307)
#define CT_ERR_SIGN_BEFORE_FILLED                   (-308)
#define CT_ERR_INIT_BEFORE_READ                     (-309)
#define CT_ERR_CRYPTO_BEFORE_INIT                   (-310)
#define CT_ERR_NEXT_BEFORE_FIRST                    (-311)
#define CT_ERR_BAD_SIGNATURE_METHOD                 (-312)
#define CT_ERR_USE_BEFORE_FILLED                    (-313)
#define CT_ERR_UNKNOWN_PICTURE_FILE                 (-314)
#define CT_ERR_WORKGROUP                            (-315)

#endif

