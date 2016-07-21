//////////////////////////////////////////////////////////////////////
// PictureEx.cpp: implementation of the CPictureEx class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
#define AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_

#include <vector>
#include <ocidl.h>
#include <olectl.h>

//////////////////////////////////////////////////////////////////////

class CMemWndDC
{
public:
	CMemWndDC() : m_dc(NULL), m_bmp(NULL), m_oldbmp(NULL){}

	~CMemWndDC()
	{
		if( !m_dc )
			return;
		if( m_oldbmp )
			SelectObject(m_dc, m_oldbmp);
		if( m_bmp )
			DeleteObject(m_bmp);
		DeleteDC(m_dc);
	}

	bool Init(HWND wnd, bool bMonohrome)
	{
		HDC dc = GetDC(wnd);
		if( !dc )
			return false;
		m_dc = CreateCompatibleDC(dc);
		if( m_dc )
		{
			RECT rc; GetClientRect(wnd, &rc);
			m_size.cx = rc.right;
			m_size.cy = rc.bottom;

			if( bMonohrome )
				m_bmp = CreateBitmap(m_size.cx, m_size.cy, 1, 1, NULL);
			else
				m_bmp = CreateCompatibleBitmap(dc, m_size.cx, m_size.cy);

			if( m_bmp )
				m_oldbmp = (HBITMAP)SelectObject(m_dc, m_bmp);
		};
		ReleaseDC(wnd, dc);
		return m_bmp != NULL;
	}

	void Copy(HDC dc, DWORD flags = SRCCOPY)
	{
		BitBlt(dc, 0, 0, m_size.cx, m_size.cy, m_dc, 0, 0, flags);
	}

	void Grab(int x, int y)
	{
		HDC ddc = GetDC(NULL);
		BitBlt(m_dc, 0, 0, m_size.cx, m_size.cy, ddc, x, y, SRCCOPY);
		ReleaseDC(NULL, ddc);
	}

	void Invert()
	{
		RECT rc = {0, 0, m_size.cx, m_size.cy};
		InvertRect(m_dc, &rc);
	}

	operator HDC() const { return m_dc; }
	operator HBITMAP() const { return m_bmp; }

private:
	HDC             m_dc;
	HBITMAP         m_bmp;
	HBITMAP         m_oldbmp;
	SIZE            m_size;
};

//////////////////////////////////////////////////////////////////////

class CPictureEx
{
public:
	CPictureEx();
	virtual ~CPictureEx();

	// loads a picture from a program resource
	// i.e. Load(MAKEINTRESOURCE(IDR_MYPIC),_T("GIFTYPE"));
	BOOL Load(HINSTANCE hInstance, LPCTSTR szResourceName, LPCTSTR szResourceType);
	BOOL LoadFrames();

	BOOL Draw(HDC dc);
	BOOL Next(DWORD &nTimeout);

protected:
	SIZE             m_PictureSize;
	COLORREF         m_clrBackground;
	UINT             m_nCurrFrame;
	HGLOBAL          m_hGlobal;
	UINT             m_nDataSize;
	UINT             m_nCurrOffset;
	UINT             m_nGlobalCTSize;

public:
	struct TFrame    // structure that keeps a single frame info
	{
		IPicture *m_pPicture;  // pointer to the interface used for drawing
		SIZE     m_frameSize;
		SIZE     m_frameOffset;
		UINT     m_nDelay;     // delay (in 1/100s of a second)
		UINT     m_nDisposal;  // disposal method
	};

#pragma pack(1)   // turn byte alignment on

	enum GIFBlockTypes
	{
		BLOCK_UNKNOWN,
		BLOCK_APPEXT,
		BLOCK_COMMEXT,
		BLOCK_CONTROLEXT,
		BLOCK_PLAINTEXT,
		BLOCK_IMAGE,
		BLOCK_TRAILER
	};

	enum ControlExtValues // graphic control extension packed field values
	{
		GCX_PACKED_DISPOSAL,  // disposal method
		GCX_PACKED_USERINPUT,
		GCX_PACKED_TRANSPCOLOR
	};

	enum LSDPackedValues  // logical screen descriptor packed field values
	{
		LSD_PACKED_GLOBALCT,
		LSD_PACKED_CRESOLUTION,
		LSD_PACKED_SORT,
		LSD_PACKED_GLOBALCTSIZE
	};

	enum IDPackedValues   // image descriptor packed field values
	{
		ID_PACKED_LOCALCT,
		ID_PACKED_INTERLACE,
		ID_PACKED_SORT,
		ID_PACKED_LOCALCTSIZE
	};

	struct TGIFHeader       // GIF header  
	{
		char m_cSignature[3]; // Signature - Identifies the GIF Data Stream
							  // This field contains the fixed value 'GIF'
		char m_cVersion[3];	// Version number. May be one of the following:
							// "87a" or "89a"
	};

	struct TGIFLSDescriptor // Logical Screen Descriptor
	{
		WORD m_wWidth;	// 2 bytes. Logical screen width
		WORD m_wHeight; // 2 bytes. Logical screen height

		unsigned char m_cPacked;      // packed field	

		unsigned char m_cBkIndex;     // 1 byte. Background color index
		unsigned char m_cPixelAspect; // 1 byte. Pixel aspect ratio
		inline int GetPackedValue(enum LSDPackedValues Value);
	};

	struct TGIFAppExtension // application extension block
	{
		unsigned char m_cExtIntroducer; // extension introducer (0x21)
		unsigned char m_cExtLabel; // app. extension label (0xFF)
		unsigned char m_cBlockSize; // fixed value of 11
		char m_cAppIdentifier[8];   // application identifier
		char m_cAppAuth[3];  // application authentication code
	};

	struct TGIFControlExt // graphic control extension block
	{
		unsigned char m_cExtIntroducer; // extension introducer (0x21)
		unsigned char m_cControlLabel;  // control extension label (0xF9)
		unsigned char m_cBlockSize; // fixed value of 4
		unsigned char m_cPacked;    // packed field
		WORD m_wDelayTime;	// delay time
		unsigned char m_cTColorIndex; // transparent color index
		unsigned char m_cBlockTerm;   // block terminator (0x00)
	public:
		inline int GetPackedValue(enum ControlExtValues Value);
	};

	struct TGIFCommentExt  // comment extension block
	{
		unsigned char m_cExtIntroducer; // extension introducer (0x21)
		unsigned char m_cCommentLabel;  // comment extension label (0xFE)
	};

	struct TGIFPlainTextExt // plain text extension block
	{
		unsigned char m_cExtIntroducer;  // extension introducer (0x21)
		unsigned char m_cPlainTextLabel; // text extension label (0x01)
		unsigned char m_cBlockSize; // fixed value of 12
		WORD m_wLeftPos;    // text grid left position
		WORD m_wTopPos;     // text grid top position
		WORD m_wGridWidth;  // text grid width
		WORD m_wGridHeight; // text grid height
		unsigned char m_cCellWidth;  // character cell width
		unsigned char m_cCellHeight; // character cell height
		unsigned char m_cFgColor; // text foreground color index
		unsigned char m_cBkColor; // text background color index
	};

	struct TGIFImageDescriptor // image descriptor block
	{
		unsigned char m_cImageSeparator; // image separator byte (0x2C)
		WORD m_wLeftPos; // image left position
		WORD m_wTopPos;  // image top position
		WORD m_wWidth;   // image width
		WORD m_wHeight;  // image height
		unsigned char m_cPacked; // packed field
		inline int GetPackedValue(enum IDPackedValues Value);
	};

#pragma pack() // turn byte alignment off

	unsigned char *     m_pRawData;
	TGIFHeader *        m_pGIFHeader;
	TGIFLSDescriptor *  m_pGIFLSDescriptor;
	std::vector<TFrame> m_arrFrames;

	BOOL Load();
	BOOL LoadPicture(HGLOBAL hGlobal, DWORD dwSize, TFrame &tFrame);
	int  GetNextBlockLen() const;
	BOOL SkipNextBlock();
	enum GIFBlockTypes GetNextBlock() const;
	UINT GetSubBlocksLen(UINT nStartingOffset) const;
	HGLOBAL GetNextGraphicBlock(UINT *pBlockLen, UINT *pDelay, 
		SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal);
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
