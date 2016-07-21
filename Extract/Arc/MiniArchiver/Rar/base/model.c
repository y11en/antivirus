#include <Prague/prague.h>

#include "const.h"
#include "proto.h"



RSTATIC RINLINE void RATTRIB SEE2_init(tSEE2_CONTEXT *see2, int InitVal)
{
    see2->Shift = PERIOD_BITS-4;
    see2->Summ = (ushort)(InitVal << see2->Shift);
    see2->Count = 4;
}

RSTATIC RINLINE uint RATTRIB SEE2_getMean(tSEE2_CONTEXT *see2)
{
    uint RetVal;

    RetVal = (see2->Summ & 0xFFFF) >> see2->Shift;
    see2->Summ = (ushort)(see2->Summ-RetVal);

    return RetVal + (RetVal == 0);
}

RSTATIC RINLINE void RATTRIB SEE2_update(tSEE2_CONTEXT *see2)
{
    if (see2->Shift < PERIOD_BITS && --see2->Count == 0)
    {
        see2->Summ <<= 1;
        see2->Count = (byte)(3 << see2->Shift++);
    }
}

RSTATIC void RATTRIB RC_InitDecoder(tRAR *data, tINPUT *input)
{
    int i;
    data->PPM.Coder.low = 0;
    data->PPM.Coder.range = (uint)(-1);
    data->PPM.Coder.code = 0;
    //data->PPM.Coder.code = RarReadDword(data);
    for ( i=0;i<4;i++ )
    {
        data->PPM.Coder.code = (data->PPM.Coder.code << 8) | RarReadByte(input);
    }
}

RSTATIC RINLINE int RATTRIB RC_GetCurrentCount(tRAR *data)
{
  if ( !data->PPM.Coder.SubRange.Scale )
  {
    /* bugfix 22.06.2005 */
    data->Error = cTRUE;
    return(0);
  }
  if ( !(data->PPM.Coder.range /= data->PPM.Coder.SubRange.Scale) )
  {
    /* bugfix 22.06.2005 */
    data->Error = cTRUE;
    return(0);
  }
  return (data->PPM.Coder.code-data->PPM.Coder.low)/(data->PPM.Coder.range);
}

RSTATIC RINLINE int RATTRIB RC_GetCurrentShiftCount(tRAR *data, uint shift)
{
  data->PPM.Coder.range >>= shift;
  if ( !data->PPM.Coder.range )
  {
    /* bugfix 22.06.2005 */
    data->Error = cTRUE;
    return(0);
  }
  return (data->PPM.Coder.code-data->PPM.Coder.low)/(data->PPM.Coder.range);
}

RSTATIC RINLINE void RATTRIB RC_Decode(tRAR *data)
{
    data->PPM.Coder.low += data->PPM.Coder.range*data->PPM.Coder.SubRange.LowCount;
    data->PPM.Coder.range *= data->PPM.Coder.SubRange.HighCount - data->PPM.Coder.SubRange.LowCount;
}

RSTATIC tPPM_CONTEXT * RATTRIB PPM_createChild(tRAR *data, tPPM_CONTEXT *_this, tSTATE *pStats, tSTATE *FirstState)
{
    tPPM_CONTEXT *pc;

    pc = SA_AllocContext(data);

    if ( pc != NULL )
    {
        pc->NumStats = 1;
        pc->U.OneState = *FirstState;
        pc->Suffix = _this;
        pStats->Successor = pc;
    }
    return pc;
}

RSTATIC void RATTRIB PPM_RestartModelRare(tRAR *data)
{
    static const ushort InitBinEsc[]=
    {
        0x3CDD,0x1F3F,0x59BF,0x48F3,0x64A1,0x5ABC,0x6632,0x6051
    };
    int i;
    int k;
    int m;

    //Printf("RestartModelRare" crlf);
    SA_Dump(data,0);

    memset(data->PPM.CharMask,0,sizeof(data->PPM.CharMask));

    SA_InitSubAllocator(data);

    data->PPM.InitRL = -(data->PPM.MaxOrder < 12 ? data->PPM.MaxOrder:12)-1;
    data->PPM.MinContext = data->PPM.MaxContext = (tPPM_CONTEXT*) SA_AllocContext(data);
    data->PPM.MinContext->Suffix = NULL;
    data->PPM.OrderFall = data->PPM.MaxOrder;

    data->PPM.MinContext->U.U.SummFreq = (ushort)((data->PPM.MinContext->NumStats=256)+1);

    data->PPM.FoundState=data->PPM.MinContext->U.U.Stats=(tSTATE*)SA_AllocUnits(data,256/2);

    data->PPM.RunLength = data->PPM.InitRL;
    data->PPM.PrevSuccess = 0;

    for ( i=0 ;i < 256;i++)
    {
        data->PPM.MinContext->U.U.Stats[i].Symbol= (byte)(i);
        data->PPM.MinContext->U.U.Stats[i].Freq=1;
        data->PPM.MinContext->U.U.Stats[i].Successor=NULL;
    }


    for (i=0;i < 128;i++)
        for (k=0;k < 8;k++)
            for (m=0;m < 64;m += 8)
                data->PPM.BinSumm[i][k+m] = (ushort)(BIN_SCALE-InitBinEsc[k]/(i+2));

    for (i=0;i < 25;i++)
        for (k=0;k < 16;k++)
            SEE2_init(&data->PPM.SEE2Cont[i][k],(5*i+10));

}

RSTATIC void RATTRIB PPM_StartModelRare(tRAR *data, int MaxOrder)
{
    int i;
    int k;
    int m;
    int Step;

    data->PPM.EscCount = 1;

    data->PPM.MaxOrder=MaxOrder;
    PPM_RestartModelRare(data);

    data->PPM.NS2BSIndx[0]=2*0;
    data->PPM.NS2BSIndx[1]=2*1;
    memset(data->PPM.NS2BSIndx+2,2*2,9);
    memset(data->PPM.NS2BSIndx+11,2*3,256-11);

    for (i=0;i < 3;i++) data->PPM.NS2Indx[i]=(byte)(i);

    for (m=i, k=Step=1;i < 256;i++)
    {
        data->PPM.NS2Indx[i] = (byte)(m);
        if ( !--k )
        {
            k = ++Step;
            m++;
        }
    }

    memset(data->PPM.HB2Flag,0,0x40);
    memset(data->PPM.HB2Flag+0x40,0x08,0x100-0x40);
    data->PPM.DummySEE2Cont.Shift=PERIOD_BITS;

}

RSTATIC void RATTRIB PPMC_rescale(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model)
{
    int OldNS;
    int i;
    int Adder;
    int EscFreq;
    int n0;
    int n1;
    tSTATE *p1;
    tSTATE *p;

    OldNS = _this->NumStats;
    i = OldNS-1;

    for (p=Model->FoundState;p != _this->U.U.Stats;p--)
    {
        tSTATE tmp;

        tmp = p[0];
        p[0] = p[-1];
        p[-1] = tmp;
    }


    _this->U.U.Stats->Freq += 4;
    _this->U.U.SummFreq += 4;
    EscFreq = _this->U.U.SummFreq-p->Freq;

    Adder = (Model->OrderFall != 0);

    p->Freq = (byte)((p->Freq+Adder) >> 1);
    _this->U.U.SummFreq = p->Freq;

    do
    {
        EscFreq -= (++p)->Freq;

        p->Freq= (byte)((p->Freq+Adder) >> 1);
        _this->U.U.SummFreq = (ushort)(_this->U.U.SummFreq+p->Freq);

        if (p[0].Freq > p[-1].Freq)
        {
            tSTATE tmp=*(p1=p);

            do
            {
                p1[0]=p1[-1];
            } while (--p1 != _this->U.U.Stats && tmp.Freq > p1[-1].Freq);

            *p1=tmp;
        }
    } while ( --i );


    if (p->Freq == 0)
    {
        do
        {
            i++;
        } while ((--p)->Freq == 0);

        EscFreq += i;

        if ((_this->NumStats -= i) == 1)
        {
            tSTATE tmp=*_this->U.U.Stats;
            do
            {
                tmp.Freq-=(tmp.Freq >> 1);
                EscFreq>>=1;
            } while (EscFreq > 1);

            SA_FreeUnits(data,_this->U.U.Stats,(OldNS+1) >> 1);
            *(Model->FoundState = &_this->U.OneState) = tmp;
            return;
        }
    }


    _this->U.U.SummFreq += (EscFreq -= (EscFreq >> 1));
    n0 = (OldNS+1) >> 1;
    n1 = (_this->NumStats+1) >> 1;
    if (n0 != n1) _this->U.U.Stats = (tSTATE*) SA_ShrinkUnits(data,_this->U.U.Stats,n0,n1);
    Model->FoundState=_this->U.U.Stats;

}

RSTATIC  tPPM_CONTEXT * RATTRIB PPM_CreateSuccessors(tRAR *data, rarbool Skip, tSTATE *p1)
{
    tSTATE UpState;
    tPPM_CONTEXT *pc;
    tPPM_CONTEXT *UpBranch;

    tSTATE *p;
    tSTATE *ps[MAX_O];
    tSTATE **pps;
    byte*  min_ptr;
    byte*  max_ptr;

    min_ptr = data->PPM.SubAlloc.HeapStart;
    max_ptr = data->PPM.SubAlloc.HeapEnd;

    pc = data->PPM.MinContext;
    UpBranch = data->PPM.FoundState->Successor;
    pps = ps;

    if ( !Skip )
    {
        *pps++ = data->PPM.FoundState;
        if ( !pc->Suffix ) goto NO_LOOP;
    }

    if ( p1 )
    {
        p=p1;
        pc=pc->Suffix;
        goto LOOP_ENTRY;
    }


    do
    {
        pc = pc->Suffix;

        /* --- bugfix 16.05.2005 --- */
        if ( (byte*)pc < min_ptr || (byte*)pc > max_ptr )
        {
          data->Error = cTRUE;
          return NULL; /* error */
        }

        if ( pc->NumStats != 1 )
        {
            if ((p=pc->U.U.Stats)->Symbol != data->PPM.FoundState->Symbol)
            {
                do
                {
                    p++;
                    // --- bugfix 29.07.2004 ---
                    if ( (byte*)p < min_ptr ||(byte*)p > max_ptr )
                    {
                      data->Error = cTRUE;
                      return NULL; // error
                    }
                    // --- end fix ---
                } while (p->Symbol != data->PPM.FoundState->Symbol);
            }
        }
        else
        {
          p = &(pc->U.OneState);

          /* --- bugfix 16.05.2005 --- */
          if ( (byte*)p < min_ptr ||(byte*)p > max_ptr )
          {
            data->Error = cTRUE;
            return NULL; /* error */
          }
        }

LOOP_ENTRY:

        if ( p->Successor != UpBranch )
        {
            pc = p->Successor;
            break;
        }

        /* --- bugfix 16.05.2005 --- */
        if ( (byte*)pc < min_ptr || (byte*)pc > max_ptr )
        {
          data->Error = cTRUE;
          return NULL; // error
        }

        *pps++ = p;
    }
    while ( pc->Suffix );


NO_LOOP:
    if (pps == ps) return pc;

    /* --- bugfix 17.06.2005 --- */
    if ( (byte*)UpBranch < min_ptr || (byte*)UpBranch > max_ptr )
    {
      data->Error = cTRUE;
      return NULL; // error
    }

    UpState.Symbol = *(byte*) UpBranch;
    UpState.Successor = (tPPM_CONTEXT*) (((byte*) UpBranch)+1);

    /* --- bugfix 17.06.2005 --- */
    if ( (byte*)pc < min_ptr || (byte*)pc > max_ptr )
    {
      data->Error = cTRUE;
      return NULL; // error
    }

    if (pc->NumStats != 1)
    {
        uint cf;
        uint s0;

        if ((byte*) pc <= data->PPM.SubAlloc.pText) return(NULL);

        if ((p=pc->U.U.Stats)->Symbol != UpState.Symbol)
        {
            do
            {
                p++;
                // --- bugfix 16.08.2004 ---
                if ( (byte*)p < min_ptr ||(byte*)p > max_ptr )
                {
                  data->Error = cTRUE;
                  return NULL; // error
                }
                // --- end fix ---
            } while (p->Symbol != UpState.Symbol);
        }

        cf = p->Freq-1;
        s0 = pc->U.U.SummFreq-pc->NumStats-cf;

        // --- bugfix 16.08.2004 ---
//      UpState.Freq=1+((2*cf <= s0)?(5*cf > s0):((2*cf+3*s0-1)/(2*s0)));
        if ( 2*cf <= s0 )
        {
          UpState.Freq = 1 + (5*cf > s0);
        }
        else
        {
          if ( !(2*s0) )
          {
            data->Error = cTRUE;
            return(NULL); // error
          }
          UpState.Freq = 1 + ((2*cf+3*s0-1)/(2*s0));
        }
        // --- end fix ---
    }
    else
    {
        UpState.Freq=pc->U.OneState.Freq;
    }

    do
    {
        pc = PPM_createChild(data,pc,*--pps,&UpState);
        if ( !pc )
          return NULL;
    } while (pps != ps);

  return pc;
}

RSTATIC void RATTRIB PPM_UpdateModel(tRAR* data)
{
    tSTATE fs;
    tSTATE *p;

    tPPM_CONTEXT *pc;
    tPPM_CONTEXT *Successor;
    uint ns1;
    uint ns;
    uint cf;
    uint sf;
    uint s0;
    byte*  min_ptr;
    byte*  max_ptr;

    min_ptr = data->PPM.SubAlloc.HeapStart;
    max_ptr = data->PPM.SubAlloc.HeapEnd;

    fs = *data->PPM.FoundState;
    p = NULL;

    if ( fs.Freq < MAX_FREQ/4 && (pc = data->PPM.MinContext->Suffix) != NULL )
    {
        if (pc->NumStats != 1)
        {
            if ((p=pc->U.U.Stats)->Symbol != fs.Symbol)
            {
                do
                {
                  p++;
                  // --- bugfix 06.05.2005 ---
                  if ( (byte*)p < min_ptr || (byte*)p > (max_ptr - sizeof(tSTATE)) )
                  {
                    data->Error = cTRUE;
                    return; // overrun
                  }
                  // --- end fix ---
                } while (p->Symbol != fs.Symbol);

                if (p[0].Freq >= p[-1].Freq)
                {
                    tSTATE tmp;

                    tmp = p[0];
                    p[0] = p[-1];
                    p[-1] = tmp;
                    p--;
                }
            }
            if (p->Freq < MAX_FREQ-9)
            {
                p->Freq += 2;
                pc->U.U.SummFreq += 2;
            }
        }
        else
        {
            p = &(pc->U.OneState);
            p->Freq += (p->Freq < 32);
        }
    }


    if ( !data->PPM.OrderFall )
    {
        data->PPM.MinContext=data->PPM.MaxContext=data->PPM.FoundState->Successor=PPM_CreateSuccessors(data,TRUE,p);
        if ( !data->PPM.MinContext ) goto RESTART_MODEL;
        return;
    }

    if ( data->PPM.SubAlloc.pText < data->PPM.SubAlloc.HeapStart ||
         data->PPM.SubAlloc.pText > data->PPM.SubAlloc.HeapEnd )
    {
      /* bugfix, 26.07.2005 */
      data->Error = cTRUE;
      return;
    }
    
    *data->PPM.SubAlloc.pText++ = fs.Symbol;
    Successor = (tPPM_CONTEXT*) data->PPM.SubAlloc.pText;

    if (data->PPM.SubAlloc.pText >= data->PPM.SubAlloc.FakeUnitsStart) goto RESTART_MODEL;

    if ( fs.Successor )
    {
        if ((byte*) fs.Successor <= data->PPM.SubAlloc.pText && (fs.Successor = PPM_CreateSuccessors(data,FALSE,p)) == NULL) goto RESTART_MODEL;

        if ( !--data->PPM.OrderFall )
        {
            Successor=fs.Successor;
            data->PPM.SubAlloc.pText -= (data->PPM.MaxContext != data->PPM.MinContext);
        }
    }
    else
    {
        data->PPM.FoundState->Successor = Successor;
        fs.Successor = data->PPM.MinContext;
    }


    ns = data->PPM.MinContext->NumStats;
    s0 = data->PPM.MinContext->U.U.SummFreq-ns-(fs.Freq-1);

    for ( pc = data->PPM.MaxContext; pc != data->PPM.MinContext;pc=pc->Suffix)
    {
        if ((ns1=pc->NumStats) != 1)
        {
            if ((ns1 & 1) == 0)
            {
                pc->U.U.Stats=(tSTATE*) SA_ExpandUnits(data,pc->U.U.Stats,ns1 >> 1);
                if ( !pc->U.U.Stats ) goto RESTART_MODEL;
            }
            pc->U.U.SummFreq += (2*ns1 < ns)+2*((4*ns1 <= ns) & (pc->U.U.SummFreq <= 8*ns1));
        }
        else
        {
            p=(tSTATE*) SA_AllocUnits(data,1);

            if ( !p ) goto RESTART_MODEL;

            *p = pc->U.OneState;

            pc->U.U.Stats=p;

            if (p->Freq < MAX_FREQ/4-1)
            {
                p->Freq += p->Freq;
            }
            else
            {
                p->Freq  = MAX_FREQ-4;
            }
            pc->U.U.SummFreq=p->Freq+data->PPM.InitEsc+(ns > 3);
        }

        cf=2*fs.Freq*(pc->U.U.SummFreq+6);

        sf=s0+pc->U.U.SummFreq;

        if (cf < 6*sf)
        {
            cf=1+(cf > sf)+(cf >= 4*sf);
            pc->U.U.SummFreq += 3;
        }
        else
        {
            cf=4+(cf >= 9*sf)+(cf >= 12*sf)+(cf >= 15*sf);
            pc->U.U.SummFreq += cf;
        }
        p=pc->U.U.Stats+ns1;
        p->Successor=Successor;
        p->Symbol = fs.Symbol;
        p->Freq = cf;
        pc->NumStats=++ns1;
    }

    data->PPM.MaxContext=data->PPM.MinContext=fs.Successor;
    return;

RESTART_MODEL:
    if ( data->Error )
      return;
    PPM_RestartModelRare(data);
    data->PPM.EscCount=0;
}


// Tabulated escapes for exponential symbol distribution
static const byte ExpEscape[16]={ 25,14, 9, 7, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2 };
#define GET_MEAN(SUMM,SHIFT,ROUND) ((SUMM+(1 << (SHIFT-ROUND))) >> (SHIFT))



RSTATIC rarbool RATTRIB PPMC_decodeBinSymbol(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model)
{
    ushort *bs;
    tSTATE *rs;
    int sc;

    rs = &_this->U.OneState;


    Model->HiBitsFlag=Model->HB2Flag[Model->FoundState->Symbol];

    sc = RC_GetCurrentShiftCount(data, TOT_BITS);
    if ( !sc && data->Error )
      return(cFALSE);

    bs = &Model->BinSumm[rs->Freq-1][Model->PrevSuccess+
           Model->NS2BSIndx[_this->Suffix->NumStats-1]+
           Model->HiBitsFlag+2*Model->HB2Flag[rs->Symbol]+
           ((Model->RunLength >> 26) & 0x20)];

    if ( sc < *bs)
    {
        Model->FoundState=rs;
        rs->Freq += (rs->Freq < 128);
        Model->Coder.SubRange.LowCount=0;
        Model->Coder.SubRange.HighCount = *bs;
        *bs = (*bs+INTERVAL-GET_MEAN(*bs,PERIOD_BITS,2)) & 0xFFFF ;
        Model->PrevSuccess=1;
        Model->RunLength++;
    }
    else
    {
        Model->Coder.SubRange.LowCount = *bs;
        *bs = (*bs-GET_MEAN(*bs,PERIOD_BITS,2)) & 0xFFFF;
        Model->Coder.SubRange.HighCount=BIN_SCALE;
        Model->InitEsc=ExpEscape[*bs >> 10];
        Model->NumMasked=1;
        Model->CharMask[rs->Symbol]=Model->EscCount;
        Model->PrevSuccess=0;
        Model->FoundState=NULL;
    }

    return(cTRUE);
}


RSTATIC  void RATTRIB PPMC_update1(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model,tSTATE *p)
{
    (Model->FoundState = p)->Freq += 4;

    _this->U.U.SummFreq += 4;

    if (p[0].Freq > p[-1].Freq)
    {
        tSTATE tmp;

        tmp = p[0];
        p[0] = p[-1];
        p[-1] = tmp;

        Model->FoundState = --p;
        if (p->Freq > MAX_FREQ) PPMC_rescale(data,_this,Model);
    }
}

RSTATIC  rarbool RATTRIB PPMC_decodeSymbol1(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model)
{
  tSTATE* p;
  tINT    i;
  tINT    HiCnt;
  tINT    count;
  tBYTE*  min_ptr;
  tBYTE*  max_ptr;

  min_ptr = data->PPM.SubAlloc.HeapStart;
  max_ptr = data->PPM.SubAlloc.HeapEnd;

  Model->Coder.SubRange.Scale = _this->U.U.SummFreq;

  p = _this->U.U.Stats;

  if ( !(count = RC_GetCurrentCount(data)) && data->Error )
    return(cFALSE);

  if ( count >= (int)Model->Coder.SubRange.Scale)
    return(cFALSE);

  if ( count < (HiCnt=p->Freq) )
  {
    Model->PrevSuccess=(2*(Model->Coder.SubRange.HighCount=HiCnt) > Model->Coder.SubRange.Scale);
    Model->RunLength += Model->PrevSuccess;
    (Model->FoundState=p)->Freq=(HiCnt += 4);
     _this->U.U.SummFreq += 4;
    if (HiCnt > MAX_FREQ)
      PPMC_rescale(data, _this, Model);
    Model->Coder.SubRange.LowCount=0;
    return(TRUE);
  }
  else
  {
    if (Model->FoundState==NULL)
      return(FALSE);
  }


  Model->PrevSuccess=0;
  i = _this->NumStats-1;

  while ( cTRUE )
  {
    ++p;

    /* --- bugfix 04.03.2005 --- */
    if ( (byte*)p < min_ptr ||(byte*)p > (max_ptr - sizeof(tSTATE)) )
    {
      data->Error = cTRUE;
      return(cFALSE); /* error */
    }
    /* --- end fix --- */

    if ( (HiCnt += p->Freq) > count )
      break;

    if (--i == 0)
    {
      Model->HiBitsFlag=Model->HB2Flag[Model->FoundState->Symbol];
      Model->Coder.SubRange.LowCount=HiCnt;
      Model->CharMask[p->Symbol]=Model->EscCount;
      i=(Model->NumMasked = _this->NumStats)-1;
      Model->FoundState=NULL;

      do
      {
        --p;

        /* --- bugfix 04.03.2005 --- */
        if ( (byte*)p < min_ptr ||(byte*)p > (max_ptr - sizeof(tSTATE)) )
        {
          data->Error = cTRUE;
          return(cFALSE); /* error */
        }
        /* --- end fix --- */

        Model->CharMask[p->Symbol] = Model->EscCount;
      }
      while ( --i );

      Model->Coder.SubRange.HighCount=Model->Coder.SubRange.Scale;
      return(TRUE);
    }
  }

  Model->Coder.SubRange.LowCount=(Model->Coder.SubRange.HighCount=HiCnt)-p->Freq;
  PPMC_update1(data,_this,Model,p);
  return(TRUE);
}


RSTATIC  void RATTRIB PPMC_update2(tRAR *data, tPPM_CONTEXT *_this,tPPM *Model, tSTATE* p)
{
    (Model->FoundState=p)->Freq += 4;
    _this->U.U.SummFreq += 4;
    if (p->Freq > MAX_FREQ) PPMC_rescale(data,_this,Model);
    Model->EscCount++;
    Model->RunLength=Model->InitRL;
}


RSTATIC  tSEE2_CONTEXT * RATTRIB PPMC_makeEscFreq2(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model,int Diff)
{
    tSEE2_CONTEXT* psee2c;

    if (_this->NumStats != 256)
    {
        psee2c=Model->SEE2Cont[Model->NS2Indx[Diff-1]]+
            (Diff < _this->Suffix->NumStats-_this->NumStats)+
            2*(_this->U.U.SummFreq < 11*_this->NumStats)+4*(Model->NumMasked > Diff)+
            Model->HiBitsFlag;
        Model->Coder.SubRange.Scale = SEE2_getMean(psee2c);
    }
    else
    {
        psee2c = &Model->DummySEE2Cont;
        Model->Coder.SubRange.Scale=1;
    }
    return psee2c;
}




RSTATIC  rarbool RATTRIB PPMC_decodeSymbol2(tRAR *data, tPPM_CONTEXT *_this, tPPM *Model)
{
    int count;
    int HiCnt;
    int i;

    tSEE2_CONTEXT *psee2c;
    tSTATE *ps[256];
    tSTATE **pps;
    tSTATE *p;

    i = _this->NumStats-Model->NumMasked;

    psee2c = PPMC_makeEscFreq2(data,_this,Model,i);

    pps = ps;
    p = _this->U.U.Stats-1;

    HiCnt = 0;
    do
    {
        do
        {
            p++;
        } while (Model->CharMask[p->Symbol] == Model->EscCount);

        HiCnt += p->Freq;
        *pps++ = p;
    } while ( --i );


    Model->Coder.SubRange.Scale += HiCnt;
    if ( !(count = RC_GetCurrentCount(data)) && data->Error )
      return(FALSE);

    if (count >= (int)Model->Coder.SubRange.Scale)
      return(FALSE);

    p = *(pps=ps);

    if (count < HiCnt)
    {
        HiCnt=0;
        while ((HiCnt += p->Freq) <= count) p=*++pps;
        Model->Coder.SubRange.LowCount = (Model->Coder.SubRange.HighCount=HiCnt)-p->Freq;
        SEE2_update(psee2c);
        PPMC_update2(data,_this,Model,p);
    }
    else
    {
        Model->Coder.SubRange.LowCount=HiCnt;
        Model->Coder.SubRange.HighCount=Model->Coder.SubRange.Scale;
        i = _this->NumStats-Model->NumMasked;
        pps--;
        do
        {
            Model->CharMask[(*++pps)->Symbol]=Model->EscCount;
        } while ( --i );
        psee2c->Summ += Model->Coder.SubRange.Scale;
        Model->NumMasked = _this->NumStats;
    }
    return(TRUE);
}


RSTATIC RINLINE void RATTRIB PPM_ClearMask(tRAR *data)
{
    data->PPM.EscCount=1;
    memset(data->PPM.CharMask,0,sizeof(data->PPM.CharMask));
}

RSTATIC rarbool RATTRIB PPM_DecodeInit(tRAR *data, tINPUT *input, int *EscChar)
{
    int MaxOrder;
    int MaxMB = 0;
    rarbool Reset;

    MaxOrder = RarReadByte(input);
    Reset = (MaxOrder & 0x20) != 0;

    if (Reset)
    {
        MaxMB = RarReadByte(input);
    }
    else
    {
        if (data->PPM.SubAlloc.SubAllocatorSize == 0) return(FALSE);
    }

    if ( MaxOrder & 0x40 ) *EscChar = RarReadByte(input);

    RC_InitDecoder(data, input);

    if (Reset)
    {
        MaxOrder = (MaxOrder & 0x1f)+1;
        if (MaxOrder>16) MaxOrder=16+(MaxOrder-16)*3;

        if (MaxOrder == 1)
        {
            SA_StopSubAllocator(data);
            return(FALSE);
        }
        if (!SA_StartSubAllocator(data,MaxMB+1)) return FALSE;
        PPM_StartModelRare(data,MaxOrder);
    }
    return (data->PPM.MinContext != NULL);
}


RSTATIC void RATTRIB AriDecNormalize(tINPUT *input, tRangeCoder *coder)
{
    while ((coder->low^(coder->low+coder->range))<TOP || (coder->range<BOT && ((coder->range=(0-coder->low)&(BOT-1)),1)))
    {
        coder->code=(coder->code << 8) | RarReadByte(input);
        coder->range <<= 8;
        coder->low <<= 8;
    }
}

RSTATIC int RATTRIB PPM_DecodeChar(tRAR *data, tINPUT *input)
{
    int Symbol;

    if ( (byte*)data->PPM.MinContext <= data->PPM.SubAlloc.pText ||
         (byte*)data->PPM.MinContext > data->PPM.SubAlloc.HeapEnd )
    {
        return(-1);
    }


    if ( data->PPM.MinContext->NumStats != 1 )
    {
        //Printf("PPMC_decodeSymbol1" crlf);
        if ( !PPMC_decodeSymbol1(data,data->PPM.MinContext,&data->PPM) )
          return(-1);
    }
    else
    {
        //Printf("PPMC_decodeBinSymbol" crlf);
        if ( !PPMC_decodeBinSymbol(data,data->PPM.MinContext,&data->PPM) )
          return(-1);
    }

    RC_Decode(data);

    while ( !data->PPM.FoundState )
    {

        AriDecNormalize(input, &data->PPM.Coder);
        //ARI_DEC_NORMALIZE(data->PPM.Coder.code,data->PPM.Coder.low,data->PPM.Coder.range);

        do
        {
            data->PPM.OrderFall++;
            data->PPM.MinContext = data->PPM.MinContext->Suffix;
            if ((byte*)data->PPM.MinContext <= data->PPM.SubAlloc.pText || (byte*)data->PPM.MinContext > data->PPM.SubAlloc.HeapEnd) return(-1);

        } while (data->PPM.MinContext->NumStats == data->PPM.NumMasked);

        if ( !PPMC_decodeSymbol2(data,data->PPM.MinContext,&data->PPM)) return(-1);
        RC_Decode(data);
    }

    Symbol = data->PPM.FoundState->Symbol;

    if (!data->PPM.OrderFall && (byte*) data->PPM.FoundState->Successor > data->PPM.SubAlloc.pText)
    {
        data->PPM.MinContext=data->PPM.MaxContext=data->PPM.FoundState->Successor;
    }
    else
    {
        PPM_UpdateModel(data);
        if (data->PPM.EscCount == 0) PPM_ClearMask(data);
    }

    AriDecNormalize(input, &data->PPM.Coder);
    //ARI_DEC_NORMALIZE(data->PPM.Coder.code,data->PPM.Coder.low,data->PPM.Coder.range);
    return(Symbol);
}
