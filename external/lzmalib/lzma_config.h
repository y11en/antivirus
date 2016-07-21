#ifndef _LZMA_CONFIG_H_
#define _LZMA_CONFIG_H_  "LZMA Compressor/Decompresor Options"

/* ------------------------------------------------------------------------- */

#define LZMA_CONF_MAXWINDOW       (32*1024*1024)
#define LZMA_CONF_STATEBITS       (2)       /* 0 .. 4   */
#define LZMA_CONF_CONTEXTBITS     (3)       /* 0 .. 8   */
#define LZMA_CONF_POSITIONBITS    (0)       /* 0 .. 4   */
#define LZMA_CONF_ALGORITHM       (2)       /* ?        */
#define LZMA_CONF_FASTBYTES       (256)     /* 5 .. 273 */
#define LZMA_CONF_WRITEEOS        (0)       /* 0 .. 1   */
#define LZMA_CONF_MATCHFINDER     (L"BT4")  /* bt2, bt3, bt4, hc4 */

/* ------------------------------------------------------------------------- */

#endif /* _LZMA_CONFIG_H_ */
