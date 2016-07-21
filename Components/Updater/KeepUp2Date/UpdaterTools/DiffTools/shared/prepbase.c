#include <memory.h>
#include "prepbase.h"
#include "../avepack/base.h"
#include "../avepack/records.h"
#include "types.h"

#define DUMP(x)

/* ------------------------------------------------------------------------- */

static tINT Offset2Len(tUINT* pdwOffset, tUINT* pdwLastOffset, tBOOL bRestore)
{
  if (!bRestore)
  {
    tUINT dwOffset = *pdwOffset;
    *pdwOffset -= *pdwLastOffset;
    *pdwLastOffset = dwOffset;
  }
  else
  {
    *pdwLastOffset = *pdwOffset = *pdwOffset + *pdwLastOffset;
  }
	return 0;
}

/* ------------------------------------------------------------------------- */

tINT PrepBase4Diff(tBYTE* pBase, tBOOL bRestore)
{
	tINT rCount = 0;
	AVP_BlockHeader*	bhp;
	AVP_BaseHeader		BaseHeader;

  memcpy(&BaseHeader, pBase, sizeof(BaseHeader));

  if( BaseHeader.Magic != AVP_MAGIC )
  {
abort_: ;
  }
  else
	{
    tUINT  i, j;
    tBYTE* nbuf = NULL;
    AVP_BlockHeader* BlockHeaderArray = (AVP_BlockHeader*)(pBase +
      BaseHeader.BlockHeaderTableFO);

    for( i = 0; i < BaseHeader.BlockHeaderTableSize; i++ )
    {
      tBYTE* buf;
      tUINT  npos = 0;
      bhp = (BlockHeaderArray + i);
      if(bhp->Compression) goto abort_;
      buf = pBase + bhp->BlockFO;

			switch(bhp->BlockType)
      {
			case BT_NAME:
//				if(names_done){
//					mfNames.Write(buf,bhp->CompressedSize);
//				}
//				else{
//					if(nbuf)delete nbuf;
//					nbuf=new BYTE[bhp->CompressedSize+1];
//					memcpy(nbuf+1,buf,bhp->CompressedSize);
//					nbuf[0]='\n';
//					DUMP(("* Names:"));
//					unsigned char* n1= nbuf;
//					for(k=1;k<=bhp->CompressedSize;k++)
//					{
//						if(nbuf[k]=='\n')
//						{
//							nbuf[k]=0;
//							DUMP(("%s", n1));
//							n1 = nbuf+k+1;
//						}
//					}
//					delete buf;
//					buf=0;
//					names_done=1;
//					goto names_done;
//				}
				break;
			case BT_RECORD:	
				rCount+=bhp->NumRecords;
				
				switch(bhp->RecordType){
				case RT_MEMORY:	{
					R_Memory* p=(R_Memory*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				case RT_SECTOR:	{
					R_Sector* p=(R_Sector*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				case RT_FILE:	{
					R_File* p=(R_File*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				case RT_CA:		{
					R_CA* p=(R_CA*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				case RT_UNPACK:	{
					R_Unpack* p=(R_Unpack*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				case RT_EXTRACT:{
					R_Extract* p=(R_Extract*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						Offset2Len(&p->NameIdx, &npos, bRestore);
						DUMP(("%s\t%04X\t%s", GetRecordTypeName(bhp->RecordType), p->LinkIdx, NamePtr));
					}	break;
								}
				default:	break;
				}
				break;

			case BT_LINK16:
				break;
				
			case BT_LINK32:
				break;
			default:
				break;
			}
		}
	}
	return rCount;
}

/* ------------------------------------------------------------------------- */
