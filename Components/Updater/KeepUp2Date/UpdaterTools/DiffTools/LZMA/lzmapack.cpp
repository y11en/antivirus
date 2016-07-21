/* LZMA compression C wrapper by Dmitry Glavatskikh */

#include <memory.h>
#include <stdlib.h>
#include "../shared/differr.h"
#include "../shared/types.h"
#include "lzmapack.h"

/* ------------------------------------------------------------------------- */

#ifdef WIN32
# include <initguid.h>
#else
# define INITGUID
#endif

#include "Common/MyWindows.h"
#include "7zip/Common/FileStreams.h"
#include "Common/StringConvert.h"
#include "7zip/Compress/LZMA/LZMADecoder.h"
#include "7zip/Compress/LZMA/LZMAEncoder.h"

/* ------------------------------------------------------------------------- */

class CInMemoryStream:
  public IInStream,
  public IStreamGetSize,
  public CMyUnknownImp
{
private:
  tBYTE* BaseBuff;
  tUINT  BuffSize;
  tUINT  BuffPosn;

public:
  CInMemoryStream()
  {
    BaseBuff = NULL;
    BuffSize = 0;
    BuffPosn = 0;
  }
  virtual ~CInMemoryStream() {}

  tBOOL Open(tBYTE* Buff, tUINT BufSize);

  MY_UNKNOWN_IMP2(IInStream, IStreamGetSize)

  STDMETHOD(Read)(tVOID* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(ReadPart)(tVOID* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(Seek)(Int64 Offset, UInt32 Origin, UInt64* NewPosition);

  STDMETHOD(GetSize)(UInt64* Size);
};

tBOOL CInMemoryStream::Open(tBYTE* Buff, tUINT BufSize)
{
  if ( NULL == Buff || !BufSize )
    return(cFALSE);

  BaseBuff = Buff;
  BuffSize = BufSize;
  BuffPosn = 0;
  return(cTRUE);
}

STDMETHODIMP CInMemoryStream::GetSize(UInt64* Size)
{
  if ( NULL == Size )
    return(E_FAIL);

  *Size = BuffSize;
  return(S_OK);
}

STDMETHODIMP CInMemoryStream::Read(tVOID* Data, UInt32 Size,
                                   UInt32* ProcessedSize)
{
  tUINT CanRead;

  if ( NULL == Data )
    return(E_FAIL);

  CanRead = (BuffSize - BuffPosn);
  CanRead = MIN(CanRead, Size);
  if ( NULL != ProcessedSize )
    *ProcessedSize = CanRead;

  memcpy(Data, &BaseBuff[BuffPosn], CanRead);
  BuffPosn += CanRead;
  return(S_OK);
}

STDMETHODIMP CInMemoryStream::ReadPart(tVOID* Data, UInt32 Size,
                                       UInt32* ProcessedSize)
{
  return Read(Data, Size, ProcessedSize);
}

STDMETHODIMP CInMemoryStream::Seek(Int64 Offset, UInt32 Origin,
                                   UInt64* NewPosition)
{
  return(E_FAIL); /* not implemented */
}

/* ------------------------------------------------------------------------- */

class COutMemoryStream: 
  public IOutStream,
  public CMyUnknownImp
{
private:
  tBYTE* BaseBuff;
  tUINT  MaxBSize;
  tUINT  BuffSize;
  tUINT  BuffPosn;

public:
  COutMemoryStream() {
    BaseBuff = NULL;
    MaxBSize = 0;
    BuffSize = 0;
    BuffPosn = 0;
  }
  virtual ~COutMemoryStream() {}
  tBOOL Open(tBYTE* Buff, tUINT BufSize);
  tUINT GetCurSize() { return(BuffPosn); }
  
  MY_UNKNOWN_IMP1(IOutStream)

  STDMETHOD(Write)(const tVOID* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(WritePart)(const tVOID* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(Seek)(Int64 Offset, UInt32 Origin, UInt64* NewPosition);
  STDMETHOD(SetSize)(Int64 NewSize);
};

tBOOL COutMemoryStream::Open(tBYTE* Buff, tUINT BufSize)
{
  if ( NULL == Buff || !BufSize )
    return(cFALSE);

  BaseBuff = Buff;
  MaxBSize = BufSize;
  BuffSize = BufSize;
  BuffPosn = 0;
  return(cTRUE);
}

STDMETHODIMP COutMemoryStream::SetSize(Int64 NewSize)
{
  if ( NewSize > MaxBSize )
    return(E_FAIL);

  BuffSize = (tUINT)(NewSize);
  return(S_OK);
}

STDMETHODIMP COutMemoryStream::Write(const tVOID* Data, UInt32 Size,
                                     UInt32* ProcessedSize)
{
  tUINT CanWrite;

  if ( NULL == Data )
    return(E_FAIL);

  CanWrite = (BuffSize - BuffPosn);
  CanWrite = MIN(CanWrite, Size);
  if ( NULL != ProcessedSize )
    *ProcessedSize = CanWrite;

  memcpy(&BaseBuff[BuffPosn], (void*)Data, CanWrite);
  BuffPosn += CanWrite;
  return(S_OK);
}

STDMETHODIMP COutMemoryStream::WritePart(const tVOID* Data, UInt32 Size,
                                         UInt32* ProcessedSize)
{
  return Write(Data, Size, ProcessedSize);
}

STDMETHODIMP COutMemoryStream::Seek(Int64 Offset, UInt32 Origin,
                                    UInt64* NewPosition)
{
  return(E_FAIL); /* not implemented */
}

/* ------------------------------------------------------------------------- */

static tINT DoLzmaPackMem2Mem(tBYTE* BSrc, tUINT  SrcSize,
                              tBYTE* BDst, tUINT* PDstSize)
{
  tUINT DstSize;
  HRESULT Result;

  /* check params */
  if ( NULL == BSrc || !SrcSize || NULL == BDst || NULL == PDstSize )
    return(DIFF_ERR_PARAM);
  if ( 0 == (DstSize = *PDstSize) )
    return(DIFF_ERR_PARAM);
  *PDstSize = 0;

#if 0 /* dummy pack */
  if ( SrcSize > DstSize )
    return(DIFF_ERR_NOMEM);
  memcpy(BDst, BSrc, SrcSize);
  *PDstSize = SrcSize;
  return(DIFF_ERR_OK);
#endif

  /* create input stream */
  CInMemoryStream* inStreamSpec = new CInMemoryStream;
  if ( !inStreamSpec->Open(BSrc, SrcSize) )
    return(DIFF_ERR_PACK);
  CMyComPtr<ISequentialInStream> inStream = inStreamSpec;

  /* create output stream */
  COutMemoryStream* outStreamSpec = new COutMemoryStream;
  if ( !outStreamSpec->Open(BDst, DstSize) )
    return(DIFF_ERR_PACK);
  CMyComPtr<ISequentialOutStream> outStream = outStreamSpec;

  PROPID propIDs[] = 
  {
    NCoderPropID::kDictionarySize,
    NCoderPropID::kPosStateBits,
    NCoderPropID::kLitContextBits,
    NCoderPropID::kLitPosBits,
    NCoderPropID::kAlgorithm,
    NCoderPropID::kNumFastBytes,
    NCoderPropID::kMatchFinder,
    NCoderPropID::kEndMarker
  };
  const int kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);

  /* make props */
  tUINT dictionary     = MIN(SrcSize, 32*1024*1024);
  tUINT posStateBits   = 2;
  tUINT litContextBits = 3;
  tUINT litPosBits     = 0;
  tUINT algorithm      = 2;
  tUINT numFastBytes   = 256;
  tBOOL eos            = cFALSE;
  UString mf           = L"BT4";

  /* create encoder propertyes */
  PROPVARIANT properties[kNumProps];
  for (int p = 0; p < 6; p++)
    properties[p].vt = VT_UI4;
  properties[0].ulVal = UInt32(dictionary);
  properties[1].ulVal = UInt32(posStateBits);
  properties[2].ulVal = UInt32(litContextBits);
  properties[3].ulVal = UInt32(litPosBits);
  properties[4].ulVal = UInt32(algorithm);
  properties[5].ulVal = UInt32(numFastBytes);
    
  properties[6].vt = VT_BSTR;
  properties[6].bstrVal = (BSTR)(const wchar_t *)mf;

  properties[7].vt = VT_BOOL;
  properties[7].boolVal = eos ? VARIANT_TRUE : VARIANT_FALSE;

  NCompress::NLZMA::CEncoder* encoderSpec;
  encoderSpec = new NCompress::NLZMA::CEncoder;
  CMyComPtr<ICompressCoder> encoder = encoderSpec;

  /* set encoder propertyes */
  Result = encoderSpec->SetCoderProperties(propIDs, properties, kNumProps);
  if ( Result != S_OK ) return(DIFF_ERR_PACK);

  /* encode block */
  Result = encoder->Code(inStream, outStream, NULL, NULL, NULL);
  if ( Result == E_OUTOFMEMORY )
  {
    return(DIFF_ERR_NOMEM);
  }   
  else if ( Result != S_OK )
  {
    return(DIFF_ERR_PACK);
  }   

  *PDstSize = outStreamSpec->GetCurSize();
  return(DIFF_ERR_OK); /* all ok */
}

/* ------------------------------------------------------------------------- */

extern "C" /* must be accessible from ANSI-C source */
tINT LzmaPackMem2Mem(tBYTE* BSrc, tUINT SrcSize, tBYTE* BDst, tUINT* PDstSize)
{
  try
  {
    return DoLzmaPackMem2Mem(BSrc, SrcSize, BDst, PDstSize);
  }
  catch(...)
  {
    return(DIFF_ERR_PACK);
  }
}

/* ------------------------------------------------------------------------- */
