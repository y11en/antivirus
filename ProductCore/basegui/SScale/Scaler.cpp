#include "../StdAfx.h"
#include "ClassMatrix.h"

#ifndef INT32
	#define INT32 int
#endif

#ifndef max
	#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static int MaxCommonDivider(int a,int b){
	int m,n,r;
	if(a>b){
		n=a;
		r=b;
		}
	else{
		n=b;
		r=a;
		}
	do{
		m=n;
		n=r;
		r=m%n;
		}while(r);
	return n;
	}

static bool scEqualRect(const RECT *lprc1, const RECT *lprc2)
{
	return lprc1->left == lprc2->left && lprc1->right == lprc2->right &&
			lprc1->top == lprc2->top && lprc1->bottom == lprc2->bottom;
}

static bool scIntersectRect(LPRECT lprcDst, const RECT *lprcSrc1, const RECT *lprcSrc2)
{
	lprcDst->left = max(lprcSrc1->left, lprcSrc2->left);
	lprcDst->right = min(lprcSrc1->right, lprcSrc2->right);
	if(lprcDst->left >= lprcDst->right)
	{
		lprcDst->left = lprcDst->top = lprcDst->right = lprcDst->bottom = 0;
		return false;
	}
	lprcDst->top = max(lprcSrc1->top, lprcSrc2->top);
	lprcDst->bottom = min(lprcSrc1->bottom, lprcSrc2->bottom);
	if(lprcDst->top >= lprcDst->bottom)
	{
		lprcDst->left = lprcDst->top = lprcDst->right = lprcDst->bottom = 0;
		return false;
	}
	return true;
}

bool SuperRescaleBitmapWithAlpha(CColorMatrix32 &SrcMatrix,LPRECT pSrcRect,
					   	CColorMatrix32 &DstMatrix,LPRECT pDstRect,
					   	int RecommendedBufferSize){
	if(!SrcMatrix.IsReady()){
		return false;
		}
	RECT SrcRect=SrcMatrix.GetRect(),DstRect;
	if(pSrcRect!=NULL){
		RECT IntsectRect;
		scIntersectRect(&IntsectRect, pSrcRect, &SrcRect);
		if(!scEqualRect(&IntsectRect, pSrcRect)){
			return false;
			}
		SrcRect=*pSrcRect;
		}
	int SrcSizeX=SrcRect.right-SrcRect.left+1,SrcSizeY=SrcRect.bottom-SrcRect.top+1,FirstMult;
	if(SrcSizeX<2||SrcSizeY<2){
		return false;
		}

	if(pDstRect!=NULL){
		if(DstMatrix.IsReady()){
			DstRect=DstMatrix.GetRect();
			RECT IntsectRect;
			scIntersectRect(&IntsectRect, pDstRect, &DstRect);
			if(!scEqualRect(&IntsectRect, pDstRect)){
				return false;
				}
			}
		DstRect=*pDstRect;
		}
	else{
		if(DstMatrix.IsReady()){
			DstRect=DstMatrix.GetRect();
			}
		else{
			return false;
			}

		}
	int DstSizeX=DstRect.right-DstRect.left+1,DstSizeY=DstRect.bottom-DstRect.top+1;
	if(DstSizeX<2||DstSizeY<2){
		return false;
		}

	if(DstSizeX==SrcSizeX&&DstSizeY==SrcSizeY){
		if(!DstMatrix.IsReady()){
			DstMatrix.Create(&DstRect,true);
			}

		POINT pt = *(POINT *)&DstRect;

		DstMatrix.CopyPartMatr(SrcMatrix,&SrcRect,&pt);
		return true;
		}
	CIntMatrix AuxMatrix;
	int CurNumOfLineInPart=0,TotalBaseSrcLineNumber=SrcRect.top;
	int LineLen=4*DstSizeX,NumOfLines;
	if(RecommendedBufferSize<=0){
		NumOfLines=SrcSizeY;
		}
	else{
		NumOfLines=RecommendedBufferSize/LineLen;
		if(NumOfLines<3){
			NumOfLines=3;
			}
		else if(NumOfLines>SrcSizeY){
			NumOfLines=SrcSizeY;
			}
		}
	if(DstSizeY>=SrcSizeY){ //учащаем отсчёты вдоль столбцов
		AuxMatrix.Create(NumOfLines,4*DstSizeX);
///	Старт хитрого блока
		int TempSrc,TempAux;
		if(SrcSizeX<=DstSizeX){
			TempSrc=SrcSizeX-1;
			TempAux=DstSizeX-1;
			}
		else{
			TempSrc=SrcSizeX;
			TempAux=DstSizeX;
			}

		int		nod=MaxCommonDivider(TempSrc,TempAux),
				nok=TempSrc*TempAux/nod,
				StepSrc=TempAux/nod,StepAux=TempSrc/nod;
		if(SrcSizeX<=DstSizeX){
			FirstMult=StepSrc;
			}
		else{
			FirstMult=StepAux;
			}
		int TempAuxVer=SrcSizeY-1,TempDstVer=DstSizeY-1;
		nod=MaxCommonDivider(TempDstVer,TempAuxVer);
		nok=TempDstVer*TempAuxVer/nod;
		int StepAuxVer=TempDstVer/nod,StepDstVer=TempAuxVer/nod,
				SecondMult=StepAuxVer,
				TotalMult=FirstMult*SecondMult,
				HalfTotalMult=(TotalMult+1)>>1;
		int InterPassPosInAux,NumberOfProcessedSrcLines,NumOfLinesInBlock=NumOfLines-1,
				OffsetToNextAuxLine=DstSizeX*4,NumberOfProcessedDstLines=0;
		bool FirstPass=true,CanContinue=true;

		int OffsetToNextDstLine;
		if(DstMatrix.IsReady()){
			OffsetToNextDstLine=DstMatrix.GetX();
			}
		else{
			OffsetToNextDstLine=DstSizeX;
			DstMatrix.Create(DstSizeY,DstSizeX);
			}
		for(NumberOfProcessedSrcLines=0;CanContinue;FirstPass=false){
			if(!FirstPass){
				RECT rc = {0,NumOfLines-1,4*DstSizeX-1,NumOfLines-1};
				POINT pt = {0, 0};
				AuxMatrix.CopyPartMatr(AuxMatrix,&rc,&pt);
				}
			if(SrcSizeX<=DstSizeX){	//учащаем отсчёты вдоль строк
				int CurY;
				CMatrixRow<RGBA> *CurSrcLine;
				CMatrixRow<int>	*CurAuxLine;
				for(CurY=NumOfLinesInBlock+FirstPass,CurSrcLine=SrcMatrix+SrcRect.top+NumberOfProcessedSrcLines,CurAuxLine=AuxMatrix+(!FirstPass);
						CurY;CurY--,CurSrcLine++,CurAuxLine++){
					int CurX,PosInSrc,*CurSrc=(int *)(*CurSrcLine+SrcRect.left),*CurAux=*CurAuxLine,
					
							BlueCurAuxValue=*CurSrc++,BlueNextSrcValue=*CurSrc++,BlueCurDiffValue,
							GreenCurAuxValue,GreenNextSrcValue,GreenCurDiffValue,
							RedCurAuxValue,RedNextSrcValue,RedCurDiffValue,
							AlphaCurAuxValue,AlphaNextSrcValue,AlphaCurDiffValue;
					GreenCurAuxValue=BlueCurAuxValue>>8;
					BlueCurAuxValue&=0xFF;
					RedCurAuxValue=GreenCurAuxValue>>8;
					GreenCurAuxValue&=0xFF;
					AlphaCurAuxValue=(RedCurAuxValue>>8)&0xFF;
					RedCurAuxValue&=0xFF;

					GreenNextSrcValue=BlueNextSrcValue>>8;
					BlueNextSrcValue&=0xFF;
					RedNextSrcValue=GreenNextSrcValue>>8;
					GreenNextSrcValue&=0xFF;
					AlphaNextSrcValue=(RedNextSrcValue>>8)&0xFF;
					RedNextSrcValue&=0xFF;

					BlueCurDiffValue=(BlueNextSrcValue-BlueCurAuxValue)*StepAux;
					BlueCurAuxValue*=StepSrc;
					BlueCurAuxValue-=BlueCurDiffValue;

					GreenCurDiffValue=(GreenNextSrcValue-GreenCurAuxValue)*StepAux;
					GreenCurAuxValue*=StepSrc;
					GreenCurAuxValue-=GreenCurDiffValue;

					RedCurDiffValue=(RedNextSrcValue-RedCurAuxValue)*StepAux;
					RedCurAuxValue*=StepSrc;
					RedCurAuxValue-=RedCurDiffValue;

					AlphaCurDiffValue=(AlphaNextSrcValue-AlphaCurAuxValue)*StepAux;
					AlphaCurAuxValue*=StepSrc;
					AlphaCurAuxValue-=AlphaCurDiffValue;

					for(CurX=DstSizeX,PosInSrc=StepSrc;CurX;CurX--,CurAux++,PosInSrc-=StepAux){
				
						if(PosInSrc>=0){
							BlueCurAuxValue+=BlueCurDiffValue;
							*CurAux++=BlueCurAuxValue;

							GreenCurAuxValue+=GreenCurDiffValue;
							*CurAux++=GreenCurAuxValue;

							RedCurAuxValue+=RedCurDiffValue;
							*CurAux++=RedCurAuxValue;

							AlphaCurAuxValue+=AlphaCurDiffValue;
							*CurAux=AlphaCurAuxValue;
							}
						else{
							PosInSrc+=StepSrc;
							if(PosInSrc>=StepAux){ //это не единственная точка
								int CurValue=*CurSrc++;
								BlueCurAuxValue=BlueNextSrcValue;
								BlueNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								BlueCurDiffValue=(BlueNextSrcValue-BlueCurAuxValue)*StepAux;
								BlueCurAuxValue=PosInSrc*BlueCurAuxValue+(StepSrc-PosInSrc)*BlueNextSrcValue;
								*CurAux++=BlueCurAuxValue;

								GreenCurAuxValue=GreenNextSrcValue;
								GreenNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								GreenCurDiffValue=(GreenNextSrcValue-GreenCurAuxValue)*StepAux;
								GreenCurAuxValue=PosInSrc*GreenCurAuxValue+(StepSrc-PosInSrc)*GreenNextSrcValue;
								*CurAux++=GreenCurAuxValue;

								RedCurAuxValue=RedNextSrcValue;
								RedNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								RedCurDiffValue=(RedNextSrcValue-RedCurAuxValue)*StepAux;
								RedCurAuxValue=PosInSrc*RedCurAuxValue+(StepSrc-PosInSrc)*RedNextSrcValue;
								*CurAux++=RedCurAuxValue;

								AlphaCurAuxValue=AlphaNextSrcValue;
								AlphaNextSrcValue=CurValue&0xFF;
								AlphaCurDiffValue=(AlphaNextSrcValue-AlphaCurAuxValue)*StepAux;
								AlphaCurAuxValue=PosInSrc*AlphaCurAuxValue+(StepSrc-PosInSrc)*AlphaNextSrcValue;
								*CurAux=AlphaCurAuxValue;
								}
							else{ //это единственная точка

								int CurValue=*CurSrc++;

								BlueCurAuxValue=BlueNextSrcValue;
								BlueNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*BlueCurAuxValue+(StepSrc-PosInSrc)*BlueNextSrcValue;

								GreenCurAuxValue=GreenNextSrcValue;
								GreenNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*GreenCurAuxValue+(StepSrc-PosInSrc)*GreenNextSrcValue;

								RedCurAuxValue=RedNextSrcValue;
								RedNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*RedCurAuxValue+(StepSrc-PosInSrc)*RedNextSrcValue;

								AlphaCurAuxValue=AlphaNextSrcValue;
								AlphaNextSrcValue=CurValue&0xFF;
								*CurAux=PosInSrc*AlphaCurAuxValue+(StepSrc-PosInSrc)*AlphaNextSrcValue;
								}
							}
						}
					}
				}
			else if(SrcSizeX>DstSizeX){	//прореживаем отсчёты вдоль строк
				int CurY;
				CMatrixRow<RGBA> *CurSrcLine;
				CMatrixRow<int> *CurAuxLine;
				for(CurY=NumOfLinesInBlock+FirstPass,CurSrcLine=SrcMatrix+SrcRect.top+NumberOfProcessedSrcLines,CurAuxLine=AuxMatrix+(!FirstPass);
						CurY;CurY--,CurSrcLine++,CurAuxLine++){
					int CurX,PosInAux,*CurSrc=(int *)(*CurSrcLine+SrcRect.left),*CurAux=*CurAuxLine,
						BlueAccum=0,GreenAccum=0,RedAccum=0,AlphaAccum=0;
					for(CurX=SrcSizeX,PosInAux=StepAux;CurX;CurX--,CurSrc++,PosInAux-=StepSrc){ //,PosInSrc+=StepAux){
						if(PosInAux>StepSrc){
							int CurValue=*CurSrc;
							BlueAccum+=(CurValue&0xFF)*StepSrc;
							GreenAccum+=((CurValue>>8)&0xFF)*StepSrc;
							RedAccum+=((CurValue>>16)&0xFF)*StepSrc;
							AlphaAccum+=((CurValue>>24)&0xFF)*StepSrc;
							}
						else{
							int CurValue=*CurSrc;

							*CurAux++=(CurValue&0xFF)*PosInAux+BlueAccum;
							BlueAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+GreenAccum;
							GreenAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+RedAccum;
							RedAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+AlphaAccum;
							AlphaAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							PosInAux+=StepAux;//-StepSrc;
							}
						}
					}
				}
			int CurX;
			int *CurAuxBasePoint;
			RGBA *CurDstBasePoint;

			int LocalNumberOfProcessedDstLines;
			for(CurX=DstSizeX,CurAuxBasePoint=AuxMatrix[0],CurDstBasePoint=DstMatrix[DstRect.top+NumberOfProcessedDstLines]+DstRect.left;
					CurX;CurX--,CurAuxBasePoint+=4,CurDstBasePoint++){
				int CurY,PosInAux,*CurAux=CurAuxBasePoint,
						BlueCurDstValue,BlueNextAuxValue,BlueCurDiffValue,
						GreenCurDstValue,GreenNextAuxValue,GreenCurDiffValue,
						RedCurDstValue,RedNextAuxValue,RedCurDiffValue,
						AlphaCurDstValue,AlphaNextAuxValue,AlphaCurDiffValue;
				RGBA *CurDst=CurDstBasePoint;
				BlueCurDstValue=*CurAux;
				GreenCurDstValue=CurAux[1];
				RedCurDstValue=CurAux[2];
				AlphaCurDstValue=CurAux[3];
				CurAux+=OffsetToNextAuxLine;

				BlueNextAuxValue=*CurAux;
				GreenNextAuxValue=CurAux[1];
				RedNextAuxValue=CurAux[2];
				AlphaNextAuxValue=CurAux[3];
				CurAux+=OffsetToNextAuxLine;

				BlueCurDiffValue=(BlueNextAuxValue-BlueCurDstValue)*StepDstVer;
				GreenCurDiffValue=(GreenNextAuxValue-GreenCurDstValue)*StepDstVer;
				RedCurDiffValue=(RedNextAuxValue-RedCurDstValue)*StepDstVer;
				AlphaCurDiffValue=(AlphaNextAuxValue-AlphaCurDstValue)*StepDstVer;

				if(FirstPass){
					PosInAux=StepAuxVer;

					BlueCurDstValue*=StepAuxVer;
					GreenCurDstValue*=StepAuxVer;
					RedCurDstValue*=StepAuxVer;
					AlphaCurDstValue*=StepAuxVer;
					}
				else{
					PosInAux=InterPassPosInAux;
					BlueCurDstValue=PosInAux*BlueCurDstValue+(StepAuxVer-PosInAux)*BlueNextAuxValue;
					GreenCurDstValue=PosInAux*GreenCurDstValue+(StepAuxVer-PosInAux)*GreenNextAuxValue;
					RedCurDstValue=PosInAux*RedCurDstValue+(StepAuxVer-PosInAux)*RedNextAuxValue;
					AlphaCurDstValue=PosInAux*AlphaCurDstValue+(StepAuxVer-PosInAux)*AlphaNextAuxValue;

					}
				BlueCurDstValue-=BlueCurDiffValue;
				GreenCurDstValue-=GreenCurDiffValue;
				RedCurDstValue-=RedCurDiffValue;
				AlphaCurDstValue-=AlphaCurDiffValue;
				for(LocalNumberOfProcessedDstLines=0,CurY=NumOfLinesInBlock;CurY;
						CurDst+=OffsetToNextDstLine){
			
					if(PosInAux>=0){
						AlphaCurDstValue+=AlphaCurDiffValue;
						CurDst->A=(unsigned char)((AlphaCurDstValue+HalfTotalMult)/TotalMult);
						BlueCurDstValue+=BlueCurDiffValue;
						CurDst->B=(unsigned char)((BlueCurDstValue+HalfTotalMult)/TotalMult);
						GreenCurDstValue+=GreenCurDiffValue;
						CurDst->G=(unsigned char)((GreenCurDstValue+HalfTotalMult)/TotalMult);
						RedCurDstValue+=RedCurDiffValue;
						CurDst->R=(unsigned char)((RedCurDstValue+HalfTotalMult)/TotalMult);

						}
					else{
						PosInAux+=StepAuxVer;
						if(--CurY){
							if(PosInAux>=StepDstVer){ //это не единственная точка
								BlueCurDstValue=BlueNextAuxValue;
								BlueNextAuxValue=*CurAux;
								BlueCurDiffValue=(BlueNextAuxValue-BlueCurDstValue)*StepDstVer;
								BlueCurDstValue=PosInAux*BlueCurDstValue+(StepAuxVer-PosInAux)*BlueNextAuxValue;
								CurDst->B=(unsigned char)((BlueCurDstValue+HalfTotalMult)/TotalMult);

								GreenCurDstValue=GreenNextAuxValue;
								GreenNextAuxValue=CurAux[1];
								GreenCurDiffValue=(GreenNextAuxValue-GreenCurDstValue)*StepDstVer;
								GreenCurDstValue=PosInAux*GreenCurDstValue+(StepAuxVer-PosInAux)*GreenNextAuxValue;
								CurDst->G=(unsigned char)((GreenCurDstValue+HalfTotalMult)/TotalMult);

								RedCurDstValue=RedNextAuxValue;
								RedNextAuxValue=CurAux[2];
								RedCurDiffValue=(RedNextAuxValue-RedCurDstValue)*StepDstVer;
								RedCurDstValue=PosInAux*RedCurDstValue+(StepAuxVer-PosInAux)*RedNextAuxValue;
								CurDst->R=(unsigned char)((RedCurDstValue+HalfTotalMult)/TotalMult);

								AlphaCurDstValue=AlphaNextAuxValue;
								AlphaNextAuxValue=CurAux[3];
								AlphaCurDiffValue=(AlphaNextAuxValue-AlphaCurDstValue)*StepDstVer;
								AlphaCurDstValue=PosInAux*AlphaCurDstValue+(StepAuxVer-PosInAux)*AlphaNextAuxValue;
								CurDst->A=(unsigned char)((AlphaCurDstValue+HalfTotalMult)/TotalMult);

								CurAux+=OffsetToNextAuxLine;
								}
							else{ //это единственная точка
								BlueCurDstValue=BlueNextAuxValue;
								BlueNextAuxValue=*CurAux;
								CurDst->B=(unsigned char)((PosInAux*BlueCurDstValue+(StepAuxVer-PosInAux)*BlueNextAuxValue+HalfTotalMult)/TotalMult);

								GreenCurDstValue=GreenNextAuxValue;
								GreenNextAuxValue=CurAux[1];
								CurDst->G=(unsigned char)((PosInAux*GreenCurDstValue+(StepAuxVer-PosInAux)*GreenNextAuxValue+HalfTotalMult)/TotalMult);

								RedCurDstValue=RedNextAuxValue;
								RedNextAuxValue=CurAux[2];
								CurDst->R=(unsigned char)((PosInAux*RedCurDstValue+(StepAuxVer-PosInAux)*RedNextAuxValue+HalfTotalMult)/TotalMult);

								AlphaCurDstValue=AlphaNextAuxValue;
								AlphaNextAuxValue=CurAux[3];
								CurDst->A=(unsigned char)((PosInAux*AlphaCurDstValue+(StepAuxVer-PosInAux)*AlphaNextAuxValue+HalfTotalMult)/TotalMult);

								CurAux+=OffsetToNextAuxLine;
								}
								//CurY--;
							}
						else{

							}
						}
					if(CurY!=0){
						LocalNumberOfProcessedDstLines++;
						PosInAux-=StepDstVer;
						}
					}								
				if(CurX==1){
					InterPassPosInAux=PosInAux;
					}
				}
			NumberOfProcessedDstLines+=LocalNumberOfProcessedDstLines;
			NumberOfProcessedSrcLines+=NumOfLinesInBlock+FirstPass;
			if(SrcSizeY==NumberOfProcessedSrcLines){
				CanContinue=false;
				}
			else{
				if(NumberOfProcessedSrcLines+NumOfLinesInBlock>SrcSizeY){
					NumOfLinesInBlock=SrcSizeY-NumberOfProcessedSrcLines;
					}
				}
			}
///	Финиш хитрого блока
		} // кончили учащать отсчёты вдоль столбцов

	else{ //прореживаем отсчёты вдоль столбцов
		AuxMatrix.Create(NumOfLines,4*DstSizeX);//SrcSizeY
		int TempSrc,TempAux;
		if(SrcSizeX<=DstSizeX){
			TempSrc=SrcSizeX-1;
			TempAux=DstSizeX-1;
			}
		else{
			TempSrc=SrcSizeX;
			TempAux=DstSizeX;
			}
		int	nod=MaxCommonDivider(TempSrc,TempAux),
				nok=TempSrc*TempAux/nod,
				StepSrc=TempAux/nod,StepAux=TempSrc/nod;
		if(SrcSizeX<=DstSizeX){
			FirstMult=StepSrc;
			}
		else{
			FirstMult=StepAux;
			}
  		int OffsetToNextDstLine;
		if(DstMatrix.IsReady()){
			OffsetToNextDstLine=DstMatrix.GetX();
			}
		else{
			OffsetToNextDstLine=DstSizeX;
			DstMatrix.Create(DstSizeY,DstSizeX);
			}
		nod=MaxCommonDivider(SrcSizeY,DstSizeY);
		nok=SrcSizeY*DstSizeY/nod;
		int StepAuxVer=DstSizeY/nod,
				StepDst=SrcSizeY/nod,
				SecondMult=StepDst,
				TotalMult=FirstMult*SecondMult,
				HalfTotalMult=(TotalMult+1)>>1;
		int OffsetToNextAuxLine=DstSizeX*4;
		bool FirstPass=true,CanContinue=true;
		int InterPassPosInDst,NumberOfProcessedSrcLines,
			NumOfLinesInBlock=NumOfLines-1,NumberOfProcessedDstLines=0;

		for(NumberOfProcessedSrcLines=0;CanContinue;FirstPass=false){

			if(SrcSizeX<=DstSizeX){	//учащаем отсчёты вдоль строк
				int CurY;
				CMatrixRow<RGBA> *CurSrcLine;
				CMatrixRow<int> *CurAuxLine;
				for(CurY=NumOfLinesInBlock+FirstPass,CurSrcLine=SrcMatrix+SrcRect.top+NumberOfProcessedSrcLines,CurAuxLine=AuxMatrix+(!FirstPass);
						CurY;CurY--,CurSrcLine++,CurAuxLine++){
					int CurX,PosInSrc,*CurSrc=(int *)(*CurSrcLine+SrcRect.left),*CurAux=*CurAuxLine,
						

							BlueCurAuxValue=*CurSrc++,BlueNextSrcValue=*CurSrc++,BlueCurDiffValue,
							GreenCurAuxValue,GreenNextSrcValue,GreenCurDiffValue,
							RedCurAuxValue,RedNextSrcValue,RedCurDiffValue,
							AlphaCurAuxValue,AlphaNextSrcValue,AlphaCurDiffValue;
					GreenCurAuxValue=BlueCurAuxValue>>8;
					BlueCurAuxValue&=0xFF;
					RedCurAuxValue=GreenCurAuxValue>>8;
					GreenCurAuxValue&=0xFF;
					AlphaCurAuxValue=(RedCurAuxValue>>8)&0xFF;
					RedCurAuxValue&=0xFF;

					GreenNextSrcValue=BlueNextSrcValue>>8;
					BlueNextSrcValue&=0xFF;
					RedNextSrcValue=GreenNextSrcValue>>8;
					GreenNextSrcValue&=0xFF;
					AlphaNextSrcValue=(RedNextSrcValue>>8)&0xFF;
					RedNextSrcValue&=0xFF;

					BlueCurDiffValue=(BlueNextSrcValue-BlueCurAuxValue)*StepAux;
					BlueCurAuxValue*=StepSrc;
					BlueCurAuxValue-=BlueCurDiffValue;

					GreenCurDiffValue=(GreenNextSrcValue-GreenCurAuxValue)*StepAux;
					GreenCurAuxValue*=StepSrc;
					GreenCurAuxValue-=GreenCurDiffValue;

					RedCurDiffValue=(RedNextSrcValue-RedCurAuxValue)*StepAux;
					RedCurAuxValue*=StepSrc;
					RedCurAuxValue-=RedCurDiffValue;

					AlphaCurDiffValue=(AlphaNextSrcValue-AlphaCurAuxValue)*StepAux;
					AlphaCurAuxValue*=StepSrc;
					AlphaCurAuxValue-=AlphaCurDiffValue;

					for(CurX=DstSizeX,PosInSrc=StepSrc;CurX;CurX--,CurAux++,PosInSrc-=StepAux){
					
						if(PosInSrc>=0){
							BlueCurAuxValue+=BlueCurDiffValue;
							*CurAux++=BlueCurAuxValue;

							GreenCurAuxValue+=GreenCurDiffValue;
							*CurAux++=GreenCurAuxValue;

							RedCurAuxValue+=RedCurDiffValue;
							*CurAux++=RedCurAuxValue;

							AlphaCurAuxValue+=AlphaCurDiffValue;
							*CurAux=AlphaCurAuxValue;
							}
						else{
							PosInSrc+=StepSrc;
							if(PosInSrc>=StepAux){ //это не единственная точка
								int CurValue=*CurSrc++;
								BlueCurAuxValue=BlueNextSrcValue;
								BlueNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								BlueCurDiffValue=(BlueNextSrcValue-BlueCurAuxValue)*StepAux;
								BlueCurAuxValue=PosInSrc*BlueCurAuxValue+(StepSrc-PosInSrc)*BlueNextSrcValue;
								*CurAux++=BlueCurAuxValue;

								GreenCurAuxValue=GreenNextSrcValue;
								GreenNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								GreenCurDiffValue=(GreenNextSrcValue-GreenCurAuxValue)*StepAux;
								GreenCurAuxValue=PosInSrc*GreenCurAuxValue+(StepSrc-PosInSrc)*GreenNextSrcValue;
								*CurAux++=GreenCurAuxValue;

								RedCurAuxValue=RedNextSrcValue;
								RedNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								RedCurDiffValue=(RedNextSrcValue-RedCurAuxValue)*StepAux;
								RedCurAuxValue=PosInSrc*RedCurAuxValue+(StepSrc-PosInSrc)*RedNextSrcValue;
								*CurAux++=RedCurAuxValue;

								AlphaCurAuxValue=AlphaNextSrcValue;
								AlphaNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								AlphaCurDiffValue=(AlphaNextSrcValue-AlphaCurAuxValue)*StepAux;
								AlphaCurAuxValue=PosInSrc*AlphaCurAuxValue+(StepSrc-PosInSrc)*AlphaNextSrcValue;
								*CurAux=AlphaCurAuxValue;
								}
							else{ //это единственная точка

								int CurValue=*CurSrc++;

								BlueCurAuxValue=BlueNextSrcValue;
								BlueNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*BlueCurAuxValue+(StepSrc-PosInSrc)*BlueNextSrcValue;

								GreenCurAuxValue=GreenNextSrcValue;
								GreenNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*GreenCurAuxValue+(StepSrc-PosInSrc)*GreenNextSrcValue;

								RedCurAuxValue=RedNextSrcValue;
								RedNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux++=PosInSrc*RedCurAuxValue+(StepSrc-PosInSrc)*RedNextSrcValue;

								AlphaCurAuxValue=AlphaNextSrcValue;
								AlphaNextSrcValue=CurValue&0xFF;
								CurValue>>=8;
								*CurAux=PosInSrc*AlphaCurAuxValue+(StepSrc-PosInSrc)*AlphaNextSrcValue;
								}
							}
						}
					}
				}
			else if(SrcSizeX>DstSizeX){	//прореживаем отсчёты вдоль строк
				int CurY;
				CMatrixRow<RGBA> *CurSrcLine;
				CMatrixRow<int> *CurAuxLine;
				for(CurY=NumOfLinesInBlock+FirstPass,CurSrcLine=SrcMatrix+SrcRect.top+NumberOfProcessedSrcLines,CurAuxLine=AuxMatrix+(!FirstPass);
						CurY;CurY--,CurSrcLine++,CurAuxLine++){
					int CurX,PosInAux,*CurSrc=(int *)(*CurSrcLine+SrcRect.left),*CurAux=*CurAuxLine,
						BlueAccum=0,GreenAccum=0,RedAccum=0,AlphaAccum=0;
					for(CurX=SrcSizeX,PosInAux=StepAux;CurX;CurX--,CurSrc++,PosInAux-=StepSrc){ //,PosInSrc+=StepAux){
						if(PosInAux>StepSrc){
							int CurValue=*CurSrc;
							BlueAccum+=(CurValue&0xFF)*StepSrc;
							GreenAccum+=((CurValue>>8)&0xFF)*StepSrc;
							RedAccum+=((CurValue>>16)&0xFF)*StepSrc;
							AlphaAccum+=((CurValue>>24)&0xFF)*StepSrc;
							}
						else{
							int CurValue=*CurSrc;

							*CurAux++=(CurValue&0xFF)*PosInAux+BlueAccum;
							BlueAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+GreenAccum;
							GreenAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+RedAccum;
							RedAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							CurValue>>=8;
							*CurAux++=(CurValue&0xFF)*PosInAux+AlphaAccum;
							AlphaAccum=(CurValue&0xFF)*(StepSrc-PosInAux);
							PosInAux+=StepAux;
							}
						}
					}
				}

			int CurX;
			int OffsetToNextAuxLine=DstSizeX*4;
			int *CurAuxBasePoint;
			RGBA *CurDstBasePoint;

			int LocalNumberOfProcessedDstLines;
			for(CurX=DstSizeX,CurAuxBasePoint=AuxMatrix[0],CurDstBasePoint=DstMatrix[DstRect.top+NumberOfProcessedDstLines]+DstRect.left;//+++
					CurX;CurX--,CurAuxBasePoint+=4,CurDstBasePoint++){ /////////////////////////
				RGBA *CurDst=CurDstBasePoint;
				int CurY,PosInDst,*CurAux=CurAuxBasePoint,*InitialCurAux=CurAuxBasePoint,
					BlueAccum,GreenAccum,RedAccum,AlphaAccum;
				if(FirstPass){
					BlueAccum=0;
					GreenAccum=0;
					RedAccum=0;
					AlphaAccum=0;
					PosInDst=StepDst;
					}
				else{
					BlueAccum=*CurAux;
					GreenAccum=CurAux[1];
					RedAccum=CurAux[2];
					AlphaAccum=CurAux[3];
					CurAux+=OffsetToNextAuxLine;
					PosInDst=InterPassPosInDst;
					}
				for(LocalNumberOfProcessedDstLines=0,CurY=NumOfLinesInBlock+FirstPass;CurY;CurY--,CurAux+=OffsetToNextAuxLine,PosInDst-=StepAuxVer){
					if(PosInDst>StepAuxVer){
						BlueAccum+=*CurAux*StepAuxVer;
						GreenAccum+=CurAux[1]*StepAuxVer;
						RedAccum+=CurAux[2]*StepAuxVer;
						AlphaAccum+=CurAux[3]*StepAuxVer;
						}
					else{
						RGBA Temp;
						Temp.B=(unsigned char)((*CurAux*PosInDst+BlueAccum+HalfTotalMult)/TotalMult);
						Temp.G=(unsigned char)((CurAux[1]*PosInDst+GreenAccum+HalfTotalMult)/TotalMult);
						Temp.R=(unsigned char)((CurAux[2]*PosInDst+RedAccum+HalfTotalMult)/TotalMult);
						Temp.A=(unsigned char)((CurAux[3]*PosInDst+AlphaAccum+HalfTotalMult)/TotalMult);
						*CurDst=Temp;
						BlueAccum=*CurAux*(StepAuxVer-PosInDst);
						GreenAccum=CurAux[1]*(StepAuxVer-PosInDst);
						RedAccum=CurAux[2]*(StepAuxVer-PosInDst);
						AlphaAccum=CurAux[3]*(StepAuxVer-PosInDst);
						CurDst+=OffsetToNextDstLine;
						PosInDst+=StepDst;
						LocalNumberOfProcessedDstLines++;
						}
					}								
				if(CurX==1){
					InterPassPosInDst=PosInDst;
					}
				*InitialCurAux=BlueAccum;
				InitialCurAux[1]=GreenAccum;
				InitialCurAux[2]=RedAccum;
				InitialCurAux[3]=AlphaAccum;
				}
			NumberOfProcessedDstLines+=LocalNumberOfProcessedDstLines;
			NumberOfProcessedSrcLines+=NumOfLinesInBlock+FirstPass;
			if(SrcSizeY==NumberOfProcessedSrcLines){
				CanContinue=false;
				}
			else{
				if(NumberOfProcessedSrcLines+NumOfLinesInBlock>SrcSizeY){
					NumOfLinesInBlock=SrcSizeY-NumberOfProcessedSrcLines;
					}
				}
			}
		} //кончили прореживать отсчёты вдоль столбцов
			
	AuxMatrix.Destroy();
	return true;
	}
