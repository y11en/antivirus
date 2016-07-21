#ifndef _PROTO_H_
#define _PROTO_H_

#include <Prague/prague.h>

#include "const.h"

#ifdef __cplusplus
 extern "C" {
#endif

// ----------------------------------------------------------------------------

// from rar.h
RSTATIC void    RATTRIB CopyString(tRAR *data, uint len, uint dist);
RSTATIC void    RATTRIB MakeDecodeTables(unsigned char *LenTab,Decode *Dec,int Size);
RSTATIC int     RATTRIB DecodeNumber(tINPUT *input, Decode *Dec);
RSTATIC void    RATTRIB UnpWriteBuf(tRAR *data);
RSTATIC void    RATTRIB UnpInitData(tRAR *data, rarbool Solid);
RSTATIC void    RATTRIB UnpInitData20(tRAR *data, int Solid);
RSTATIC void    RATTRIB Unpack29(tRAR *data, tINPUT *input, rarbool Solid);
RSTATIC void    RATTRIB Unpack20(tRAR *data, tINPUT *input, rarbool Solid);
RSTATIC void    RATTRIB UnpackCreate(tRAR *data, uint size);
// RSTATIC rarbool RATTRIB ArchiveMerge(tRAR *data, tINPUT *input, tRAR_FILE_HEADER *fh);
RSTATIC dword   RATTRIB CRC32(dword crc, byte *buf, uint size);

// ----------------------------------------------------------------------------

// from input.h
RSTATIC rarbool RATTRIB   FileBufferUpdate   (struct sINPUT *input);
RSTATIC void    RATTRIB   InputInitSrc       (struct sINPUT *input, struct sINPUT *source, byte (RATTRIB *GetByte)(struct sINPUT *input));
RSTATIC void    RATTRIB   InputInitFile      (struct sINPUT *input, tRAR *rardata, tBYTE* Buff, tUINT BuffSize);
RSTATIC void    RATTRIB   InputInitBuf       (struct sINPUT *input, void *buf, uint limit);
RSTATIC void    RATTRIB   InputSetLimit      (struct sINPUT *input, uint limit);
RSTATIC word    RATTRIB   BitGet             (struct sINPUT *input);
RSTATIC void    RATTRIB   BitAdd             (struct sINPUT *input, uint bits);
RSTATIC void    RATTRIB   BitAlign           (struct sINPUT *input);
RSTATIC byte    RATTRIB   RarReadByteCrc     (struct sINPUT *input);
RSTATIC word    RATTRIB   RarReadWordCrc     (struct sINPUT *input);
RSTATIC byte    RATTRIB   RarReadByte        (struct sINPUT *input);
RSTATIC word    RATTRIB   RarReadWord        (struct sINPUT *input);
RSTATIC dword   RATTRIB   RarReadDword       (struct sINPUT *input);
RSTATIC dword   RATTRIB   RarReadDwordCrc    (struct sINPUT *input);
RSTATIC uint    RATTRIB   RarReadCrc         (struct sINPUT *input, void *buf, uint count);
RSTATIC filesize RATTRIB  RarSkipCrc         (struct sINPUT *input, uint count);
RSTATIC filesize RATTRIB  RarTell            (struct sINPUT *input);
RSTATIC filesize RATTRIB  RarSeek            (struct sINPUT *input, filesize offset );
RSTATIC void    RATTRIB   BufferUnpackStart  (struct sINPUT *input, filesize limit );
RSTATIC void    RATTRIB   BufferUnpackStop   (struct sINPUT *input);
RSTATIC void    RATTRIB   BufferCleanUp      (struct sINPUT *input);
RSTATIC byte    RATTRIB   RarReadByteStream  (tINPUT *input);
RSTATIC byte    RATTRIB   RarReadBytePPM     (tINPUT *input);

// ----------------------------------------------------------------------------

// from mem.h
RSTATIC void   RATTRIB  RarFreeAll(struct sRAR *data);
RSTATIC void   RATTRIB  RarFree   (struct sRAR *data, void *ptr);
RSTATIC void * RATTRIB  RarAlloc  (struct sRAR *data, uint size, tMEMFIXPROC MemFix);
RSTATIC void   RATTRIB  ArrayInit(tARRAY *array, uint size);
RSTATIC void   RATTRIB  ArrayDelete(struct sRAR *data, tARRAY *array);
RSTATIC rarbool RATTRIB ArrayFix(struct sRAR *data, tARRAY *array);
#ifdef MEM_DEBUG
RSTATIC void * RATTRIB  iArrayAdd(struct sRAR *data, tARRAY *array, uint count, char *comment);
#else
RSTATIC void * RATTRIB  iArrayAdd(struct sRAR *data, tARRAY *array, uint count);
#endif
RSTATIC void   RATTRIB  MemReset      (struct sRAR *data);
RSTATIC void   RATTRIB  MemInit       (struct sRAR *data, uint unitsize, uint count);
#ifdef MEM_DEBUG
RSTATIC void * RATTRIB  iMemRealloc   (struct sRAR *data, uint *allocsize, void *oldptr, uint newsize, char *comment);
#else
RSTATIC void * RATTRIB  iMemRealloc   (struct sRAR *data, uint *allocsize, void *oldptr, uint newsize);
#endif
RSTATIC void * RATTRIB  MemPtrFix(struct sRAR *data, void *ptr);
RSTATIC void   RATTRIB  MemFree       (struct sRAR *data, void *oldptr);
RSTATIC void   RATTRIB  MemScan       (struct sRAR *data);
RSTATIC void   RATTRIB  MemMove       (void *dst, void *src, uint count);
#ifdef MEM_DEBUG
RSTATIC void   RATTRIB  MemCheck      (struct sRAR *data);
RSTATIC void   RATTRIB  MemDump       (struct sRAR *data);
RSTATIC void   RATTRIB  ArrayDump     (tARRAY *array, char *comment);
#endif

// ----------------------------------------------------------------------------

// from model.h
RSTATIC int     RATTRIB PPM_DecodeChar(struct sRAR *data, struct sINPUT *input);
RSTATIC rarbool RATTRIB PPM_DecodeInit(struct sRAR *data, struct sINPUT *input, int *EscChar);

// ----------------------------------------------------------------------------

// from rarvm.h
RSTATIC rarbool RATTRIB VM_ReadCode   (struct sRAR *data, struct sINPUT *input);
RSTATIC rarbool RATTRIB VM_AddCode    (struct sRAR *data, uint FirstByte, struct sINPUT *input);
RSTATIC void    RATTRIB VM_SetMemory  (struct sRAR *data, uint pos, byte *buf, uint count);
RSTATIC rarbool RATTRIB VM_Execute    (struct sRAR *data, tSTACK_FILTER *filter);
RSTATIC void    RATTRIB VM_PrepareOperands(tVM *vm, tFILTER *filter);
RSTATIC rarbool RATTRIB VM_FilterFix  (struct sRAR *data);
RSTATIC void    RATTRIB VM_InitFilters(struct sRAR *data);
#ifdef MACHINE_IS_BIG_ENDIAN
RSTATIC void    RATTRIB VM_SetDwordValue(void *ptr, uint value);
RSTATIC uint    RATTRIB VM_GetDwordValue(void *ptr);
#endif // MACHINE_IS_BIG_ENDIAN
#ifndef VM_VALUE_DEFINE
RSTATIC void RATTRIB VM_SetByteValue (void *ptr, uint value);
RSTATIC void RATTRIB VM_SetDwordValue(void *ptr, uint value);
RSTATIC uint RATTRIB VM_GetByteValue (void *ptr);
RSTATIC uint RATTRIB VM_GetDwordValue(void *ptr);
RSTATIC uint RATTRIB VM_GetValue     (rarbool ByteMode, void *ptr);
RSTATIC void RATTRIB VM_SetValue     (rarbool ByteMode, void *ptr, uint value);
#else  // VM_VALUE_DEFINE
#define VM_SetByteValue(ptr,value)   (*((byte*)(ptr)) = (byte)(value))
#ifndef MACHINE_IS_BIG_ENDIAN
#define VM_SetDwordValue(ptr,value)  (*((uint*)(ptr)) = value)
#define VM_GetDwordValue(ptr)        (*((uint*)(ptr)))
#endif // MACHINE_IS_BIG_ENDIAN
#define VM_GetByteValue(ptr)         (*((byte*)(ptr)))
#define VM_GetValue(ByteMode,ptr)        (ByteMode ? VM_GetByteValue(ptr) : VM_GetDwordValue(ptr))
#define VM_SetValue(ByteMode,ptr,value)  (ByteMode ? VM_SetByteValue(ptr, value) : VM_SetDwordValue(ptr, value))
#endif // VM_VALUE_DEFINE

// ----------------------------------------------------------------------------

// from stdflt.h
RSTATIC uint   RATTRIB VM_StandardFilterCheck(tINPUT *input);
RSTATIC void   RATTRIB VM_StandardFilterExecute(tVM *vm, uint type);
RSTATIC byte * RATTRIB VM_FilterName(uint num);

// ----------------------------------------------------------------------------

// from suballoc.h
RSTATIC void    RATTRIB SA_InsertNode         (struct sRAR *data, void *p, uint indx);
RSTATIC void *  RATTRIB SA_RemoveNode         (struct sRAR *data, int indx);
RSTATIC void    RATTRIB SA_SplitBlock         (struct sRAR *data, void *pv, int OldIndx, int NewIndx);
RSTATIC void    RATTRIB SA_StopSubAllocator   (struct sRAR *data);
RSTATIC rarbool RATTRIB SA_StartSubAllocator  (struct sRAR *data, int SASize);
RSTATIC void    RATTRIB SA_InitSubAllocator   (struct sRAR *data);
RSTATIC void    RATTRIB SA_GlueFreeBlocks     (struct sRAR *data);
RSTATIC void *  RATTRIB SA_AllocUnitsRare     (struct sRAR *data, int  indx);
RSTATIC void *  RATTRIB SA_AllocUnits         (struct sRAR *data, uint NU);
RSTATIC void *  RATTRIB SA_AllocContext       (struct sRAR *data);
RSTATIC void *  RATTRIB SA_ExpandUnits        (struct sRAR *data, void *OldPtr,int OldNU);
RSTATIC void *  RATTRIB SA_ShrinkUnits        (struct sRAR *data, void *OldPtr,int OldNU,int NewNU);
RSTATIC void    RATTRIB SA_FreeUnits          (struct sRAR *data, void *ptr,int OldNU);
#ifdef SA_DEBUG
RSTATIC void RATTRIB SA_Dump(struct sRAR *data, int dump);
#else
#define SA_Dump(a,b)
#endif

// ----------------------------------------------------------------------------

// from pack20.c
tUINT ReadData        (tPACK20* Data, tINT FirstBuf);
tVOID WriteCode       (tPACK20* Data);
tVOID PackData        (tPACK20* Data, tINT Solid);
tVOID SearchMatch     (tPACK20* Data, tINT Depth);
tVOID PackInit        (tPACK20* Data, tINT Solid, tINT Depth);
tVOID ClearPackData   (tPACK20* Data);
tVOID CompressGeneral (tPACK20* Data);
tVOID Pack            (tPACK20* Data, tBYTE* Addr,
  tUINT (*PackRead)(tPACK20*,tBYTE*,tUINT), 
  tVOID (*CodeBlock)(tPACK20*), tUINT Solid, tUINT Method,
  tBYTE* CodeBufAddr, tUINT CodeBufSize, tLONG WindowSize);

// add on in common.c
tUINT Pack20Read(tPACK20* Data,  tBYTE* Addr, tUINT Count);
tUINT Pack20Write(tPACK20* Data, tBYTE* Addr, tUINT Count);

// from cblock.c
void CodeBlock(tPACK20* data);
void WriteTables(tPACK20* data, BitLengths *LengthTable);
void fputbits(tPACK20* data, int n, unsigned int x);
void WriteBuf(tPACK20* data);
void SetSecondFreq(tPACK20* data, unsigned int *TabFreq,int TabSize);
void FlushBuf(tPACK20* data);
void WriteBuf(tPACK20* data);
void FlushCodeBuffer(tPACK20* data);
void PackInitData(tPACK20* data, int Solid);

// from mtree.c
int  make_tree(tPACK20* data, int nparm, unsigned int freqparm[],
               unsigned char lenparm[], unsigned int codeparm[]);

// ----------------------------------------------------------------------------

// from rijndael.c
tVOID AES_Create(tAES_CTX* Ctx);
tVOID AES_Init(tAES_CTX* Ctx, tINT dir, tBYTE* key, tBYTE* initVector);
tINT  AES_blockDecrypt(tAES_CTX* Ctx,tBYTE* input,tINT inputLen,tBYTE* outBuffer);

// ----------------------------------------------------------------------------

// from sha1.c
tVOID SHA1_Initial(tSHA1_CTX* c);
tVOID SHA1_Process(tSHA1_CTX* c, tBYTE* data, tUINT len);
tVOID SHA1_Final(tSHA1_CTX* c, tUINT digest[5]);

// ----------------------------------------------------------------------------

// from crypt.c
tVOID RAR_SetCryptKeys(tCRYPT_DATA* Ctx,tCHAR* Password,tBYTE* Salt,tBOOL Encrypt,tBOOL OldOnly);
tVOID RAR_SetAV15Encryption(tCRYPT_DATA* Ctx);
tVOID RAR_SetCmt13Encryption(tCRYPT_DATA* Ctx);
tVOID RAR_DecryptBlock20(tCRYPT_DATA* Ctx, tBYTE* Buf);
tVOID RAR_DecryptBlock(tCRYPT_DATA* Ctx, tBYTE* Buf, tINT Size);
tVOID RAR_Crypt(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count, tINT Method);
tVOID RAR_SetSalt(tCRYPT_DATA* Ctx, tBYTE* Salt);
tVOID RAR_SetEncryption(tRAR* RAR, tINT Method, tCHAR* Password, tBYTE* Salt);

// ----------------------------------------------------------------------------

// from common.h
tDWORD PackRead(tRAR* RarData, tBYTE* Buff, tDWORD Cnt);
tDWORD FileWrite(tRAR* RarData, tBYTE* Mem, tDWORD Cnt);
tQWORD PackSeek(tRAR* RarData, tQWORD Offset, tDWORD Origin);
tINT   Rotate(tRAR* RarData);
tVOID* Malloc(tRAR* RarData, tUINT Cnt);
tVOID  Free(tRAR* RarData, tVOID* Mem);
tERROR ArchiveMerge(tRAR* Data, tINPUT* Input, tRAR_FILE_HEADER* FH);

#define Printf

// ----------------------------------------------------------------------------

// from unpack15.c
tVOID Unpack15(tRAR* data, tINPUT* input, rarbool Solid);

// from pack20.c
void Pack20Proc(tPACK20* data, tBYTE *Addr,  unsigned int (* PackRead)
  (tPACK20* data, tBYTE *,unsigned int), void (* CodeBlock)
  (tPACK20* data), unsigned int Solid, unsigned int Method,
   tBYTE *CodeBufAddr,unsigned int CodeBufSize,long WindowSize);

// ----------------------------------------------------------------------------

#ifdef __INTEL_COMPILER

// inlined crt redefinitions for intel c compiler
static __inline tVOID* memset(tVOID* Ptr, tINT Var, tUINT Cnt)
{
  tVOID* Res = Ptr;
  while(Cnt--)
  {
    *(tBYTE*)(Ptr) = (tBYTE)(Var);
    Ptr = (tVOID*)((tBYTE*)(Ptr) + sizeof(tBYTE));
  }
  return(Res);
}

#endif // __INTEL_COMPILER

// ----------------------------------------------------------------------------

#ifdef __cplusplus
 }; // extern "C"
#endif

#endif // _PROTO_H_
