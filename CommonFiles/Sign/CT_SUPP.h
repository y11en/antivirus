#include "CT_TYPS.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "CT_SUP_D.h"
#include "CT_FIO.h"

#define TM_TRY_READ             10
#define TM_TRY_WRITE            10

CT_INT      CTAPI DecodeBuffer( CT_PTR, CT_INT *, CT_PTR, CT_INT );
CT_INT      CTAPI DecodeConstructor( CT_INT );
CT_INT      CTAPI DecodeEnd( CT_PTR );
CT_INT      CTAPI EncodeBuffer( CT_PTR, CT_INT *, CT_PTR, CT_INT );
CT_INT      CTAPI EncodeConstructor( CT_INT );
CT_INT      CTAPI EncodeEnd( CT_PTR );
CT_INT      CTAPI Hex2Bin( CT_PTR, CT_PTR, CT_INT, CT_INT );
CT_INT      CTAPI InitTouchPort( CT_INT );
CT_INT      CTAPI ReadTouchRAM( CT_UINT, CT_PTR, CT_UINT );
CT_INT      CTAPI ReadTouchROM( CT_PTR );
CT_INT      CTAPI URand( CT_INT );
CT_INT      CTAPI UUDecode( CT_PTR, CT_INT *, CT_PTR, CT_INT );
CT_INT      CTAPI UUEncode( CT_PTR, CT_INT *, CT_PTR, CT_INT );
CT_INT      CTAPI VerifySN( CT_PTR, CT_ULONG );
CT_INT      CTAPI WriteTouchRAM( CT_INT, CT_PTR, CT_INT );
CT_INT      CTAPI createrandfile( CT_PTR, CT_PTR );
CT_INT      CTAPI modilq_rf( CT_PTR, CT_PTR );
CT_INT      CTAPI modilq_us( CT_PTR, CT_PTR );
CT_INT      CTAPI updaterandfile( CT_PTR, CT_PTR, CT_INT );
CT_LONG     CTAPI GetURandValue( void );
CT_PTR      CTAPI AMemAlloc( CT_ULONG );
CT_PTR      CTAPI FindStr( CT_PTR, CT_INT );
CT_PTR      CTAPI splitname( CT_PTR, CT_PTR );
CT_UCHAR    CTAPI UUEncodeEnd( void );
CT_UINT     CTAPI updcrc16f( CT_UINT, CT_PTR , CT_UINT );
CT_ULONG    CTAPI AMemCoreleft( void );
CT_ULONG    CTAPI crc32( CT_ULONG, CT_PTR, CT_UINT );
void        CTAPI AMemFree( CT_PTR );
void        CTAPI Bin2Hex( CT_PTR, CT_PTR, CT_INT );
void        CTAPI CreateRandomValues( CT_PTR, CT_INT );
void        CTAPI Crypto( CT_PTR, CT_PTR, CT_INT, CT_INT );
void        CTAPI DecodeDestructor( void );
void        CTAPI EncodeDestructor( void );
void        CTAPI GetGostKey( CT_PTR );
void        CTAPI GostCode( CT_ULONG *, CT_ULONG * );
void        CTAPI GostDecode( CT_ULONG *, CT_ULONG * );
void        CTAPI InitFind( CT_PTR, CT_INT );
void        CTAPI Randomize( void );
void        CTAPI SetCryptoBlock( CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI SetGostKey( CT_PTR );
void        CTAPI SetURandValue( CT_LONG );
void        CTAPI UUDecodeInit( void );
void        CTAPI UUEncodeInit( void );
void        CTAPI VestaBuffer( CT_UINT *, CT_PTR, CT_UCHAR *, CT_UINT *, CT_UINT * );
void        CTAPI VestaInit( CT_UINT *, CT_PTR, CT_UCHAR *, CT_UINT *, CT_UINT * );
void        CTAPI _dsign_rf( CT_PTR, CT_PTR );
void        CTAPI _dsign_us( CT_PTR, CT_PTR );
void        CTAPI _invert_afn( CT_PTR, CT_PTR );
void        CTAPI _modilp_afn( CT_PTR, CT_PTR );
void        CTAPI _not_rf( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _not_us( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _reg_rf( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _reg_us( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _sign_rf( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _sign_us( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _dsign_sh( CT_PTR, CT_PTR );
void        CTAPI _signa_sh( CT_PTR, CT_PTR );
void        CTAPI _signb_sh( CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _reg_sh( CT_PTR, CT_PTR, CT_PTR );
void        CTAPI _not_sh( CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR, CT_PTR );
void        CTAPI afin1( CT_PTR, CT_PTR );
void        CTAPI afin2( CT_PTR, CT_PTR, CT_PTR );
void        CTAPI decrypr( CT_PTR, CT_PTR );
void        CTAPI gcrypt( CT_PTR, CT_PTR, CT_PTR );
void        CTAPI getstat( CT_PTR filename );
void        CTAPI randname( CT_PTR );
void        CTAPI removerandfile( CT_PTR, CT_INT );
#if defined( _WINDOWS ) || defined( _Windows )
void        CTAPI CloseTouchPort( void );
#endif
#ifdef __cplusplus
}
#endif

