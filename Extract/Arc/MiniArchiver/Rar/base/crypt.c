#include <Prague/prague.h>
#include <Prague/iface/i_string.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

static tVOID Encode13(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count);
static tVOID Decode13(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count);
static tVOID Crypt15(tCRYPT_DATA*  Ctx, tBYTE* Data, tUINT Count);
static tVOID UpdKeys(tCRYPT_DATA*  Ctx, tBYTE* Buf);
static tVOID Swap(tBYTE* Ch1, tBYTE* Ch2);
static tVOID SetOldKeys(tCRYPT_DATA*  Ctx, tCHAR* Password);
static tVOID EncryptBlock20(tCRYPT_DATA* Ctx, tBYTE* Buf);

// ----------------------------------------------------------------------------

#define NROUNDS 32
#define rol(x,n,xsize)  (((x)<<(n)) | ((x)>>(xsize-(n))))
#define ror(x,n,xsize)  (((x)>>(n)) | ((x)<<(xsize-(n))))
#define substLong(t) ( (uint)Ctx->SubstTable[(uint)t&255] | \
           ((uint)Ctx->SubstTable[(int)(t>> 8)&255]<< 8) | \
           ((uint)Ctx->SubstTable[(int)(t>>16)&255]<<16) | \
           ((uint)Ctx->SubstTable[(int)(t>>24)&255]<<24) )


static const tBYTE InitSubstTable[256] = {
  215, 19,149, 35, 73,197,192,205,249, 28, 16,119, 48,221,  2, 42,
  232,  1,177,233, 14, 88,219, 25,223,195,244, 90, 87,239,153,137,
  255,199,147, 70, 92, 66,246, 13,216, 40, 62, 29,217,230, 86,  6,
   71, 24,171,196,101,113,218,123, 93, 91,163,178,202, 67, 44,235,
  107,250, 75,234, 49,167,125,211, 83,114,157,144, 32,193,143, 36,
  158,124,247,187, 89,214,141, 47,121,228, 61,130,213,194,174,251,
   97,110, 54,229,115, 57,152, 94,105,243,212, 55,209,245, 63, 11,
  164,200, 31,156, 81,176,227, 21, 76, 99,139,188,127, 17,248, 51,
  207,120,189,210,  8,226, 41, 72,183,203,135,165,166, 60, 98,  7,
  122, 38,155,170, 69,172,252,238, 39,134, 59,128,236, 27,240, 80,
  131,  3, 85,206,145, 79,154,142,159,220,201,133, 74, 64, 20,129,
  224,185,138,103,173,182, 43, 34,254, 82,198,151,231,180, 58, 10,
  118, 26,102, 12, 50,132, 22,191,136,111,162,179, 45,  4,148,108,
  161, 56, 78,126,242,222, 15,175,146, 23, 33,241,181,190, 77,225,
    0, 46,169,186, 68, 95,237, 65, 53,208,253,168,  9, 18,100, 52,
  116,184,160, 96,109, 37, 30,106,140,104,150,  5,204,117,112, 84
};

tVOID RAR_DecryptBlock(tCRYPT_DATA* Ctx, tBYTE* Buf, tINT Size)
{
  AES_blockDecrypt(&Ctx->rin, Buf, Size, Buf);
}

#define uint32  tUINT

static tVOID EncryptBlock20(tCRYPT_DATA* Ctx, tBYTE* Buf)
{
  uint A,B,C,D,T,TA,TB;
  uint32 *BufPtr=(uint32*)Buf;
  tINT I;

  A=BufPtr[0]^Ctx->Key[0];
  B=BufPtr[1]^Ctx->Key[1];
  C=BufPtr[2]^Ctx->Key[2];
  D=BufPtr[3]^Ctx->Key[3];

  for(I=0;I<NROUNDS;I++)
  {
    T=((C+rol(D,11,32))^Ctx->Key[I&3]);
    TA=A^substLong(T);
    T=((D^rol(C,17,32))+Ctx->Key[I&3]);
    TB=B^substLong(T);
    A=C;
    B=D;
    C=TA;
    D=TB;
  }

  BufPtr[0]=C^Ctx->Key[0];
  BufPtr[1]=D^Ctx->Key[1];
  BufPtr[2]=A^Ctx->Key[2];
  BufPtr[3]=B^Ctx->Key[3];

  UpdKeys(Ctx, Buf);
}

tVOID RAR_DecryptBlock20(tCRYPT_DATA* Ctx, tBYTE* Buf)
{
  byte InBuf[16];
  uint A,B,C,D,T,TA,TB;
  tINT I;
  uint32* BufPtr=(uint32*)Buf;

  A=BufPtr[0]^Ctx->Key[0];
  B=BufPtr[1]^Ctx->Key[1];
  C=BufPtr[2]^Ctx->Key[2];
  D=BufPtr[3]^Ctx->Key[3];

  memcpy(InBuf,Buf,sizeof(InBuf));
  for(I=NROUNDS-1;I>=0;I--)
  {
    T=((C+rol(D,11,32))^Ctx->Key[I&3]);
    TA=A^substLong(T);
    T=((D^rol(C,17,32))+Ctx->Key[I&3]);
    TB=B^substLong(T);
    A=C;
    B=D;
    C=TA;
    D=TB;
  }

  BufPtr[0]=C^Ctx->Key[0];
  BufPtr[1]=D^Ctx->Key[1];
  BufPtr[2]=A^Ctx->Key[2];
  BufPtr[3]=B^Ctx->Key[3];

  UpdKeys(Ctx, InBuf);
}

extern tDWORD CRC_DATA[256];

static tVOID UpdKeys(tCRYPT_DATA* Ctx, tBYTE* Buf)
{
  tINT I;

  for (I = 0; I < 16; I +=4 )
  {
    Ctx->Key[0]^=CRC_DATA[Buf[I]];
    Ctx->Key[1]^=CRC_DATA[Buf[I+1]];
    Ctx->Key[2]^=CRC_DATA[Buf[I+2]];
    Ctx->Key[3]^=CRC_DATA[Buf[I+3]];
  }
}

static __inline tVOID Swap(tBYTE* Ch1, tBYTE* Ch2)
{
  tBYTE Ch;
  
  Ch   = *Ch1;
  *Ch1 = *Ch2;
  *Ch2 = Ch;
}

static __inline tBYTE* WideToRaw(tWORD* Src, tBYTE* Dest, tINT DestSize)
{
  int I;

  for (I=0;I<DestSize;I++,Src++)
  {
    Dest[I*2]=(byte)*Src;
    Dest[I*2+1]=(byte)(*Src>>8);
    if (*Src==0)
      break;
  }
  return(Dest);
}

tVOID RAR_SetCryptKeys(tCRYPT_DATA* Ctx,tCHAR* Password,tBYTE* Salt,tBOOL Encrypt,tBOOL OldOnly)
{
  tBYTE xN1, xN2;
  tINT  Len, PswLength, I, J, K;
  tBOOL Cached;
  tBYTE Psw[MAXPASSWORD];

  if (*Password==0)
    return;

  SetOldKeys(Ctx, Password);
  Ctx->Key[0]=0xD3A3B879L;
  Ctx->Key[1]=0x3F6D12F7L;
  Ctx->Key[2]=0x7515A235L;
  Ctx->Key[3]=0xA4E7F123L;
  memset(Psw, 0, sizeof(Psw));
  Len = _toi32(strlen(Password));
  Len = (Len > MAXPASSWORD-4) ? (MAXPASSWORD-4) : (Len);
  memcpy(&Psw[0], Password, Len+1);

  PswLength = Len;
  memcpy(Ctx->SubstTable, (tVOID*)(&InitSubstTable[0]), sizeof(Ctx->SubstTable));

  for ( J=0; J < 256; J++ )
  {
    for ( I=0; I < PswLength; I+=2 )
    {
      xN1= (tBYTE)CRC_DATA[(tUINT)((Psw[I]-J)&0xff)];
      xN2= (tBYTE)CRC_DATA[(tUINT)((Psw[I+1]+J)&0xff)];
      for ( K=1; (tUINT)(xN1&0xFF) != (tUINT)(xN2&0xFF); xN1++,K++ )
      {
        Swap(&Ctx->SubstTable[xN1], &Ctx->SubstTable[(xN1+I+K)&0xff]);
      }
    }
  }

  for ( I=0; I < PswLength; I+=16 )
    EncryptBlock20(Ctx, &Psw[I]);

  if (OldOnly)
    return;

  Cached = cFALSE;
  for (I=0;I<sizeof(Ctx->Cache)/sizeof(Ctx->Cache[0]);I++)
    if (strcmp(Ctx->Cache[I].Password, Password) == 0 &&
        ((Salt == NULL && !Ctx->Cache[I].SaltPresent) || (Salt!=NULL &&
        Ctx->Cache[I].SaltPresent && memcmp(Ctx->Cache[I].Salt,Salt,SALT_SIZE)==0)))
    {
      memcpy(Ctx->AESKey, Ctx->Cache[I].AESKey, sizeof(Ctx->AESKey));
      memcpy(Ctx->AESInit,Ctx->Cache[I].AESInit,sizeof(Ctx->AESInit));
      Cached=cTRUE;
      break;
    }

  if (!Cached)
  {
    tSHA1_CTX c;
    const tINT HashRounds=0x40000;
    tWORD PswW[MAXPASSWORD];
    tBYTE RawPsw[2*MAXPASSWORD+SALT_SIZE];
    tINT  RawLength, I, J;
    tBYTE PswNum[3];
    tUINT digest[5];

    PswW[0] = 0;
    if ( NULL != Ctx->hPswd )
    {
      CALL_String_ExportToBuff(Ctx->hPswd, NULL, cSTRING_WHOLE, &PswW[0], MAXPASSWORD-4,
        cCP_UNICODE, cSTRING_Z);
    }

    PswW[MAXPASSWORD-1]=0;
    WideToRaw(PswW,RawPsw,MAXPASSWORD-1);
    RawLength = 2*Len;
    if (Salt!=NULL)
    {
      memcpy(RawPsw+RawLength,Salt,SALT_SIZE);
      RawLength+=SALT_SIZE;
    }
    
    SHA1_Initial(&c);

    for (I=0;I<HashRounds;I++)
    {
      SHA1_Process(&c, RawPsw, RawLength);
      PswNum[0] = (byte)I;
      PswNum[1] = (byte)(I>>8);
      PswNum[2] = (byte)(I>>16);
      SHA1_Process(&c, PswNum, 3);
      if (I%(HashRounds/16)==0)
      {
        tSHA1_CTX tempc;
        tUINT digest[5];
        
        memcpy(&tempc, &c, sizeof(tSHA1_CTX));
        SHA1_Final(&tempc, digest);
        Ctx->AESInit[I/(HashRounds/16)] = (byte)digest[4];
      }
    }

    SHA1_Final(&c, digest);
    for (I=0;I<4;I++)
      for (J=0;J<4;J++)
        Ctx->AESKey[I*4+J] = (byte)(digest[I]>>(J*8));

    strcpy_s(Ctx->Cache[Ctx->CachePos].Password,countof(Ctx->Cache[Ctx->CachePos].Password),Password);
    if ((Ctx->Cache[Ctx->CachePos].SaltPresent = (Salt!=NULL))!=cFALSE)
      memcpy(Ctx->Cache[Ctx->CachePos].Salt,Salt,SALT_SIZE);
    memcpy(Ctx->Cache[Ctx->CachePos].AESKey,Ctx->AESKey,sizeof(Ctx->AESKey));
    memcpy(Ctx->Cache[Ctx->CachePos].AESInit,Ctx->AESInit,sizeof(Ctx->AESInit));
    Ctx->CachePos=(Ctx->CachePos+1)%(sizeof(Ctx->Cache)/sizeof(Ctx->Cache[0]));
  }

  AES_Create(&Ctx->rin);
  AES_Init(&Ctx->rin, Encrypt ? AES_ENCRYPT : AES_DECRYPT, Ctx->AESKey, Ctx->AESInit);
}

static tVOID SetOldKeys(tCRYPT_DATA* Ctx, tCHAR* Password)
{
  tDWORD PswCRC;
  tBYTE  Ch;
  
  PswCRC = _toui32(CRC32(0xffffffff, Password, (uint)strlen(Password)));
  Ctx->OldKey[0] = PswCRC&0xffff;
  Ctx->OldKey[1] = (PswCRC>>16)&0xffff;
  Ctx->OldKey[2] = Ctx->OldKey[3] = 0;
  Ctx->PN1=Ctx->PN2=Ctx->PN3=0;

  while ((Ch=*Password)!=0)
  {
    Ctx->PN1+=Ch;
    Ctx->PN2^=Ch;
    Ctx->PN3+=Ch;
    Ctx->PN3=(byte)rol(Ctx->PN3,1,8);
    Ctx->OldKey[2]^=Ch^CRC_DATA[Ch];
    Ctx->OldKey[3]+=Ch+(CRC_DATA[Ch]>>16);
    Password++;
  }
}


tVOID RAR_SetAV15Encryption(tCRYPT_DATA* Ctx)                            
{
  Ctx->OldKey[0]=0x4765;
  Ctx->OldKey[1]=0x9021;
  Ctx->OldKey[2]=0x7382;
  Ctx->OldKey[3]=0x5215;
}


tVOID RAR_SetCmt13Encryption(tCRYPT_DATA* Ctx)
{
  Ctx->PN1=0;
  Ctx->PN2=7;
  Ctx->PN3=77;
}


tVOID RAR_Crypt(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count, tINT Method)
{
  if (Method==OLD_DECODE)
    Decode13(Ctx, Data,Count);
  else
    if (Method==OLD_ENCODE)
      Encode13(Ctx, Data,Count);
    else
      Crypt15(Ctx, Data,Count);
}


static tVOID Encode13(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count)
{
  while (Count--)
  {
    Ctx->PN2+=Ctx->PN3;
    Ctx->PN1+=Ctx->PN2;
    *Data+=Ctx->PN1;
    Data++;
  }
}


static tVOID Decode13(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count)
{
  while (Count--)
  {
    Ctx->PN2+=Ctx->PN3;
    Ctx->PN1+=Ctx->PN2;
    *Data-=Ctx->PN1;
    Data++;
  }
}


static tVOID Crypt15(tCRYPT_DATA* Ctx, tBYTE* Data, tUINT Count)
{
  while (Count--)
  {
    Ctx->OldKey[0]+=0x1234;
    Ctx->OldKey[1]^=CRC_DATA[(Ctx->OldKey[0] & 0x1fe)>>1];
    Ctx->OldKey[2]-=CRC_DATA[(Ctx->OldKey[0] & 0x1fe)>>1]>>16;
    Ctx->OldKey[0]^=Ctx->OldKey[2];
    Ctx->OldKey[3]=ror(Ctx->OldKey[3]&0xffff,1,16)^Ctx->OldKey[1];
    Ctx->OldKey[3]=ror(Ctx->OldKey[3]&0xffff,1,16);
    Ctx->OldKey[0]^=Ctx->OldKey[3];
    *Data^=(byte)(Ctx->OldKey[0]>>8);
    Data++;
  }
}

tVOID RAR_SetEncryption(tRAR* RAR, tINT Method, tCHAR* Password, tBYTE* Salt)
{
  RAR_SetCryptKeys(&RAR->Crypt, Password, Salt, cFALSE, Method < 29);
  RAR->Crypt.Version = (Method);
}

