class CDWordArray
{
public:

// Construction
        CDWordArray();

// Attributes
        int GetSize() const;
        int GetUpperBound() const;
        void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
        // Clean up
        void FreeExtra();
        void RemoveAll();

        // Accessing elements
        ULONG GetAt(int nIndex) const;
        void SetAt(int nIndex, ULONG newElement);
        ULONG& ElementAt(int nIndex);

        // Direct Access to the element data (may return NULL)
        const ULONG* GetData() const;
        ULONG* GetData();

        // Potentially growing the array
        void SetAtGrow(int nIndex, ULONG newElement);
        int Add(ULONG newElement);
        int Append(const CDWordArray& src);
        void Copy(const CDWordArray& src);

        // overloaded operator helpers
        ULONG operator[](int nIndex) const;
        ULONG& operator[](int nIndex);

        // Operations that move elements around
        void InsertAt(int nIndex, ULONG newElement, int nCount = 1);
        void RemoveAt(int nIndex, int nCount = 1);
        void InsertAt(int nStartIndex, CDWordArray* pNewArray);

// Implementation
protected:
        ULONG* m_pData;   // the actual array of data
        int m_nSize;     // # of elements (upperBound - 1)
        int m_nMaxSize;  // max allocated
        int m_nGrowBy;   // grow amount

public:
        ~CDWordArray();

//        void Serialize(CArchive&);
#ifdef _DEBUG
//        void Dump(CDumpContext&) const;
//        void AssertValid() const;
#endif

protected:
        // local typedefs for class templates
        typedef ULONG BASE_TYPE;
        typedef ULONG BASE_ARG_TYPE;
};


