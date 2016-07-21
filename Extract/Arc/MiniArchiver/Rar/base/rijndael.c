#include <Prague/prague.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

// private functions
static tVOID AES_keySched(tAES_CTX* Ctx, tBYTE key[AES_MAX_KEY_COLUMNS][4]);
static tVOID AES_keyEncToDec(tAES_CTX* Ctx);
static tVOID AES_decrypt(tAES_CTX* Ctx, tBYTE a[16], tBYTE b[16]);
static tVOID AES_GenerateTables(tAES_CTX* Ctx);

#define uKeyLenInBytes (16)
#define m_uRounds      (10)

#define PRESENT_INT32
#define uint32  tUINT

__inline void AES_Xor128_3(byte *dest,const byte *arg1,const byte *arg2)
{
#ifdef PRESENT_INT32
  ((uint32*)dest)[0]=((uint32*)arg1)[0]^((uint32*)arg2)[0];
  ((uint32*)dest)[1]=((uint32*)arg1)[1]^((uint32*)arg2)[1];
  ((uint32*)dest)[2]=((uint32*)arg1)[2]^((uint32*)arg2)[2];
  ((uint32*)dest)[3]=((uint32*)arg1)[3]^((uint32*)arg2)[3];
#else
  for (int I=0;I<16;I++)
    dest[I]=arg1[I]^arg2[I];
#endif
}


__inline void AES_Xor128_5(byte *dest,const byte *arg1,const byte *arg2,
                    const byte *arg3,const byte *arg4)
{
#ifdef PRESENT_INT32
  (*(uint32*)dest)=(*(uint32*)arg1)^(*(uint32*)arg2)^(*(uint32*)arg3)^(*(uint32*)arg4);
#else
  for (int I=0;I<4;I++)
    dest[I]=arg1[I]^arg2[I]^arg3[I]^arg4[I];
#endif
}


__inline void AES_Copy128(byte *dest,const byte *src)
{
#ifdef PRESENT_INT32
  ((uint32*)dest)[0]=((uint32*)src)[0];
  ((uint32*)dest)[1]=((uint32*)src)[1];
  ((uint32*)dest)[2]=((uint32*)src)[2];
  ((uint32*)dest)[3]=((uint32*)src)[3];
#else
  for (int I=0;I<16;I++)
    dest[I]=src[I];
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tVOID AES_Create(tAES_CTX* Ctx)
{
  if (Ctx->S[0]==0)
    AES_GenerateTables(Ctx);
}

#define m_direction   Ctx->m_direction
#define m_initVector  Ctx->m_initVector
#define m_expandedKey Ctx->m_expandedKey

tVOID AES_Init(tAES_CTX* Ctx, tINT dir, tBYTE* key, tBYTE* initVector)
{
  tBYTE keyMatrix[AES_MAX_KEY_COLUMNS][4];
  tINT  i;

  m_direction = dir;
  for(i = 0;i < uKeyLenInBytes;i++)
    keyMatrix[i >> 2][i & 3] = key[i]; 

  for(i = 0;i < AES_MAX_IV_SIZE;i++)
    m_initVector[i] = initVector[i];

  AES_keySched(Ctx, keyMatrix);

  if(m_direction == AES_DECRYPT)
    AES_keyEncToDec(Ctx);
}
  
tINT AES_blockDecrypt(tAES_CTX* Ctx,tBYTE* input,tINT inputLen,tBYTE* outBuffer)
{
  tINT  numBlocks, i;
  tBYTE block[16], iv[4][4];


  if (input == 0 || inputLen <= 0)
    return 0;

  memcpy(iv,m_initVector,16); 
  numBlocks=inputLen/16;
  for (i = numBlocks; i > 0; i--)
  {
    AES_decrypt(Ctx, input, block);
    AES_Xor128_3(block, block,(byte*)iv);
#if STRICT_ALIGN
    memcpy(iv, input, 16);
    memcpy(outBuffer, block, 16);
#else
    AES_Copy128((byte*)iv,input);
    AES_Copy128(outBuffer,block);
#endif
    input += 16;
    outBuffer += 16;
  }

  memcpy(m_initVector,iv,16);
  return 16*numBlocks;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ALGORITHM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static tVOID AES_keySched(tAES_CTX* Ctx, tBYTE key[AES_MAX_KEY_COLUMNS][4])
{
  tBYTE tempKey[AES_MAX_KEY_COLUMNS][4];
  tINT  j, rconpointer = 0;
  tINT  uKeyColumns, r, t, k;

  // Calculate the necessary round keys
  // The number of calculations depends on keyBits and blockBits
  uKeyColumns = m_uRounds - 6;


  // Copy the input key to the temporary key matrix
  memcpy(tempKey,key,sizeof(tempKey));

  r = 0;
  t = 0;

  // copy values into round key array
  for( j = 0; (j < uKeyColumns) && (r <= m_uRounds); )
  {
    for(;(j < uKeyColumns) && (t < 4); j++, t++)
      for (k=0;k<4;k++)
        m_expandedKey[r][t][k]=tempKey[j][k];

    if(t == 4)
    {
      r++;
      t = 0;
    }
  }
    
  while(r <= m_uRounds)
  {
    tempKey[0][0] ^= Ctx->S[tempKey[uKeyColumns-1][1]];
    tempKey[0][1] ^= Ctx->S[tempKey[uKeyColumns-1][2]];
    tempKey[0][2] ^= Ctx->S[tempKey[uKeyColumns-1][3]];
    tempKey[0][3] ^= Ctx->S[tempKey[uKeyColumns-1][0]];
    tempKey[0][0] ^= Ctx->rcon[rconpointer++];

    if (uKeyColumns != 8)
      for(j = 1; j < uKeyColumns; j++)
        for (k=0;k<4;k++)
          tempKey[j][k] ^= tempKey[j-1][k];
    else
    {
      for(j = 1; j < uKeyColumns/2; j++)
        for (k=0;k<4;k++)
          tempKey[j][k] ^= tempKey[j-1][k];

      tempKey[uKeyColumns/2][0] ^= Ctx->S[tempKey[uKeyColumns/2 - 1][0]];
      tempKey[uKeyColumns/2][1] ^= Ctx->S[tempKey[uKeyColumns/2 - 1][1]];
      tempKey[uKeyColumns/2][2] ^= Ctx->S[tempKey[uKeyColumns/2 - 1][2]];
      tempKey[uKeyColumns/2][3] ^= Ctx->S[tempKey[uKeyColumns/2 - 1][3]];
      for(j = uKeyColumns/2 + 1; j < uKeyColumns; j++)
        for (k=0;k<4;k++)
          tempKey[j][k] ^= tempKey[j-1][k];
    }
    for(j = 0; (j < uKeyColumns) && (r <= m_uRounds); )
    {
      for(; (j < uKeyColumns) && (t < 4); j++, t++)
        for (k=0;k<4;k++)
          m_expandedKey[r][t][k] = tempKey[j][k];
      if(t == 4)
      {
        r++;
        t = 0;
      }
    }
  }   
}

static tVOID AES_keyEncToDec(tAES_CTX* Ctx)
{
  tBYTE  n_expandedKey[4][4];
  tINT   r, i, j;
  tBYTE* w;

  for(r = 1; r < m_uRounds; r++)
  {
    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
      {
        w = m_expandedKey[r][j];
        n_expandedKey[j][i]=Ctx->U1[w[0]][i]^Ctx->U2[w[1]][i]^Ctx->U3[w[2]][i]^Ctx->U4[w[3]][i];
      }
    memcpy(m_expandedKey[r],n_expandedKey,sizeof(m_expandedKey[0]));
  }
} 


static tVOID AES_decrypt(tAES_CTX* Ctx, tBYTE a[16], tBYTE b[16])
{
  tBYTE temp[4][4];
  tINT  r;
  
  AES_Xor128_3((byte*)temp,(byte*)a,(byte*)m_expandedKey[m_uRounds]);

  AES_Xor128_5(b,   Ctx->T5[temp[0][0]],Ctx->T6[temp[3][1]],Ctx->T7[temp[2][2]],Ctx->T8[temp[1][3]]);
  AES_Xor128_5(b+4, Ctx->T5[temp[1][0]],Ctx->T6[temp[0][1]],Ctx->T7[temp[3][2]],Ctx->T8[temp[2][3]]);
  AES_Xor128_5(b+8, Ctx->T5[temp[2][0]],Ctx->T6[temp[1][1]],Ctx->T7[temp[0][2]],Ctx->T8[temp[3][3]]);
  AES_Xor128_5(b+12,Ctx->T5[temp[3][0]],Ctx->T6[temp[2][1]],Ctx->T7[temp[1][2]],Ctx->T8[temp[0][3]]);

  for(r = m_uRounds-1; r > 1; r--)
  {
    AES_Xor128_3((byte*)temp,(byte*)b,(byte*)m_expandedKey[r]);
    AES_Xor128_5(b,   Ctx->T5[temp[0][0]],Ctx->T6[temp[3][1]],Ctx->T7[temp[2][2]],Ctx->T8[temp[1][3]]);
    AES_Xor128_5(b+4, Ctx->T5[temp[1][0]],Ctx->T6[temp[0][1]],Ctx->T7[temp[3][2]],Ctx->T8[temp[2][3]]);
    AES_Xor128_5(b+8, Ctx->T5[temp[2][0]],Ctx->T6[temp[1][1]],Ctx->T7[temp[0][2]],Ctx->T8[temp[3][3]]);
    AES_Xor128_5(b+12,Ctx->T5[temp[3][0]],Ctx->T6[temp[2][1]],Ctx->T7[temp[1][2]],Ctx->T8[temp[0][3]]);
  }
 
  AES_Xor128_3((byte*)temp,(byte*)b,(byte*)m_expandedKey[1]);
  b[ 0] = Ctx->S5[temp[0][0]];
  b[ 1] = Ctx->S5[temp[3][1]];
  b[ 2] = Ctx->S5[temp[2][2]];
  b[ 3] = Ctx->S5[temp[1][3]];
  b[ 4] = Ctx->S5[temp[1][0]];
  b[ 5] = Ctx->S5[temp[0][1]];
  b[ 6] = Ctx->S5[temp[3][2]];
  b[ 7] = Ctx->S5[temp[2][3]];
  b[ 8] = Ctx->S5[temp[2][0]];
  b[ 9] = Ctx->S5[temp[1][1]];
  b[10] = Ctx->S5[temp[0][2]];
  b[11] = Ctx->S5[temp[3][3]];
  b[12] = Ctx->S5[temp[3][0]];
  b[13] = Ctx->S5[temp[2][1]];
  b[14] = Ctx->S5[temp[1][2]];
  b[15] = Ctx->S5[temp[0][3]];
  AES_Xor128_3((byte*)b,(byte*)b,(byte*)m_expandedKey[0]);
}

#define ff_poly 0x011b
#define ff_hi   0x80

#define FFinv(x)    ((x) ? pow[255 - log[x]]: 0)

#define FFmul02(x) (x ? pow[log[x] + 0x19] : 0)
#define FFmul03(x) (x ? pow[log[x] + 0x01] : 0)
#define FFmul09(x) (x ? pow[log[x] + 0xc7] : 0)
#define FFmul0b(x) (x ? pow[log[x] + 0x68] : 0)
#define FFmul0d(x) (x ? pow[log[x] + 0xee] : 0)
#define FFmul0e(x) (x ? pow[log[x] + 0xdf] : 0)
#define fwd_affine(x) \
    (w = (uint)x, w ^= (w<<1)^(w<<2)^(w<<3)^(w<<4), (byte)(0x63^(w^(w>>8))))

#define inv_affine(x) \
    (w = (uint)x, w = (w<<1)^(w<<3)^(w<<6), (byte)(0x05^(w^(w>>8))))

static tVOID AES_GenerateTables(tAES_CTX* Ctx)
{
  unsigned char pow[512],log[256];
  int i = 0, w = 1;

  do
  {   
    pow[i] = (byte)w;
    pow[i + 255] = (byte)w;
    log[w] = (byte)i++;
    w ^=  (w << 1) ^ (w & ff_hi ? ff_poly : 0);
  } while (w != 1);
 
  for (i = 0,w = 1; i < sizeof(Ctx->rcon)/sizeof(Ctx->rcon[0]); i++)
  {
    Ctx->rcon[i] = w;
    w = (w << 1) ^ (w & ff_hi ? ff_poly : 0);
  }
  for(i = 0; i < 256; ++i)
  {   
    unsigned char b = Ctx->S[i]=fwd_affine(FFinv((byte)i));
    Ctx->T1[i][1]=Ctx->T1[i][2]=Ctx->T2[i][2]=Ctx->T2[i][3]=Ctx->T3[i][0]=Ctx->T3[i][3]=Ctx->T4[i][0]=Ctx->T4[i][1]=b;
    Ctx->T1[i][0]=Ctx->T2[i][1]=Ctx->T3[i][2]=Ctx->T4[i][3]=FFmul02(b);
    Ctx->T1[i][3]=Ctx->T2[i][0]=Ctx->T3[i][1]=Ctx->T4[i][2]=FFmul03(b);
    Ctx->S5[i] = b = FFinv(inv_affine((byte)i));
    Ctx->U1[b][3]=Ctx->U2[b][0]=Ctx->U3[b][1]=Ctx->U4[b][2]=Ctx->T5[i][3]=Ctx->T6[i][0]=Ctx->T7[i][1]=Ctx->T8[i][2]=FFmul0b(b);
    Ctx->U1[b][1]=Ctx->U2[b][2]=Ctx->U3[b][3]=Ctx->U4[b][0]=Ctx->T5[i][1]=Ctx->T6[i][2]=Ctx->T7[i][3]=Ctx->T8[i][0]=FFmul09(b);
    Ctx->U1[b][2]=Ctx->U2[b][3]=Ctx->U3[b][0]=Ctx->U4[b][1]=Ctx->T5[i][2]=Ctx->T6[i][3]=Ctx->T7[i][0]=Ctx->T8[i][1]=FFmul0d(b);
    Ctx->U1[b][0]=Ctx->U2[b][1]=Ctx->U3[b][2]=Ctx->U4[b][3]=Ctx->T5[i][0]=Ctx->T6[i][1]=Ctx->T7[i][2]=Ctx->T8[i][3]=FFmul0e(b);
  }
}
