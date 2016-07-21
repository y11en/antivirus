#include <Prague/prague.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

static void CountTreeLength(tPACK20* data, int i)
{
  if (i < data->FreqTableSize)
    data->LenCount[(data->Depth < 15) ? data->Depth : 15]++;
  else
  {
    data->Depth++;
    CountTreeLength(data, data->Left[i]);
    CountTreeLength(data, data->Right[i]);
    data->Depth--;
  }
}

static void MakeLen(tPACK20* data, int root)
{
  int i, k;
  unsigned int cum;

  for (i = 0; i <= 15; i++)
    data->LenCount[i] = 0;
  CountTreeLength(data,root);
  cum = 0;
  for (i = 15; i > 0; i--)
    cum += data->LenCount[i] << (15 - i);
  while (cum != (1U << 15))
  {
    data->LenCount[15]--;
    for (i = 14; i > 0; i--)
    {
      if (data->LenCount[i] != 0)
      {
        data->LenCount[i]--;
        data->LenCount[i+1] += 2;
        break;
      }
    }
    cum--;
  }
  for (i = 15; i > 0; i--)
  {
    k = data->LenCount[i];
    while (--k >= 0)
      data->TreeLen[*data->SortPtr++] = i;
  }
}

static void DownHeap(tPACK20* data, int i)
{
  int j, k;

  k = data->TreeHeap[i];
  while ((j = 2 * i) <= data->TreeHeapSize)
  {
    if (j < data->TreeHeapSize && data->TreeFreq[data->TreeHeap[j]] > data->TreeFreq[data->TreeHeap[j+1]])
      j++;
    if (data->TreeFreq[k] <= data->TreeFreq[data->TreeHeap[j]])
      break;
    data->TreeHeap[i] = data->TreeHeap[j];
    i = j;
  }
  data->TreeHeap[i] = k;
}

static void MakeTreeCode(tPACK20* data,int n, unsigned char len[], unsigned int code[])
{
  int i;
  unsigned int start[17];

  start[1] = 0;
  for (i = 1; i <= 15; i++)
    start[i + 1] = (start[i] + data->LenCount[i]) << 1;
  for (i = 0; i < n; i++)
    code[i] = start[len[i]]++;
}

int make_tree(tPACK20* data, int nparm, unsigned int freqparm[],
        unsigned char lenparm[], unsigned int codeparm[])
{
  int i, j, k, avail;

  data->FreqTableSize = nparm;
  data->TreeFreq = freqparm;
  data->TreeLen = lenparm;
  avail = data->FreqTableSize;
  data->TreeHeapSize = 0;
  data->TreeHeap[1] = 0;
  for (i = 0; i < data->FreqTableSize; i++)
  {
    data->TreeLen[i] = 0;
    if (data->TreeFreq[i])
      data->TreeHeap[++data->TreeHeapSize] = i;
  }
  if (data->TreeHeapSize < 2)
  {
    codeparm[data->TreeHeap[1]] = 0;
    return data->TreeHeap[1];
  }
  for (i = data->TreeHeapSize/2; i>=1; i--)
    DownHeap(data,i);
  data->SortPtr = codeparm;
  do
  {
    i = data->TreeHeap[1];
    if (i < data->FreqTableSize)
      *data->SortPtr++ = i;
    data->TreeHeap[1] = data->TreeHeap[data->TreeHeapSize--];
    DownHeap(data,1);
    j = data->TreeHeap[1];
    if (j < data->FreqTableSize)
      *data->SortPtr++ = j;
    k = avail++;
    data->TreeFreq[k] = data->TreeFreq[i] + data->TreeFreq[j];
    data->TreeHeap[1] = k;
    DownHeap(data,1);
    data->Left[k] = i;
    data->Right[k] = j;
  } while (data->TreeHeapSize > 1);
  data->SortPtr = codeparm;
  MakeLen(data,k);
  MakeTreeCode(data,nparm, lenparm, codeparm);
  return k;
}
