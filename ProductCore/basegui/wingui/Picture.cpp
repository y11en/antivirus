//////////////////////////////////////////////////////////////////////
// PictureEx.cpp: implementation of the CPictureEx class.
//
// Picture displaying control with support for the following formats:
// GIF (including animated GIF87a and GIF89a), JPEG, BMP, WMF, ICO, CUR
// 
// Written by Oleg Bykov (oleg_bykoff@rsdn.ru)
// Copyright (c) 2001
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "Picture.h"

//////////////////////////////////////////////////////////////////////
// CPictureEx

CPictureEx::CPictureEx()
{
	m_PictureSize.cx = m_PictureSize.cy = 0;
	m_clrBackground    = RGB(255,255,255); // white by default
	m_nGlobalCTSize    = 0;
	m_nCurrOffset	   = 0;
	m_nDataSize		   = 0;

	m_pGIFLSDescriptor = NULL;
	m_pGIFHeader	   = NULL;
	m_pRawData		   = NULL;
	m_hGlobal          = NULL;
}

CPictureEx::~CPictureEx()
{
	Unload();
}

void CPictureEx::Unload()
{
	ClearGlobal();

	std::vector<TFrame>::iterator it;
	for (it=m_arrFrames.begin();it<m_arrFrames.end();it++)
		(*it).m_pPicture->Release();

	m_arrFrames.clear();
}

void CPictureEx::ClearGlobal()
{
	if( m_hGlobal )
	{
		GlobalUnlock(m_hGlobal);
		GlobalFree(m_hGlobal);
		m_hGlobal = NULL;
	}
}

BOOL CPictureEx::Load(LPCTSTR szFileName)
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	BOOL bRetValue = FALSE;
	m_nDataSize = GetFileSize(hFile, NULL);
	m_hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, m_nDataSize);
	if( m_hGlobal )
	{
		m_pRawData = (unsigned char*)GlobalLock(m_hGlobal);

		DWORD nReadBytes;
		if( m_pRawData && ReadFile(hFile, m_pRawData, m_nDataSize, &nReadBytes, NULL) )
			bRetValue = Load();
	}
	CloseHandle(hFile);
	return bRetValue;
}

BOOL CPictureEx::Load(HINSTANCE hInstance, LPCTSTR szResourceName, LPCTSTR szResourceType)
{
	HRSRC hPicture = FindResource(hInstance,szResourceName,szResourceType);
	if( !hPicture )
		return FALSE;

	HGLOBAL hResData = LoadResource(hInstance,hPicture);
	if( !hResData )
		return FALSE;

	m_nDataSize = SizeofResource(hInstance, hPicture);
	m_hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_NODISCARD,m_nDataSize);

	// hResData is not the real HGLOBAL (we can't lock it)
	BOOL bRetValue = FALSE;
	if( m_hGlobal )
	{
		m_pRawData = (unsigned char*)GlobalLock(m_hGlobal);
		if( m_pRawData )
		{
			CopyMemory(m_pRawData,(char*)LockResource(hResData),m_nDataSize);
			bRetValue = Load();
		}
	};

	FreeResource(hResData);
	return bRetValue;
}

#define MAP_LOGHIM_TO_PIX(x,ppli) ( ((ppli)*(x) + 2540/2) / 2540 )

inline void HiMetricToPixel(const SIZEL * lpSizeInHiMetric, LPSIZEL lpSizeInPix)
{
	int nPixelsPerInchX;    // Pixels per logical inch along width
	int nPixelsPerInchY;    // Pixels per logical inch along height

	HDC hDCScreen = GetDC(NULL);
	nPixelsPerInchX = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	nPixelsPerInchY = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	ReleaseDC(NULL, hDCScreen);

	lpSizeInPix->cx = MAP_LOGHIM_TO_PIX(lpSizeInHiMetric->cx, nPixelsPerInchX);
	lpSizeInPix->cy = MAP_LOGHIM_TO_PIX(lpSizeInHiMetric->cy, nPixelsPerInchY);
}

BOOL CPictureEx::Load()
{
	m_pGIFHeader = (TGIFHeader*)m_pRawData;

	if(	(memcmp(&m_pGIFHeader->m_cSignature,"GIF",3) != 0) &&	// it's neither GIF87a nor GIF89a
	   ((memcmp(&m_pGIFHeader->m_cVersion,"87a",3) != 0) ||		// do the default processing     
		(memcmp(&m_pGIFHeader->m_cVersion,"89a",3) != 0)) )
	{
		TFrame frame;
		memset(&frame, 0, sizeof(TFrame));

		if( !LoadPicture(m_hGlobal, m_nDataSize, frame) )
		{
			ClearGlobal();
			return FALSE;
		}

		SIZE hmSize;
		frame.m_pPicture->get_Width(&hmSize.cx);
		frame.m_pPicture->get_Height(&hmSize.cy);

		HiMetricToPixel(&hmSize, &m_PictureSize);
		m_arrFrames[0].m_frameSize = m_PictureSize;
		ClearGlobal();
	}
	else
	{
		// it's a GIF
		m_pGIFLSDescriptor = (TGIFLSDescriptor*)(m_pRawData + sizeof(TGIFHeader));

		if( m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1 )
		{
			// calculate the globat color table size
			m_nGlobalCTSize = 3* (1 << (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1));

			// get the background color if GCT is present
			unsigned char *pBkClr = m_pRawData + sizeof(TGIFHeader) + 
				sizeof(TGIFLSDescriptor) + 3*m_pGIFLSDescriptor->m_cBkIndex;
			m_clrBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
		};

		// store the picture's size
		m_PictureSize.cx = m_pGIFLSDescriptor->m_wWidth;
		m_PictureSize.cy = m_pGIFLSDescriptor->m_wHeight;
		if( !LoadFrames() )
			return FALSE;
	}

	return TRUE;
}

BOOL CPictureEx::LoadFrames()
{
	m_nCurrOffset = sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;

	TFrame frame;
	UINT nBlockLen;
	HGLOBAL hFrameData;
	while( hFrameData = GetNextGraphicBlock(&nBlockLen, &frame.m_nDelay, &frame.m_frameSize,
		&frame.m_frameOffset, &frame.m_nDisposal) )
	{
		GlobalUnlock(hFrameData);
		LoadPicture(hFrameData, nBlockLen, frame);
		GlobalFree(hFrameData);
	}

	ClearGlobal();
	return !m_arrFrames.empty();
}

BOOL CPictureEx::LoadPicture(HGLOBAL hGlobal, DWORD dwSize, TFrame &tFrame)
{
	IStream *pStream = NULL;
	IPicture *pPicture = NULL;
	IPersistStream *pPersistentStream = NULL; 

	HRESULT hRes = CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);

	if( !FAILED(hRes) )
		hRes = CoCreateInstance(CLSID_StdPicture, NULL, CLSCTX_INPROC_SERVER, IID_IPicture, (void**)&pPicture);

	if( !FAILED(hRes) && pPicture )
	{
		pPicture->put_KeepOriginalFormat(FALSE); 
        hRes = pPicture->QueryInterface(IID_IPersistStream,(void **)&pPersistentStream);
	}

	if( !FAILED(hRes) && pPersistentStream )
		hRes = pPersistentStream->Load(pStream);

	if( pPersistentStream )
		pPersistentStream->Release();

	if( pStream )
		pStream->Release();

	if( FAILED(hRes) || !pPicture )
	{
		if( pPicture )
			pPicture->Release();
		return FALSE;
	}

	tFrame.m_pPicture = pPicture;
	m_arrFrames.push_back(tFrame);
	return TRUE;
}

BOOL CPictureEx::Draw(HDC dc, UINT nCurrFrame, bool bTransparent)
{
	if( !bTransparent )
	{
		RECT rc = {0, 0, m_PictureSize.cx, m_PictureSize.cx};
		FillRect(dc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	TFrame &frame = m_arrFrames[nCurrFrame];

	long hmWidth, hmHeight;
	frame.m_pPicture->get_Width(&hmWidth);
	frame.m_pPicture->get_Height(&hmHeight);

	HRESULT res = frame.m_pPicture->Render(dc, 
		frame.m_frameOffset.cx, 
		frame.m_frameOffset.cy, 
		frame.m_frameSize.cx, 
		frame.m_frameSize.cy, 
		0, hmHeight, hmWidth, -hmHeight, NULL);

	return res == S_OK;
}

void CPictureEx::Frame(UINT nCurrFrame, DWORD &nTimeout)
{
	TFrame &frame = m_arrFrames[nCurrFrame];
	nTimeout = m_arrFrames.size() <= 1 ? 0 : frame.m_nDelay < 5 ? 100 : frame.m_nDelay*10;
}

BOOL CPictureEx::Next(UINT &nCurrFrame, DWORD &nTimeout)
{
	Frame(nCurrFrame, nTimeout);
	if( ++nCurrFrame >= m_arrFrames.size() )
		nCurrFrame = 0;

	return TRUE;
}

UINT CPictureEx::GetSubBlocksLen(UINT nStartingOffset) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;
	
	while (m_pRawData[nCurOffset] != 0)
	{
		nRet += m_pRawData[nCurOffset]+1;
		nCurOffset += m_pRawData[nCurOffset]+1;
	};

	return nRet+1;
}

BOOL CPictureEx::SkipNextBlock()
{
	if (!m_pRawData) return FALSE;

	int nLen = GetNextBlockLen();
	if ((nLen <= 0) || ((m_nCurrOffset+nLen) > m_nDataSize))
		return FALSE;

	m_nCurrOffset += nLen;
	return TRUE;
}

int CPictureEx::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch(nBlock)
	{
	case BLOCK_UNKNOWN: return -1;
	case BLOCK_TRAILER: return 1;
	case BLOCK_APPEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFAppExtension));
		if (nTmp > 0)
			return sizeof(TGIFAppExtension)+nTmp;
		break;
	case BLOCK_COMMEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFCommentExt));
		if (nTmp > 0)
			return sizeof(TGIFCommentExt)+nTmp;
		break;
	case BLOCK_CONTROLEXT: return sizeof(TGIFControlExt);
	case BLOCK_PLAINTEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFPlainTextExt));
		if (nTmp > 0)
			return sizeof(TGIFPlainTextExt)+nTmp;
		break;
	case BLOCK_IMAGE:
		TGIFImageDescriptor *pIDescr = (TGIFImageDescriptor*)&m_pRawData[m_nCurrOffset];
		int nLCTSize = (int)(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*
			(1 << (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1)));

		int nTmp = GetSubBlocksLen(m_nCurrOffset + sizeof(TGIFImageDescriptor) + nLCTSize + 1);
		if (nTmp > 0)
			return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
		break;
	};

	return 0;
}

HGLOBAL CPictureEx::GetNextGraphicBlock(UINT *pBlockLen, 
	UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal)
{
	if (!m_pRawData) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)
		)
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart = m_nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = 
			reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
		// store delay time
		*pDelay = pControl->m_wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while (
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)
			)
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		};

		if ((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value

	if (nBlock == BLOCK_IMAGE)
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = (TGIFImageDescriptor*)&m_pRawData[m_nCurrOffset];
		pBlockSize->cx = pImage->m_wWidth;
		pBlockSize->cy = pImage->m_wHeight;
		pBlockOffset->cx = pImage->m_wLeftPos;
		pBlockOffset->cy = pImage->m_wTopPos;
	};

	if (!SkipNextBlock()) return NULL;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, sizeof(TGIFHeader) + sizeof(TGIFLSDescriptor) +
		m_nGlobalCTSize + nBlockLen + 1);  // for the trailer

	if (!hGlobal) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *> (hGlobal);

	CopyMemory(pGlobal,m_pRawData, sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize);
	nOffset += sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;

	CopyMemory(pGlobal + nOffset,&m_pRawData[nStart], nBlockLen);
	nOffset += nBlockLen;

	pGlobal[nOffset++] = 0x3B; // trailer
	*pBlockLen = nOffset;

	return hGlobal;
}

//////////////////////////////////////////////////////////////////////

inline int CPictureEx::TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:  nRet = (nRet & 28) >> 2; break;
	case GCX_PACKED_USERINPUT: nRet = (nRet & 2) >> 1; break;
	case GCX_PACKED_TRANSPCOLOR: nRet &= 1; break;
	};
	return nRet;
}

inline int CPictureEx::TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case LSD_PACKED_GLOBALCT: nRet = nRet >> 7; break;
	case LSD_PACKED_CRESOLUTION: nRet = ((nRet & 0x70) >> 4) + 1; break;
	case LSD_PACKED_SORT: nRet = (nRet & 8) >> 3; break;
	case LSD_PACKED_GLOBALCTSIZE: nRet &= 7; break;
	};
	return nRet;
}

inline int CPictureEx::TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case ID_PACKED_LOCALCT: nRet >>= 7; break;
	case ID_PACKED_INTERLACE: nRet = ((nRet & 0x40) >> 6); break;
	case ID_PACKED_SORT: nRet = (nRet & 0x20) >> 5; break;
	case ID_PACKED_LOCALCTSIZE: nRet &= 7; break;
	};
	return nRet;
}

enum CPictureEx::GIFBlockTypes CPictureEx::GetNextBlock() const
{
	switch(m_pRawData[m_nCurrOffset])
	{
	case 0x21:
		switch(m_pRawData[m_nCurrOffset+1]) // extension block
		{
		case 0x01: return BLOCK_PLAINTEXT; // plain text extension
		case 0xF9: return BLOCK_CONTROLEXT; // graphic control extension
		case 0xFE: return BLOCK_COMMEXT; // comment extension
		case 0xFF: return BLOCK_APPEXT; // application extension
		};
		break;
	case 0x3B: return BLOCK_TRAILER; // trailer
	case 0x2C: return BLOCK_IMAGE; // image data
	};
	return BLOCK_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////
