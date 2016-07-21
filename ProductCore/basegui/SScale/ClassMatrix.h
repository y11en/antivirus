#ifndef MATRIX_TEMPLATE_DEFINED
#define MATRIX_TEMPLATE_DEFINED

#define DEFAULT_BUFFER_SIZE_FOR_SCALING 1000

template<class T> class CMatrix;

template<class T>
class CMatrixRow
{
public:
	T *pRow;

	operator T*()
	{
		return pRow;
	}

	T &operator[](char nIndex)
	{
		return pRow[nIndex];
	}

	T &operator[](short nIndex)
	{
		return pRow[nIndex];
	}

	T &operator[](int nIndex)
	{
		return pRow[nIndex];
	}
	
	T &operator[](long nIndex)
	{
		return pRow[nIndex];
	}
};

template<class T>
class CMatrix
{
public:
	CMatrix()
	{
		m_Content = NULL;
	}
	
	~CMatrix()
	{
		Destroy();
	}

	bool Create(int DimY = 1, int DimX = 1, bool fZeroInit = false)
	{
		if(DimY < 1 || DimX < 1)
			return false;

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = DimX - 1;
		rc.bottom = DimY - 1;

		return CMatrix<T>::Create(&rc, fZeroInit);
	}

	bool Create(LPCRECT pRect, bool fZeroInit = false)
	{
		int i, SizeMatrix;
		char *Pointer2;
		CMatrixRow<T> *MemBlock, *Pointer1;

		int SizeX = pRect->right - pRect->left + 1;
		int SizeY = pRect->bottom - pRect->top + 1;
		int ItemSize = sizeof(T);
		
		Destroy();

		m_Rect = *pRect;
		m_SizeY = SizeY;
		m_SizeX = SizeX;
		SizeMatrix = (SizeX * SizeY) * ItemSize + SizeY * sizeof(CMatrixRow<T>);
		
		if((MemBlock = (CMatrixRow<T> *)malloc(SizeMatrix)) == NULL)
		{
			m_Content = NULL;
			return false;
		}
		
		if(fZeroInit)
			memset(MemBlock, 0, SizeMatrix);

		Pointer1 = MemBlock;
		Pointer2 = (char *)Pointer1 + SizeY * sizeof(CMatrixRow<T>) - pRect->left * ItemSize;
		
		ItemSize *= SizeX;

		for(i = SizeY; i != 0; i--, Pointer1++, Pointer2 += ItemSize)
			Pointer1->pRow = (T *)Pointer2;
		
		m_Content = MemBlock - pRect->top;
		
		return true;
	}

	void Destroy()
	{
		if(m_Content != NULL)
			free(m_Content + m_Rect.top);
		m_Content = NULL;
	}

	int GetX() const
	{
		return m_SizeX;
	}
	
	int	GetY() const
	{
		return m_SizeY;
	}
	
	RECT GetRect() const
	{
		return m_Rect;
	}

	CMatrixRow<T>& operator[](int nIndex)
	{
		//assert(m_Content && nIndex >= m_Rect.top && nIndex <= m_Rect.bottom);
		return m_Content[nIndex];
	}

	CMatrixRow<T>* operator+(int nIndex)
	{
		return m_Content + nIndex;
	}

	CMatrix& operator=(const CMatrix &InMatrix)
	{
		if(!m_Content || !EqualRect(&m_Rect, &InMatrix.m_Rect))
			Create(&InMatrix.m_Rect, false);

		CMatrixRow<T>	*Line1 = m_Content + m_Rect.top,
						*Line2 = InMatrix.m_Content + m_Rect.top;
		
		int Len = m_SizeX * sizeof(T);
		for(int i = m_SizeY; i > 0; i--, Line1++, Line2++)
		{
			memcpy(*Line1 + m_Rect.left, *Line2 + m_Rect.left, Len);
		}
		
		return *this;
	}

	void CopyPartMatr(CMatrix &SourceMatrix, LPCRECT pSourceRect, LPPOINT pDestBasePoint)
	{
		int Len = (pSourceRect->right - pSourceRect->left + 1) * sizeof(T),
			SizeY = pSourceRect->bottom - pSourceRect->top + 1;
		
		CMatrixRow<T>	*Line1 = m_Content + pDestBasePoint->y,
						*Line2 = SourceMatrix.m_Content + pSourceRect->top;
		
		for(int i = SizeY; i; i--, Line1++, Line2++)
		{
			memcpy(*Line1 + pDestBasePoint->x, *Line2 + pSourceRect->left, Len);
		}
	}

	void ZeroInit()
	{
		memset(m_Content + m_Rect.top + m_SizeY, 0, m_SizeX * m_SizeY * sizeof(T));
	}

	bool IsReady() const
	{
		return(m_Content != NULL);
	}

	void Fill(T& Element)
	{
		T *Line;
		
		int i, j;
		for(i = 0; i < m_SizeY; i++)
		{
			Line = *(m_Content + m_Rect.top + i); 
			for(j = m_Rect.left; j < m_Rect.left + m_SizeX; j++)
				Line[j] = Element;
		}
	}

protected:
	CMatrixRow<T> *m_Content;

	int m_SizeY;
	int m_SizeX;
	RECT m_Rect;
};

typedef struct tagRGBA {
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
} RGBA;

typedef CMatrix<int>			CIntMatrix;
typedef CMatrix<unsigned char>	CByteMatrix;
typedef CMatrix<unsigned short> CWordMatrix;
typedef CMatrix<float> 			CFloatMatrix;
typedef CMatrix<double>			CDoubleMatrix;
typedef	CMatrix<RGBA>			CColorMatrix32;

bool SuperRescaleBitmapWithAlpha(CColorMatrix32 &SrcMatrix,LPRECT pSrcRect,
								 CColorMatrix32 &DstMatrix,LPRECT pDstRect,
								 int RecommendedBufferSize = DEFAULT_BUFFER_SIZE_FOR_SCALING);

#endif