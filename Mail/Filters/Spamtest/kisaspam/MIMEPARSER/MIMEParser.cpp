/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: MIMEParser.cpp                                                        *
 * Created: Wed Feb 20 23:20:13 2002                                           *
 * Desc: Some static data.                                                     *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  MIMEParser.cpp
 * \brief Some static data.
 */

#include "MIMEParser.hpp"

#define HEADERS_CONFIG_ENTRY(hc_name, hc_id) { hc_name, sizeof(hc_name)-1, hc_id }

MIMEParserGlobal::header_config MIMEParserGlobal::headers[] = {
    HEADERS_CONFIG_ENTRY("Content-type", MIMEParserGlobal::HDR_CONTENT_TYPE),
    HEADERS_CONFIG_ENTRY("MIME-Version", MIMEParserGlobal::HDR_MIME_VERSION),
    HEADERS_CONFIG_ENTRY("Content-transfer-encoding", MIMEParserGlobal::HDR_CONTENT_TRANSFER_ENCODING),
    { NULL, 0, MIMEParserGlobal::HDR_UNKNOWN }
};

#define CT_PARAMETERS_CONFIG_ENTRY(ctp_name, ctp_id) { ctp_name, sizeof(ctp_name)-1, ctp_id }

MIMEParserGlobal::ct_parameter_config MIMEParserGlobal::ct_parameters[] = {
    CT_PARAMETERS_CONFIG_ENTRY("charset", MIMEParserGlobal::CT_CHARSET),
    CT_PARAMETERS_CONFIG_ENTRY("boundary", MIMEParserGlobal::CT_BOUNDARY),
    CT_PARAMETERS_CONFIG_ENTRY("protocol", MIMEParserGlobal::CT_PROTOCOL),
    CT_PARAMETERS_CONFIG_ENTRY("name", MIMEParserGlobal::CT_NAME),
    { NULL, 0, MIMEParserGlobal::CT_UNKNOWN }
};

#define TRANSFER_ENCODING_CONFIG_ENTRY(tec_name, tec_id) { tec_name, sizeof(tec_name)-1, tec_id }

MIMEParserGlobal::transfer_encoding_config MIMEParserGlobal::transfer_encodings[] = {
    TRANSFER_ENCODING_CONFIG_ENTRY("7bit",             MIMEParserGlobal::TE_7BIT),
    TRANSFER_ENCODING_CONFIG_ENTRY("8bit",             MIMEParserGlobal::TE_8BIT),
    TRANSFER_ENCODING_CONFIG_ENTRY("binary",           MIMEParserGlobal::TE_BINARY),
    TRANSFER_ENCODING_CONFIG_ENTRY("quoted-printable", MIMEParserGlobal::TE_QP),
    TRANSFER_ENCODING_CONFIG_ENTRY("base64",           MIMEParserGlobal::TE_B64),

    /* Skipping x-uue and x-uuencode here, becouse they internal structure are equivalent 
     * to ordinary uue in text/plain and detected by QP detector (begin 644 filename).
     */

    { NULL, 0, MIMEParserGlobal::TE_UNKNOWN }
};

/* aaaack but it's fast and const should make it shared text page. */
unsigned char MIMEParserGlobal::pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};


int MIMEParserGlobal::mime_parser_decode_b64(const char *in, size_t length, char *out)
{
    int nbytesdecoded;
    register const unsigned char *bufin;
    register unsigned char *bufout;
    register int nprbytes;
    size_t i;

    bufin = (const unsigned char *) in;
    /* while (pr2six[*(bufin++)] <= 63 && length--); */
    /* while (length-- && pr2six[*(bufin++)] <= 63);
       nprbytes = (bufin - (const unsigned char *) in) - 1; */

    nprbytes=0;
    for(i=0; i < length; i++) {
        if(pr2six[bufin[i]] <= 63) 
            nprbytes++;
        else
            break;
    }

    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (unsigned char *) out;
    bufin = (const unsigned char *) in;

    while (nprbytes > 4) {
	*(bufout++) =
	    (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
	*(bufout++) =
	    (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
	*(bufout++) =
	    (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
	bufin += 4;
	nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
//    if(nprbytes==1)
//	syslog(6,"nprbytes==1");
    if (nprbytes > 1) {
	*(bufout++) =
	    (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
	*(bufout++) =
	    (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
	*(bufout++) =
	    (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}


/*
 * <eof MIMEParser.cpp>
 */
