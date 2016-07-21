#include <Prague/prague.h>

#include <AV/plugin/p_avlib.h>

#include "isarcbuf.h"

//////////////////////////////////////////////////////////////////////////

typedef tINT    kav_int, kav_int_fast32;
typedef tBYTE   kav_data8, kav_unaligned_uint8, kav_uint8;
typedef tWORD   kav_data16, kav_le_udata16, kav_unaligned_uint16, kav_uint16;
typedef tDWORD  kav_data32, kav_le_udata32, kav_unaligned_uint32, kav_uint32;

/**
   Return a multibyte value of type kav_uintN. If this value is stored
in memory, then the sequence of bytes in it would be (b0, b1, b2, ...)
*/

#define VALUE_POS_IN_NATIVE_VALUE(Offset, NBytes)   (Offset)

#define         NATIVE_SEQ8(b0)                                          \
   ((kav_uint8)b0)

#define         NATIVE_SEQ16(b0, b1)                                     \
   ((kav_uint16)(kav_uint8)b0 << (8 * VALUE_POS_IN_NATIVE_VALUE(0, 2)) | \
    (kav_uint16)(kav_uint8)b1 << (8 * VALUE_POS_IN_NATIVE_VALUE(1, 2)))

#define         NATIVE_SEQ32(b0, b1, b2, b3)                             \
   ((kav_uint32)(kav_uint8)b0 << (8 * VALUE_POS_IN_NATIVE_VALUE(0, 4)) | \
    (kav_uint32)(kav_uint8)b1 << (8 * VALUE_POS_IN_NATIVE_VALUE(1, 4)) | \
    (kav_uint32)(kav_uint8)b2 << (8 * VALUE_POS_IN_NATIVE_VALUE(2, 4)) | \
    (kav_uint32)(kav_uint8)b3 << (8 * VALUE_POS_IN_NATIVE_VALUE(3, 4)))

#define         NATIVE_SEQ64(b0, b1, b2, b3, b4, b5, b6, b7)             \
   ((kav_uint64)(kav_uint8)b0 << (8 * VALUE_POS_IN_NATIVE_VALUE(0, 8)) | \
    (kav_uint64)(kav_uint8)b1 << (8 * VALUE_POS_IN_NATIVE_VALUE(1, 8)) | \
    (kav_uint64)(kav_uint8)b2 << (8 * VALUE_POS_IN_NATIVE_VALUE(2, 8)) | \
    (kav_uint64)(kav_uint8)b3 << (8 * VALUE_POS_IN_NATIVE_VALUE(3, 8)) | \
    (kav_uint64)(kav_uint8)b4 << (8 * VALUE_POS_IN_NATIVE_VALUE(4, 8)) | \
    (kav_uint64)(kav_uint8)b5 << (8 * VALUE_POS_IN_NATIVE_VALUE(5, 8)) | \
    (kav_uint64)(kav_uint8)b6 << (8 * VALUE_POS_IN_NATIVE_VALUE(6, 8)) | \
    (kav_uint64)(kav_uint8)b7 << (8 * VALUE_POS_IN_NATIVE_VALUE(7, 8)))


#define     BP(offset)  (*(kav_data8 *)(Page+offset))
#define     WP(offset)  (*(kav_data16*)(Page+offset))
#define     DP(offset)  (*(kav_data32*)(Page+offset))

#define M1(ptr,x1)          (*(const kav_unaligned_uint8 *)(CheckInPage+ptr) == NATIVE_SEQ8(x1))
#define M2(ptr,x1,x2)       (*(const kav_unaligned_uint16*)(CheckInPage+ptr) == NATIVE_SEQ16(x1,x2))
#define M3(ptr,x1,x2,x3)    (M2(ptr,x1,x2) && M1(ptr+2,x3))
#define M4(ptr,x1,x2,x3,x4) (*(const kav_unaligned_uint32*)(CheckInPage+ptr) == NATIVE_SEQ32(x1,x2,x3,x4))

#define      M5(ptr,x1,x2,x3,x4,x5)\
(M4(ptr,x1,x2,x3,x4) && M1(ptr+4,x5))
#define      M6(ptr,x1,x2,x3,x4,x5,x6)\
(M4(ptr,x1,x2,x3,x4) && M2(ptr+4,x5,x6))
#define      M7(ptr,x1,x2,x3,x4,x5,x6,x7)\
(M4(ptr,x1,x2,x3,x4) && M3(ptr+4,x5,x6,x7))
#define      M8(ptr,x1,x2,x3,x4,x5,x6,x7,x8)\
(M4(ptr,x1,x2,x3,x4) && M4(ptr+4,x5,x6,x7,x8))
#define      M9(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9)\
(M8(ptr,x1,x2,x3,x4,x5,x6,x7,x8) && M1(ptr+8,x9))
#define     M10(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)\
(M8(ptr,x1,x2,x3,x4,x5,x6,x7,x8) && M2(ptr+8,x9,x10))
#define     M11(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11)\
(M8(ptr,x1,x2,x3,x4,x5,x6,x7,x8) && M3(ptr+8,x9,x10,x11))
#define     M12(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12)\
(M8(ptr,x1,x2,x3,x4,x5,x6,x7,x8) && M4(ptr+8,x9,x10,x11,x12))
#define     M13(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13)\
(M12(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12) && M1(ptr+12,x13))
#define     M14(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14)\
(M12(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12) && M2(ptr+12,x13,x14))
#define     M15(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15)\
(M12(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12) && M3(ptr+12,x13,x14,x15))
#define     M16(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16)\
(M12(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12) && M4(ptr+12,x13,x14,x15,x16))
#define     M17(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17)\
(M16(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16) && M1(ptr+16,x17))
#define     M18(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18)\
(M16(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16) && M2(ptr+16,x17,x18))
#define     M19(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19)\
(M16(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16) && M3(ptr+16,x17,x18,x19))
#define     M20(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20)\
(M16(ptr,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16) && M4(ptr+16,x17,x18,x19,x20))

//////////////////////////////////////////////////////////////////////////

tERROR pr_call AVLIB_IsArchiveFormatBuff(tBYTE* pInData, tDWORD dwSize, tDWORD* pdwResult)
{
    const kav_data8 * const Page = pInData;
    const kav_data8 * const CheckInPage = Page;
    tDWORD ptr;

	if (pdwResult == NULL || pInData == NULL || dwSize == 0)
		return errPARAMETER_INVALID;

    // fast scan of zero-filles caves
    for (ptr = 0; ptr < dwSize; ptr++)
    {
        if (BP(ptr) != 0)
            break;
    }

    for (ptr = 0; ptr < dwSize; ptr++)
    {
        if (ptr + 0x10 >= dwSize)
            break;

        switch (WP(ptr))
        {
            case 0x6152:                    // "Rar!" 52 61 72 21 1A 07 00 xx xx 73
                    if (DP(ptr+2) != 0x071A2172)    break;
                    if (BP(ptr+6) != 0)             break;
                    if (BP(ptr+9) != 0x73)          break;
                    goto Found;

            case 0x4B50:                    // "PK"
                    // Printf("PK at %04X : %04X %04X",ptr,((kav_le_udata16*)(Page+ptr+2))[0],((kav_le_udata16*)(Page+ptr+4))[0]);
                    if (WP(ptr+2) != 0x0403)        break;  // Zip ??
                    if (WP(ptr+4) > 0x100)          break;
                    goto Found;

            case 0x534D:                    // MSCF - Microsoft Cabinet     Printf("MSCF at %04X ?",ptr);
                    if (ptr + 0x1A >= dwSize)       break;
                    if (WP(ptr+2) != 0x4643)        break;  // CF
                    if (WP(ptr+0x18) != 0x103)      break;  // version = 0103h
                    goto Found;

            case 0x5A53:                    // Microsoft self-installer             Printf("MS Pack? %04X",ptr);
                    if (DP(ptr+2) != 0xF0884444)    break;  // SZDD F088h
                    goto Found;
        }

        // ACE
        if (M7 (ptr + 0x07,     '*', '*', 'A', 'C', 'E', '*', '*'))
            goto Found;

        //LHA
        if (M3 (ptr + 0x02,   '-', 'l', 'h') && M1 (ptr + 0x06, '-'))
            goto Found;

        // Paquet
        if (ptr + 0x22 < dwSize && M3 (ptr + 0x01,   0xA4, 0xBE, 0xC4) && M2 (ptr + 0x20,  'G', 'D'))
            goto Found;

        // ARJ
        if (M2 (ptr + 0x00,   0x60, 0xEA) && WP(ptr + 2) <= 0x500 && BP (ptr + 0x0A) == 2)
        {        
            if (ptr + WP(ptr + 2) + 0x0A + 2 < dwSize)
            {
                if (WP(ptr + WP(ptr + 2) + 0x0A) == 0xEA60)
                    goto Found;
            }
//            else
//            {
//                return errBUFFER_TOO_SMALL;
//            }
        }

        // AppleSingle
        if (M4 (ptr + 0x00,   0x00, 0x05, 0x16, 0x00))
            goto Found;

        // BZIP2
        if (M3 (ptr + 0x00,   0x42, 0x5A, 0x68) && M6 (ptr + 0x04,  0x31, 0x41, 0x59, 0x26, 0x53, 0x59))
            goto Found;

        // CHM
        if (M12(ptr + 0x00,   'I', 'T', 'S', 'F', 0x03, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00))
            goto Found;
        
        // DISKDUPE 5.1
        if (M9 (ptr + 0x00,   'M', 'S', 'D', ' ', 'I', 'm', 'a', 'g', 'e'))
            goto Found;

        // RTF
        if (M6 (ptr + 0x00,   '{', '\\', 'r', 't', 'f', '1'))
            goto Found;

        // GEA
        if (M4 (ptr + 0x00,   'G', 'E', 'A', '\0'))
            goto Found;
    
        // GZIP
        if (M3 (ptr + 0x00,   0x1F, 0x8B, 0x08))
            goto Found;
    
        // HA
        if (M2 (ptr + 0x00,   'H', 'A'))
        {
            const kav_int_fast32 type = BP(ptr + 4) - 0x20;
            if ((type >= 0x00 && type <= 0x02) || type == 0x0E || type == 0x0F)
                goto Found;
        }
    
        // MSO
        if (M14(ptr + 0x00,   'A', 'c', 't', 'i', 'v', 'e', 'M', 'i', 'm', 'e', 0x00, 0x00, 0x01, 0xF0))
            goto Found;
    
        // SIS
        if (M4 (ptr + 0x08,   0x19, 0x04, 0x00, 0x10))
            goto Found;
    
        // SPIS
        if (M5 (ptr + 0x00,   'S', 'P', 'I', 'S', 0x1A))
            goto Found;

        // TAR
        if (ptr + 0x106 < dwSize && M5 (ptr + 0x101,   'u', 's', 't', 'a', 'r'))
            goto Found;
    
        // DISKDUPE
        // DiskImage
        // Effect Office
        // ProCarry
        // Stream 1C
        // TeleDisk
        // WinBackup
    }    

	*pdwResult = 0;
	return errOK;

Found:
	*pdwResult = cAVLIB_ARCHIVE_FORMAT;
	return errOK;
}

#if (TEST_AVLIB_IS_ARCHIVE_FORMAT_BUFF)

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char * argv[])
{
    tBYTE buf[0x200];
    tDWORD res = 0;

    {
        FILE * fp = fopen (argv[1], "rb");
        int i;

        if (!fp)
        {
            puts ("-1");
            return -1;
        }

        for (i = 0; i < sizeof (buf) / 4; i++)
        {
            buf[i] = rand();
        }

        if (fread (buf + sizeof (buf) / 4, 1, 3 * sizeof (buf) / 4, fp) <= 0)
//        if (fread (buf+1, 1, sizeof (buf)-1, fp) <= 0)
        {
            return -2;
            puts ("-2");
        }

        fclose (fp);
    }

    {
        tERROR err = AVLIB_IsArchiveFormatBuff (buf, sizeof (buf), &res);
        
        if (PR_FAIL(err))
        {
            return -3;
            puts ("-3");
        }
    }

    if (res != cAVLIB_ARCHIVE_FORMAT)
    {
        return -5;
    }

    return 0;
}

#endif /* TEST_AVLIB_IS_ARCHIVE_FORMAT_BUFF */
