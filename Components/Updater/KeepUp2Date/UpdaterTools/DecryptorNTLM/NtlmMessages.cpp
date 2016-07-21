#include "stdafx.h"

#include "NtlmMessages.h"



////////////////////////////////////////////////
/// Type1Message

CString Type1Message::toString()const
{
    const char *p = reinterpret_cast<const char *>(this);
    CString result;


    // host
    CString host;
    memcpy(host.GetBufferSetLength(host_len + 1), p + host_off, host_len);
    memset(host.GetBuffer(0) + host_len, 0, 1);

    // domain
    CString domain;
    memcpy(domain.GetBufferSetLength(dom_len + 1), p + dom_off, dom_len);
    memset(domain.GetBuffer(0) + dom_len, 0, 1);

    result.Format("protocol=%s;type=%d;flags=0x%x;dom_len=%d;host_len=%d;host=%s;dom=%s",
        protocol, (long)type, (long)ntohs(flags), (long)dom_len, (long)host_len, host.GetBuffer(0), domain.GetBuffer(0));

    return result;
}

////////////////////////////////////////////////
/// Type2Message


CString Type2Message::toString()const
{
    const char *p = reinterpret_cast<const char *>(this);
    CString result;

    // domain
    const unsigned nonceLength = 8;
    CString nonce;
    memcpy(nonce.GetBufferSetLength(nonceLength + 1), p + 24, nonceLength);
    memset(nonce.GetBuffer(0) + nonceLength, 0, 1);

    result.Format("protocol=%s;type=%d;msg_len=%d;flags=0x%x", protocol, (long)type, (long)msg_len, (long)ntohs(flags));

    return result;
}


////////////////////////////////////////////////
/// Type3Message

CString Type3Message::toString()const
{
    const char *p = reinterpret_cast<const char *>(this);
    CString result;

    // lm_resp
    CString lm_resp;
    memcpy(lm_resp.GetBufferSetLength(lm_resp_len + 1), p + lm_resp_off, lm_resp_len);
    memset(lm_resp.GetBuffer(0) + lm_resp_len, 0, 1);

    // nt_resp
    CString nt_resp;
    memcpy(nt_resp.GetBufferSetLength(nt_resp_len + 1), p + nt_resp_off, nt_resp_len);
    memset(nt_resp.GetBuffer(0) + nt_resp_len, 0, 1);

    // user
    CString user;
    memcpy(user.GetBufferSetLength(user_len + 1), p + user_off, user_len);
    memset(user.GetBuffer(0) + user_len, 0, 1);
    

    // host
    CString host;
    memcpy(host.GetBufferSetLength(host_len + 1), p + host_off, host_len);
    memset(host.GetBuffer(0) + host_len, 0, 1);

    // domain
    CString domain;
    memcpy(domain.GetBufferSetLength(dom_len + 1), p + dom_off, dom_len);
    memset(domain.GetBuffer(0) + dom_len, 0, 1);


    result.Format("protocol=%s;type=%d;flags=0x%x;user=%s;host=%s;dom=%s",
        protocol, (long)type, (long)ntohs(flags), user.GetBuffer(0), host.GetBuffer(0), domain.GetBuffer(0));

    return result;
}

