/*-----------------22.12.98 13:43-------------------
 * TextToBin and BinToText converters             *
 *                                                *
 * header file                                    *
 *                                                *
 * Andrew Krukov, AVP Team                        *
--------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * both procedures can use static or dynamic conversion tables
 * static versions use additional static const data and better performance
 * dynamic versions use addition stack space
 */
#define TEXT2BIN_STATIC


#define T2B_LENGTH(scount) (((scount/4)*3) + (scount % 4))
/*
 * length of buffer to convert from text to bin 
 */

unsigned int TextToBin(const void *source, unsigned int scount, void *destination, unsigned int dcount);
/*
 * TextToBin procedure
 *
 * convert printable ASCII text to its original binary representation.
 *
 * parameters
 *      source - pointer to source buffer contaning already converted text
 *      scount - number of bytes of text to be converted form text to binary
 *      destination - pointer to destination buffer for storing binary data
 *      dcount - size of destination buffer in bytes. Buffer should be at
 *               least ((scount+3)/4)*3 bytes
 *
 * return values
 *      > 0 - number of bytes successfully written to destination buffer
 *
 *      0 - error, probably causes:
 *        - source or destination points to NULL
 *        - dcount smaller than ((scount+3)/4)*3 bytes
 */

#define B2T_LENGTH(scount) ( (scount/3)*4 + ( ((scount % 3)==2) ? 3 : (((scount % 3)==1) ? 2 : 0) ) )
/*
 * length of buffer to convert from bin to text
 */
unsigned int BinToText(const void *source, unsigned int scount, void *destination, unsigned int dcount);
/*
 * BinToText procedure
 *
 * convert binary data to printable ASCII text
 *
 * parameters
 *      source - pointer to source buffer contaning data to be converted.
 *      scount - number of bytes to convert to text.
 *      destination - pointer to destination buffer for storing resulting text
 *      dcount - size of destination buffer in bytes. Buffer should be at
 *               least ((scount+2)/3)*4 bytes
 *
 * return values
 *      > 0 - number of bytes successfully written to destination buffer
 *
 *      0 - error, no data was written. Probably causes:
 *        - source or destination points to NULL
 *        - dcount smaller than ((scount+2)/3)*4 bytes
 */


unsigned int BinToTextEx(const void *source, unsigned int scount, void *destination, unsigned int dcount);
/*
 * This function allows to use overlapped input/output buffers for expanding to text
 */

#ifdef __cplusplus
}
#endif
