// ItemStyles.h: interface for the CItemPropsa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEDEFS_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_)
#define AFX_BASEDEFS_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_

#include <string>

class CItemBase;
class CRootItem;

#ifndef __PR_TYPES_H
typedef unsigned int tDWORD;
#endif

#ifdef _WIN32
#include <windows.h>
#endif // _WINDOWS

#ifndef _WINDOWS_

typedef void*            HINSTANCE;
typedef void*            HANDLE;
typedef void*            HWND;
typedef void*            HBRUSH;
typedef void*            HICON;
typedef void*            HFONT;
typedef void*            HMENU;
typedef void*            HDC;
typedef unsigned long    COLORREF;
typedef const char *     LPCSTR;
typedef const wchar_t*   LPCWSTR;
typedef char *           LPSTR;
typedef wchar_t*         LPWSTR;
typedef unsigned char    BYTE;
typedef int              BOOL;
typedef char             CHAR;
typedef unsigned short   WORD;
typedef unsigned int     DWORD;
typedef unsigned int     UINT;
typedef const char*      LPCTSTR;
typedef char*            LPTSTR;
typedef long             LONG;
#if defined(__LP64__)
typedef unsigned long long UINT_PTR;
#else
typedef unsigned int     UINT_PTR;
#endif
typedef struct tagPOINT{long x; long y;}  POINT;
typedef POINT * LPPOINT;
typedef const POINT * LPCPOINT;

typedef struct tagSIZE{long cx; long cy;} SIZE;
typedef struct tagRECT{long left; long top; long right; long bottom;} RECT;
typedef RECT * LPRECT;
typedef const RECT *LPCRECT;

typedef struct tagRGBQUAD {
        BYTE       rgbBlue;
        BYTE       rgbGreen;
        BYTE       rgbRed;
        BYTE       rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
        long       biSize;
        long       biWidth;
        long       biHeight;
        short      biPlanes;
        short      biBitCount;
        long       biCompression;
        long       biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        long       biClrUsed;
        long       biClrImportant;
} BITMAPINFOHEADER;

#define MB_TYPEMASK                 0x0000000FL
#define MB_ICONMASK                 0x000000F0L
#define MB_DEFMASK                  0x00000F00L
#define MB_MODEMASK                 0x00003000L
#define MB_MISCMASK                 0x0000C000L

#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L

#define MB_ICONERROR                0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONINFORMATION          0x00000040L
#define MB_ICONWARNING		        MB_ICONEXCLAMATION

#define GetRValue(rgb)              ((tBYTE)(rgb))
#define GetGValue(rgb)              ((tBYTE)(((tWORD)(rgb)) >> 8))
#define GetBValue(rgb)              ((tBYTE)((rgb)>>16))
#define RGB(r,g,b)          	    ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define CLR_INVALID                 0xFFFFFFFF

// Win32 GDI API Substitutes
extern "C" COLORREF __stdcall GetPixel(HDC, int, int);

template <class T>
struct _TempObjRef
{
	_TempObjRef(const T& obj) : m_obj (obj) {}
	operator T& () const { return const_cast<T&>(m_obj); }
	const T& m_obj;
};

#define TOR(_T, _args) _TempObjRef<_T>(_T _args).operator _T&()

#else // _WINDOWS_
	#define TOR(_T, _args) _T _args
#endif // _WINDOWS_

#define IMAGELIST_SIZE              6

typedef std::string          tString;
typedef std::wstring         tStringW;
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > TSTRING;
typedef unsigned             id_t;

#define TOR_cStrObj TOR(cStrObj,())
#define TOR_tString TOR(tString,())

enum DTT_TYPE{DTT_DT, DTT_DATE, DTT_TIME, DTT_TIMESHORT, DTT_TIMESTAMP};

//////////////////////////////////////////////////////////////////////

enum enBaseGuiStructTypes
{
	bguist__start = 1000,
	
	bguist_cGuiParams,
	bguist_CAttachedFile,
	bguist_CSendMailInfo,
	bguist_CDeviceAction,
	bguist_cGuiVectorInfo,
	bguist_cGuiMsgBox,
	bguist_cGridViewTemplate,
};

//////////////////////////////////////////////////////////////////////

enum enUserAction
{
	sdUnknown,
	sdNext,
	sdPrev,
	sdLeft,
	sdRight,
	sdPageNext,
	sdPagePrev,
	sdHome,
	sdEnd,
	sdExactPos,
	sdSpace,
	sdDelete,
	sdEnter,
};

enum enUserState
{
	fLButton  = 0x0001,
	fRButton  = 0x0002,

	fDouble   = 0x0004,

	fControl  = 0x0010,
	fShift    = 0x0020,
};

enum enScrollType
{
	stVScroll,
	stHScroll,
};

enum enDrawRect
{
	drRect      = 0x0000, // default
	drColHeader = 0x0001,
	drBorder    = 0x0002,
	drCheckBox  = 0x0004,
	drRadioBox  = 0x0008,

	drPressed   = 0x1000,
	drFocused   = 0x2000,
	drSelected  = 0x4000,
	drHotlight  = 0x8000,
	drDisabled  = 0x10000,
	drFlat      = 0x20000,
	drInvert    = 0x40000,
	drSortUp    = 0x80000,
	drSortDown  = 0x100000,
};

enum enCursorType
{
	ctDefault   = 0x0000,
	ctResize    = 0x0001,
	ctDragCol   = 0x0002,
	ctNo        = 0x0003,
	ctHand      = 0x0004,
};

enum enDragType
{
	dtMove      = 0x0000,
	dtHide      = 0x0001,
	dtShow      = 0x0002,
	dtEnd       = 0x0003,
};

struct cScrollInfo
{
	enScrollType nType;
    tDWORD		 nMax;
    tDWORD		 nPage;
    tDWORD		 nPos;
};

//////////////////////////////////////////////////////////////////////

class CBgContext
{
public:
	CBgContext(){ memset(this, 0, sizeof(CBgContext)); }
	long    	 m_nCurFrame;
	COLORREF     m_clBgTop;
	COLORREF     m_clBgBottom;
	bool         m_bGrayed;
};

class CImageBase
{
public:
	enum enStyle
	{
		None    = 0x000,
		Top     = 0x001,
		Bottom  = 0x002,
		Left    = 0x004,
		Right   = 0x008,
		System  = 0x010,
		Mozaic  = 0x020,
		Transparent = 0x040,
	};

	CImageBase() : m_ref(1) {}
	virtual ~CImageBase() {}

	virtual void     AddRef()  { m_ref++; }
	virtual void     Destroy() { if( !(--m_ref) ) delete this; }

	virtual bool     Draw(HDC dc, RECT& rc, CBgContext* pCtx = NULL, tDWORD nIdx = 0) = 0;
	virtual bool     Merge(CImageBase * pIcon, tDWORD nIdx = 0) { return false; }
	virtual bool     IsTransparent() { return false; }

	virtual SIZE     Size() = 0;
	virtual tDWORD   Count()     { return 1; }
	virtual HBRUSH   BrushHnd()  { return NULL; }
	virtual HICON    IconHnd()   { return NULL; }
	virtual HDC      DcHnd()     { return NULL; }
	virtual COLORREF BGColor()   { return -1; }
	virtual void*    ImageList() { return NULL; }

public:
	CRootItem*   m_pRoot;
	tDWORD       m_ref;
};

typedef CImageBase    CIcon;
typedef CImageBase    CBackground;

class CFontStyle
{
public:
	struct INFO
	{
		long     m_nHeight;
		long     m_nBold;
		bool     m_bItalic;
		bool     m_bUnderline;
		long     m_nCharExtra;
	};
	
	virtual void     Destroy()=0;
	virtual bool     Init(CRootItem * pRoot, LPCSTR strName, tDWORD nHeight, COLORREF nColor, tDWORD nBold, bool bItalic=false, bool bUnderline=false, tDWORD nCharExtra=0)=0;
	virtual bool     Draw(HDC dc, LPCSTR strText, RECT& rc, tDWORD nFlags)=0;
	virtual HFONT    Handle(){ return NULL; }
	
	virtual tString  GetName()=0;
	virtual COLORREF& GetColor()=0;
	virtual void     GetInfo(INFO& pInfo)=0;
};

class CBorder
{
public:
	enum StyleFlags
	{
		Top     = 0x001,
		Bottom  = 0x002,
		Left    = 0x004,
		Right   = 0x008,
		XPBrush = 0x010,
	};

	virtual void   Destroy()=0;
	virtual bool   Init(tDWORD nStyle, COLORREF clColor, tDWORD nWidth)=0;
	virtual bool   Draw(HDC dc, RECT& rc, bool p_fCalcRect = false)=0;
	virtual tDWORD Width()=0;
	virtual StyleFlags GetStyle()=0;
};

//////////////////////////////////////////////////////////////////////

class CImageList_
{
public:
	CImageList_() : m_pImage(NULL), m_nIndex(NULL) {}
	CImageList_(const CImageList_& src) : m_pImage(NULL), m_nIndex(NULL) { operator =(src); }
	~CImageList_() { Clean(); }
	
	CImageList_& operator =(const CImageList_& src)
	{
		m_pImage = src.m_pImage;
		if( src.m_nIndex )
			memcpy(m_nIndex ? m_nIndex : m_nIndex = new tDWORD[IMAGELIST_SIZE],
				src.m_nIndex, IMAGELIST_SIZE * sizeof(tDWORD));
		else
			Clean();
		return *this;
	}

	void operator =(CImageBase* src) { m_pImage = src; }

	CImageBase* operator ->() { return m_pImage; }
	operator CImageBase*()    { return m_pImage; }

	void   Clean() { delete [] m_nIndex; m_nIndex = NULL; }
	bool   IsIndexed() { return !!m_nIndex; }

	bool   SetImage(tDWORD nPos, tDWORD nIdx)
	{
		if( nPos >= IMAGELIST_SIZE )
			return false;
		
		if( !m_nIndex )
			memset(m_nIndex = new tDWORD[IMAGELIST_SIZE], 0xff, IMAGELIST_SIZE * sizeof(tDWORD));
		
		tDWORD nOldIdx = m_nIndex[nPos];
		m_nIndex[nPos] = nIdx;
		return nOldIdx != nIdx;
	}
	
	tDWORD GetImage(tDWORD nPos)
	{
		return m_nIndex && nPos < IMAGELIST_SIZE ? m_nIndex[nPos] : 0;
	}

private:
	CImageBase*      m_pImage;
	tDWORD*          m_nIndex;
};

#include "Variant.h"

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BASEDEFS_H__ACE32930_00DB_4B40_BD2D_AD0EE47B244F__INCLUDED_)
