// Graphics.cpp
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UniRoot.h"
#include "../Graphics.h"

//////////////////////////////////////////////////////////////////////
// CFontStyleImpl

class CFontStyleImpl : public CFontStyle
{
public:
	CFontStyleImpl() : m_pFont(NULL), m_pFontU(NULL), m_pPen(NULL), m_pMetr(NULL) {}
	~CFontStyleImpl() { Clear(); }

	void Destroy(){ delete this; };
	void Clear();
	void Create(bool bU = false);
	bool Init(CRootItem * pRoot, LPCSTR strName, tDWORD nHeight, COLORREF nColor, tDWORD nBold, bool bItalic, bool bUnderline, tDWORD nCharExtra);
	bool Draw(HDC dc, LPCSTR strText, RECT& rc, tDWORD nFlags);
	void GetInfo(INFO& pInfo);

	HFONT    Handle()   { return (HFONT)m_pFont; }
	COLORREF& GetColor() { return m_clColor; }
	tString  GetName()  { return m_strName; }
	
protected:
	CUniRoot *     m_pRoot;
	QFont *        m_pFont;
	QFont *        m_pFontU;
	QPen *         m_pPen;
	QFontMetrics * m_pMetr;
	
	COLORREF       m_clColor;
	tDWORD         m_nHeight;
	tDWORD         m_nCharExtra;
	tDWORD         m_nBold;
	tString        m_strName;
	unsigned       m_bItalic : 1;
	unsigned       m_bUnderline : 1;
};

//////////////////////////////////////////////////////////////////////

void CFontStyleImpl::Clear()
{
	if( m_pFont )  delete m_pFont,  m_pFont  = NULL;
	if( m_pFontU ) delete m_pFontU, m_pFontU = NULL;
	if( m_pPen )   delete m_pPen,   m_pPen   = NULL;
	if( m_pMetr )  delete m_pMetr,  m_pMetr  = NULL;
}

void CFontStyleImpl::Create(bool bU)
{
	int weight = m_nBold/10;

	QFont * pFont = new QFont(QString::fromLocal8Bit(m_strName.c_str()), m_nHeight, weight, m_bItalic);
	pFont->setUnderline(bU ? true : m_bUnderline);
	
	if( bU )
		m_pFontU = pFont;
	else
		m_pFont = pFont;

	m_pPen = new QPen(QColor(GetRValue(m_clColor), GetGValue(m_clColor), GetBValue(m_clColor)));
}

bool CFontStyleImpl::Init(CRootItem * pRoot, LPCSTR strName, tDWORD nHeight, COLORREF nColor, tDWORD nBold, bool bItalic, bool bUnderline, tDWORD nCharExtra)
{
	Clear();

	m_pRoot = (CUniRoot *)pRoot;
	m_clColor = nColor;
	m_nHeight = nHeight;
	m_nCharExtra = nCharExtra;
	m_nBold = nBold;
	m_bItalic = bItalic;
	m_bUnderline = bUnderline;
	m_strName = strName;

	Create();
	return true;
}

bool CFontStyleImpl::Draw(HDC dc, LPCSTR strText, RECT& rc, tDWORD nFlags)
{
	if( !dc )
		return false;
	
	int nDrawFlags = Qt::TextWordWrap|Qt::TextShowMnemonic;
	if( nFlags & ALIGN_RIGHT )
		nDrawFlags |= Qt::AlignRight;
	else if( nFlags & ALIGN_HCENTER )
		nDrawFlags |= Qt::AlignHCenter;
	if( nFlags & ALIGN_VCENTER )
		nDrawFlags |= Qt::AlignVCenter;

	if( nFlags & TEXT_SINGLELINE )
		nDrawFlags |= Qt::TextSingleLine;
	
	const QString& qsDraw = _S2Q(strText);
	if( nFlags & TEXT_CALCSIZE )
	{
		if( !m_pFont )
			return false;

		if( !m_pMetr )
		{
			m_pMetr = new QFontMetrics(*m_pFont);
			if( !m_pMetr )
				return false;
		}
		
		//nDrawFlags |= Qt::TextDontPrint;

		bool bStrNull = false;
		if( !strText || *strText == '\0' )
		{
			strText = "W";
			bStrNull = true;
		}
		
		const QRect& qrc = m_pMetr->boundingRect(rc.left, rc.top, RECT_CX(rc), RECT_CY(rc), nDrawFlags, qsDraw, qsDraw.length());
		rc.left   = qrc.left();
		rc.top    = qrc.top();
		rc.right  = qrc.right();
		rc.bottom = qrc.bottom();

		if( rc.left < rc.right )
			rc.right++;

		if( bStrNull )
			rc.right = rc.left;
	}
	else
	{
		QPainter& painter = *(QPainter *)dc;

		QFont * pFont = m_pFont;
		QPen *  pPen  = m_pPen;
		
		if( nFlags & TEXT_GRAYED )
			pPen = m_pRoot->m_pPenGrayed;
		else if( nFlags & TEXT_HOTLIGHT )
		{
			if( m_bUnderline )
				pPen = m_pRoot->m_pPenHotlight;
			else
			{
				if( !m_pFontU ) Create(true);
				pFont = m_pFontU;
			}
		}
		
		//painter.save();

		if( pFont ) painter.setFont(*pFont);
	
		if( pPen )  painter.setPen(*pPen);
		painter.setBackgroundMode((nFlags & TEXT_OPAQUE) ? Qt::OpaqueMode : Qt::TransparentMode);

		painter.drawText(QRect(rc.left, rc.top, RECT_CX(rc), RECT_CY(rc)), nDrawFlags, qsDraw);
		
		//painter.restore();
	}

	return true;
}

void CFontStyleImpl::GetInfo(INFO& pInfo)
{
	pInfo.m_nHeight    = m_nHeight;
	pInfo.m_nBold      = m_nBold;
	pInfo.m_bItalic    = m_bItalic;
	pInfo.m_bUnderline = m_bUnderline;
	pInfo.m_nCharExtra = m_nCharExtra;
}

//////////////////////////////////////////////////////////////////////

CFontStyle * CUniRoot::CreateFont()
{
	return new CFontStyleImpl();
}

//////////////////////////////////////////////////////////////////////
// CAlphaBrush

void CAlphaBrush::Draw(HDC dc, RECT& rc)
{
	QPainter& painter = *(QPainter *)dc;

	QBrush qbr(QColor(GetRValue(m_cl), GetGValue(m_cl), GetBValue(m_cl)));
	painter.fillRect(rc.left, rc.top, RECT_CX(rc), RECT_CY(rc), qbr);
}

//////////////////////////////////////////////////////////////////////
// CBackgroundImpl

// RM - Need to reconsider!
CBackground * CUniRoot::CreateBackground(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle, CImageBase* pImage)
{
	if( !pImage )
		pImage = new CBackgroundBase();

	if( pImage )
		pImage->m_pRoot = this;

// RM
	((CBackgroundBase *)pImage)->Init(nBGColor, nFrameColor, nRadius, nStyle);
	return pImage;
}

//////////////////////////////////////////////////////////////////////
// CIconImpl

class CIconImpl : public CImageBase, public QIcon
{
public:
	CIconImpl() { m_sz.cx = m_sz.cy = 0; }
	~CIconImpl(){ Clear(); }

	void  Clear() {}
	bool  Init(const QImage& image);
	bool  Init(const QIcon * icon, bool bRef);
	
public:
	bool  Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx);
	bool  Merge(CImageBase * pIcon);
	SIZE  Size() { return m_sz; }
	HICON IconHnd() { return (HICON)(QIcon *)this; }
	
protected:
	SIZE m_sz;
};

//////////////////////////////////////////////////////////////////////

bool CIconImpl::Init(const QImage& image)
{
	if( image.isNull() )
		return false;
	
	addPixmap(QPixmap::fromImage(image));
	m_sz.cx = image.width();
	m_sz.cy = image.height();
	return true;
}

bool CIconImpl::Init(const QIcon * icon, bool bRef)
{
	if( !icon || icon->isNull() )
		return false;

	*(QIcon *)this = *icon;
	if( bRef )
		delete icon;
	
	QSize szActual = actualSize(QSize(1000, 1000));
	m_sz.cx = szActual.width();
	m_sz.cy = szActual.height();
	return true;
}

bool CIconImpl::Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx)
{
	if( !dc )
		return false;
	
	QPainter& painter = *(QPainter *)dc;
	paint(&painter, rc.left, rc.top, RECT_CX(rc), RECT_CY(rc), Qt::AlignLeft|Qt::AlignTop);
	return true;
}

bool CIconImpl::Merge(CImageBase * pIcon)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
// CStdImage

class CStdImage : public CImageBase, public QPixmap
{
public:
	CStdImage() {}
	~CStdImage() {}
	
	void     Clear() {}
	bool     Init(const QImage& image, int nImageSizeX);
	bool     IsValid() { return true; }
	bool     IsTransparent() { return true; }
	bool     Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx);
	SIZE     Size() { return m_sz; }
	tDWORD   Count(){ return m_sz.cx ? (m_szImageX / m_sz.cx) : 1; }

protected:
	SIZE m_sz;
	int  m_szImageX;
};

//////////////////////////////////////////////////////////////////////

bool CStdImage::Init(const QImage& image, int nImageSizeX)
{
	if( image.isNull() )
		return false;
	
	*(QPixmap *)this = QPixmap::fromImage(image);
	m_szImageX = image.width();
	m_sz.cx = nImageSizeX ? nImageSizeX : m_szImageX;
	m_sz.cy = image.height();
	return true;
}

bool CStdImage::Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx)
{
	if( !dc )
		return false;

	if( pCtx && !nIdx )
		nIdx = pCtx->m_nCurFrame;
	
	QPainter& painter = *(QPainter *)dc;
	
	QRect qrDst(rc.left, rc.top, RECT_CX(rc), RECT_CY(rc));
	QRect qrSrc(nIdx*m_sz.cx, 0, m_sz.cx, m_sz.cy);
	
	if( qrSrc.left() > m_szImageX )
		qrSrc.setLeft(m_szImageX);
	if( qrSrc.right() > m_szImageX )
		qrSrc.setRight(m_szImageX);

	if( qrDst.width() > qrSrc.width() )
		qrDst.setWidth(qrSrc.width());
	if( qrDst.height() > qrSrc.height() )
		qrDst.setHeight(qrSrc.height());
	
	painter.drawPixmap(qrDst, *this, qrSrc);
	return true;
}

//////////////////////////////////////////////////////////////////////

CImageBase * CUniRoot::CreateImage(eImageType eType, hIO pIo, int nImageSizeX, int nImageNotStretch, int nStyle, CImageBase* pImage)
{
	if( pIo->propGetDWord(pgPLUGIN_ID) != PID_NATIVE_FIO )
	{
		if( pImage )
			pImage->Destroy();
		return NULL;
	}

	cStringObj strPath; strPath.assign(pIo, pgOBJECT_FULL_NAME);
	QImage image(_S2Q(strPath.data()));

	bool bInited = true;
	bool bNew = !pImage;

	if( !nImageSizeX && eType == eImageIcon )
	{
		if( !pImage )
			pImage = new CIconImpl();
		bInited = ((CIconImpl *)pImage)->Init(image);
	}
	else
	{
		if( !pImage )
			pImage = new CStdImage();
		bInited = ((CStdImage *)pImage)->Init(image, nImageSizeX);
	}

	if( bNew && !bInited && pImage )
		pImage->Destroy(), pImage = NULL;

	if( pImage )
		pImage->m_pRoot = this;
	
	return pImage;
}

//////////////////////////////////////////////////////////////////////

CIcon * CUniRoot::CreateIcon(HICON hIcon, bool bRef, CImageBase * pImage)
{
	if( !pImage )
		pImage = new CIconImpl();

	if( pImage )
		pImage->m_pRoot = this;

	if( !((CIconImpl *)pImage)->Init((QIcon *)hIcon, bRef) )
		delete pImage, pImage = NULL;
	return pImage;
}

//////////////////////////////////////////////////////////////////////
// Win32 GDI API Substitutes

extern "C" COLORREF __stdcall GetPixel(HDC, int, int)
{
	return RGB(0xFF, 0xFF, 0xFF);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
