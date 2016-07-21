#if !defined(NTLM_MESSAGES_H__8883A9FD_7B06_4038_B3EB_93C9EB08A9A7)
#define NTLM_MESSAGES_H__8883A9FD_7B06_4038_B3EB_93C9EB08A9A7

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(1)


struct Type1Message
{
    // output structure as readable string
    CString toString()const;


    byte    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    byte    type;            // 0x01
    byte    zero1[3];
    unsigned short flags;           // 0xb203
    byte    zero2[2];

    short   dom_len;         // domain string length
    short   dom_len1;         // domain string length
    short   dom_off;         // domain string offset
    byte    zero3[2];

    short   host_len;        // host string length
    short   host_len1;        // host string length
    short   host_off;        // host string offset (always 0x20)
    byte    zero4[2];

    byte    host[256];         // host string (ASCII)
    //byte    dom;          // domain string (ASCII)
};

struct Type2Message
{
    // output structure as readable string
    CString toString()const;


    byte    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    byte    type;            // 0x02
    byte    zero1[7];
    short   msg_len;         // 0x28
    byte    zero2[2];
    unsigned short flags;           // 0x8201
    byte    zero3[2];

    byte    nonce[8];        // nonce
    byte    zero4[8 + 2048]; // zero4 + pad
};


struct Type3Message
{
    // output structure as readable string
    CString toString()const;


    byte    protocol[8];     // 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
    byte    type;            // 0x03
    byte    zero1[3];

    short   lm_resp_len;     // LanManager response length (always 0x18)
    short   lm_resp_len2;     // LanManager response length (always 0x18)
    short   lm_resp_off;     // LanManager response offset
    byte    zero2[2];

    short   nt_resp_len;     // NT response length (always 0x18)
    short   nt_resp_len2;     // NT response length (always 0x18)
    short   nt_resp_off;     // NT response offset
    byte    zero3[2];

    short   dom_len;         // domain string length
    short   dom_len2;         // domain string length
    short   dom_off;         // domain string offset (always 0x40)
    byte    zero4[2];

    short   user_len;        // username string length
    short   user_len2;        // username string length
    short   user_off;        // username string offset
    byte    zero5[2];

    short   host_len;        // host string length
    short   host_len2;        // host string length
    short   host_off;        // host string offset
    byte    zero6[6];

    short   msg_len;         // message length
    byte    zero7[2];

    unsigned short flags;           // 0x8201
    byte    zero8[2];

    byte    dom[2048];          // domain string (unicode UTF-16LE)
//    byte    user[*];         // username string (unicode UTF-16LE)
//    byte    host[*];         // host string (unicode UTF-16LE)
//    byte    lm_resp[*];      // LanManager response
//    byte    nt_resp[*];      // NT response
};


#endif // !defined(NTLM_MESSAGES_H__8883A9FD_7B06_4038_B3EB_93C9EB08A9A7)