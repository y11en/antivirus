/* LZMA compression C wrapper by Dmitry Glavatskikh */

#include <memory.h>
#include <stdlib.h>
#include "lzma_config.h"
#include "lzma_encode.h"

/* ------------------------------------------------------------------------- */

#ifdef WIN32
# include <initguid.h>
#else
# define INITGUID
# define min(l,r) ((l)<(r))?(l):(r)
#endif

#include "lzmasdk/C/Common/MyWindows.h"
#include "lzmasdk/C/7zip/Common/FileStreams.h"
#include "lzmasdk/C/Common/StringConvert.h"
#include "lzmasdk/C/7zip/Compress/LZMA/LZMADecoder.h"
#include "lzmasdk/C/7zip/Compress/LZMA/LZMAEncoder.h"

/* ------------------------------------------------------------------------- */

class CInMemoryStream:
  public IInStream,
  public IStreamGetSize,
  public CMyUnknownImp
{
private:
  Byte*   BaseBuff;
  UInt32  BuffSize;
  UInt32  BuffPosn;

public:
  CInMemoryStream()
  {
    BaseBuff = NULL;
    BuffSize = 0;
    BuffPosn = 0;
  }
  virtual ~CInMemoryStream() {}

  int Open(Byte* Buff, UInt32 BufSize);

  MY_UNKNOWN_IMP2(IInStream, IStreamGetSize)

  STDMETHOD(Read)(void* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(ReadPart)(void* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(Seek)(Int64 Offset, UInt32 Origin, UInt64* NewPosition);

  STDMETHOD(GetSize)(UInt64* Size);
};

int CInMemoryStream::Open(Byte* Buff, UInt32 BufSize)
{
  if ( NULL == Buff || !BufSize )
    return(0);

  BaseBuff = Buff;
  BuffSize = BufSize;
  BuffPosn = 0;
  return(1);
}

STDMETHODIMP CInMemoryStream::GetSize(UInt64* Size)
{
  if ( NULL == Size )
    return(E_FAIL);

  *Size = BuffSize;
  return(S_OK);
}

STDMETHODIMP CInMemoryStream::Read(void* Data, UInt32 Size,
                                   UInt32* ProcessedSize)
{
  UInt32 CanRead;

  if ( NULL == Data )
    return(E_FAIL);

  CanRead = (BuffSize - BuffPosn);
  CanRead = min(CanRead, Size);
  if ( NULL != ProcessedSize )
    *ProcessedSize = CanRead;

  memcpy(Data, &BaseBuff[BuffPosn], CanRead);
  BuffPosn += CanRead;
  return(S_OK);
}

STDMETHODIMP CInMemoryStream::ReadPart(void* Data, UInt32 Size,
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
  Byte*   BaseBuff;
  UInt32  MaxBSize;
  UInt32  BuffSize;
  UInt32  BuffPosn;

public:
  COutMemoryStream() {
    BaseBuff = NULL;
    MaxBSize = 0;
    BuffSize = 0;
    BuffPosn = 0;
  }
  virtual ~COutMemoryStream() {}
  int     Open(Byte* Buff, UInt32 BufSize);
  UInt32  GetCurSize() { return(BuffPosn); }
  
  MY_UNKNOWN_IMP1(IOutStream)

  STDMETHOD(Write)(const void* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(WritePart)(const void* Data, UInt32 Size, UInt32* ProcessedSize);
  STDMETHOD(Seek)(Int64 Offset, UInt32 Origin, UInt64* NewPosition);
  STDMETHOD(SetSize)(Int64 NewSize);
};

int COutMemoryStream::Open(Byte* Buff, UInt32 BufSize)
{
  if ( NULL == Buff || !BufSize )
    return(0);

  BaseBuff = Buff;
  MaxBSize = BufSize;
  BuffSize = BufSize;
  BuffPosn = 0;
  return(1);
}

STDMETHODIMP COutMemoryStream::SetSize(Int64 NewSize)
{
  if ( NewSize > MaxBSize )
    return(E_FAIL);

  BuffSize = (UInt32)(NewSize);
  return(S_OK);
}

STDMETHODIMP COutMemoryStream::Write(const void* Data, UInt32 Size,
                                     UInt32* ProcessedSize)
{
  UInt32 CanWrite;

  if ( NULL == Data )
    return(E_FAIL);

  CanWrite = (BuffSize - BuffPosn);
  CanWrite = min(CanWrite, Size);
  if ( NULL != ProcessedSize )
    *ProcessedSize = CanWrite;
  if ( !CanWrite && Size )
    return(E_FAIL);

  if ( CanWrite )
    memcpy(&BaseBuff[BuffPosn], (void*)Data, CanWrite);

  BuffPosn += CanWrite;
  return(S_OK);
}

STDMETHODIMP COutMemoryStream::WritePart(const void* Data, UInt32 Size,
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

static int DoLzmaPackMem2Mem(Byte* BSrc, unsigned int  SrcSize,
                             Byte* BDst, unsigned int* pDstSize)
{
  UInt32  DstSize;
  HRESULT Result;

  /* check params */
  if ( NULL == BSrc || !SrcSize || NULL == BDst || NULL == pDstSize )
    return(LZMA_ENCODE_ERR_FAIL);
  if ( 0 == (DstSize = *pDstSize) )
    return(LZMA_ENCODE_ERR_FAIL);

  *pDstSize = 0;

  /* create input stream */
  CInMemoryStream* inStreamSpec = new CInMemoryStream;
  if ( !inStreamSpec->Open(BSrc, SrcSize) )
    return(LZMA_ENCODE_ERR_FAIL);
  CMyComPtr<ISequentialInStream> inStream = inStreamSpec;

  /* create output stream */
  COutMemoryStream* outStreamSpec = new COutMemoryStream;
  if ( !outStreamSpec->Open(BDst, DstSize) )
    return(LZMA_ENCODE_ERR_FAIL);
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
  UInt32  dictionary     = min(SrcSize, LZMA_CONF_MAXWINDOW);
  UInt32  posStateBits   = LZMA_CONF_STATEBITS;
  UInt32  litContextBits = LZMA_CONF_CONTEXTBITS;
  UInt32  litPosBits     = LZMA_CONF_POSITIONBITS;
  UInt32  algorithm      = LZMA_CONF_ALGORITHM;
  UInt32  numFastBytes   = LZMA_CONF_FASTBYTES;
  UInt32  eos            = LZMA_CONF_WRITEEOS;
  UString mf             = LZMA_CONF_MATCHFINDER;

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
  if ( Result != S_OK ) return(LZMA_ENCODE_ERR_FAIL);

  /* encode block */
  Result = encoder->Code(inStream, outStream, NULL, NULL, NULL);
  if ( Result == E_OUTOFMEMORY )
  {
    return(LZMA_ENCODE_ERR_NOMEM);
  }   
  else if ( Result != S_OK )
  {
    return(LZMA_ENCODE_ERR_FAIL);
  }   

  *pDstSize = outStreamSpec->GetCurSize();
  return(LZMA_ENCODE_ERR_OK); /* all ok */
}

/* ------------------------------------------------------------------------- */

extern "C" /* must be accessible from ANSI-C source */
int LzmaEncodeMem2Mem(const void* Source, unsigned int  SrcSize,
                      void*  Destination, unsigned int* pDstSize)
{
  try
  {
    return DoLzmaPackMem2Mem((Byte*)Source, SrcSize, (Byte*)Destination, pDstSize);
  }
  catch(...)
  {
    return(LZMA_ENCODE_ERR_FAIL);
  }
}

/* ------------------------------------------------------------------------- */
