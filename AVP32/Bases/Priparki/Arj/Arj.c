//#define _DEBUG


#pragma pack(1)
#include <memory.h>
#include "../dlink3.h"
#include "../pack3.h"
#include "../pelib.h"
#include "../retflags.h"
// #include "debug.h"

static BYTE corrupt;
static long ZipLen;

#include "unarj.h"
#include "arj.h"

#define NULL 0

#define crctable ((ulong*)Page_E)

//static ulong FAR* crctable=NULL;
static ulong crc=0;

#define CRCPOLY         0xEDB88320L
#define UPDATE_CRC(r,c) r=crctable[((uchar)(r)^(uchar)(c))&0xff]^(r>>CHAR_BIT)

static long Process_ARJ(void);

static void make_crctable(void)
{
        uint i, j;
        UCRC r;
        if(crctable==NULL)return;

        for (i = 0; i <= UCHAR_MAX; i++)
        {
                r = i;
                for (j = CHAR_BIT; j > 0; j--)
                {
                        if (r & 1)
                                r = (r >> 1) ^ CRCPOLY;
                        else
                                r >>= 1;
                }
                crctable[i] = r;
        }
}


static void crc_buf(char *str, int len)
{
#ifdef _DEBUG
Printf("crc_buf :%d",len);
#endif
    if(crctable)
        while (len--)
		{
/*
			BYTE i=crc^(*(str++));
			crc=crctable[i]^(crc>>CHAR_BIT);
*/
			UPDATE_CRC(crc,*(str++));
		}
}

static void fread_crc(uchar* p, int n)
{
#ifdef _DEBUG
Printf("fread_crc :%d",n);
#endif

        n = P_Read(p, (WORD)n);
        origsize += n;
        crc_buf(p, n);
}





#include "io.c"
#include "decode.c"


static int NEAR b_Copy(long size){
        uint r;
        while(size && !corrupt){
                r=P_Read(inbuf, (WORD)((size>INBUFSIZ) ? INBUFSIZ : size ));
                if(r<=0)break;
                size-=r;
                if(Rotate(1)){
					corrupt=17;
					return 0;
                }
                r=Write(inbuf,r);
                if(r<=0)break;
        }
        return r;
}

#define buf (Page_B)

static int Read_ARJ_Hdr(void){
        long r;

        r=P_Read(Page_C,4);
#ifdef _DEBUG
Printf("read=%d, sig=%d size=%d",(WORD)r,headeri->sig,headeri->size);
#endif

        if(4!=r) return P_END;
        if(headeri->sig != HEADER_ID) return P_END;
        if(headeri->size == 0 ) return P_END;
        if(headeri->size > 2600 ) return P_ERR;

//      siz=headeri->size+6;

        crc = CRC_MASK;
        fread_crc(Page_C,headeri->size);
#ifdef _DEBUG
Printf("read 6...");
#endif
        P_Read(buf, 6);
        if(*(WORD*)(buf+4)) return P_ERR;
#ifdef _DEBUG
Printf("crc check...");
#endif
//		if(crctable)
//			if ((crc ^ CRC_MASK) != *(DWORD*)buf) return P_ERR;

#ifdef _DEBUG
Printf("read arj hdr ok.");
#endif
        return P_OK;
}

static long Start_ARJ(){
                int r;
				inbuf=Page_A;
                outbuf=Page_B;
                text=Page_C;
                c_len=text+DDICSIZ;
				pt_table=(ushort*)(c_len+NC);
                left=pt_table+PTABLESIZE;
                right=left+2*NC;


//        c_table=pt_table+PTABLESIZE;
//        c_len=(uchar*)(c_table+CTABLESIZE);//[NC];


                r=Read_ARJ_Hdr();
                if(r!=P_OK) return r;
                if(headerf->type != 2) return P_END;
                if(headerf->version_needed_to_extract>3) return P_END;
//              if(headerf->flags & VOLUME_FLAG)        return P_VOL;

#ifdef _DEBUG
Printf("Start ARJ exit.");
#endif

        return P_OK;
}



static long Process_ARJ(void){
        int i;
		int need_next_vol;
        uchar *pc=NULL;
        long  ret=P_END;
        char v_flag=0;
		long tmp;

		corrupt=0;
//        crctable=(DWORD FAR*)New(1024);
        make_crctable();

next:
		need_next_vol=0;
        ret=Read_ARJ_Hdr();
		tmp=P_Seek(0,SEEK_CUR);
        if(ret!=P_OK){
#ifdef _DEBUG
Printf("Can't read hdr.");
#endif
				goto ret;
		}

        pc=Page_C+headerl->first_size;
        for(i=0;  ;i++,pc++){
                Zip_Name[i]=*pc;
                if(*pc==0)break;
                if(i>=500) goto err;
        }
        Zip_Name[i]=0;


		last_len=headerl->compressed_size;
		if((tmp+last_len) > ZipLen){
#ifdef _DEBUG
Printf("ZipLen: tmp(%d)+last_len(%d)> ZipLen(%d)",tmp,last_len,ZipLen);
#endif
			 goto err;
		}

#ifdef _DEBUG
Printf("Check Ext: %s",Zip_Name);
#endif

        if(!CheckExt()){
skip:
#ifdef _DEBUG
Printf("skip.");
#endif
                P_Seek(last_len, SEEK_CUR);
                goto next;
        }

        ret=P_ERR;
        if(headerl->version_needed_to_extract>3)goto skip;
        if(headerl->method>MAXMETHOD
                || (headerl->method==4 && headerl->version==1))goto skip;
        if(headerl->type !=BINARY_TYPE && headerl->type !=TEXT_TYPE)goto skip;

        if((headerl->flags & EXTFILE_FLAG) && !v_flag)
		{
#ifdef _DEBUG
Printf("EXTFILE_FLAG , v_flag=%d",v_flag);
#endif
			 ret=P_END;goto ret;
		}
        if(headerl->flags & VOLUME_FLAG) need_next_vol=1;
        ret=P_OK;
        if(headerl->flags & GARBLE_FLAG){
			RFlags|=RF_PROTECTED;
			goto try_next;
		}


        compsize=last_len;
        origsize=headerl->original_size;
//      if(origsize>Get_Temp_Space()){ret=P_DISK;goto ret;}


#ifdef _DEBUG
Printf("%d %s:  %s",(WORD)last_len,meth[headerl->method],Zip_Name);
#endif

        incnt=0;
        switch(headerl->method){
                case STORED     :               b_Copy(last_len);
                                                        last_len=0;     break;
                case DEFLATED1 :
                case DEFLATED2 :
                case DEFLATED3 : decode_main(); break;
                case DEFLATED4 : decode_f();break;
                default :        ret=P_ERR; break;
        }
try_next:
        if(need_next_vol && ret==P_OK  &&
// --- added by Dmitry Glavatskikh, 25.03.2003  ---
// --- for avoid endless loop on current volume ---
           Ext[0] == '.'  &&
           Ext[1] != '\0' &&
           Ext[2] != '\0' &&
           Ext[3] != '\0' &&
           Ext[4] == '\0' &&
// --- end add ---
          (Ext[1]  | 0x20) == 'a'){
                if((Ext[3] | 0x20) == 'j'
                || (Ext[2] | 0x20) == 'r')
                {       Ext[2]='0';Ext[3]='0';}

                Ext[3]++;
                if(Ext[3]==':'){
						Ext[3]='0';
                        Ext[2]++;
                        if(Ext[2]==':'){
							Ext[2]='0';
							Ext[1]++;
						}
                }
#ifdef _DEBUG
Printf("Need next volume: %s",Name);
#endif

                if(NextVolume())
                {
#ifdef _DEBUG
Printf("Next volume opened.");
#endif
                        if(P_OK==Start_ARJ())
                        {
#ifdef _DEBUG
Printf("Next volume started.");
#endif
						        ZipLen=File_Length;
                                v_flag=1; goto next;
                        }
                }
//              ret=P_END;
//				goto ret;
        }
        else     v_flag=0;


        if(last_len){
                P_Seek(last_len, SEEK_CUR);
                last_len=0;
        }
        Rotate(0);

		if(corrupt==17){
			ret=P_END;
			goto ret;
		}
		if(corrupt)goto err;

        goto ret;

err:
        RFlags|=RF_CORRUPT;
        ret=P_ERR;
ret:

//        Delete((BYTE FAR*)crctable);
//        crctable=NULL;
        return ret;
}


WORD FAR decode(){
//#ifdef _WIN32
// 		crctable=(ulong*)Page_E;
        ZipLen=File_Length;
        if(P_N==0)	return Start_ARJ();
        return  Process_ARJ();
//#else
//        return P_END;
//#endif
}

#pragma pack()
