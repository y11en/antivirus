#include "typedef.h"
#include <string.h>
#include "dword.h"

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

CDWordArray::CDWordArray()
 {
  m_pData = NULL;
  m_nSize = m_nMaxSize = m_nGrowBy = 0;
 }

CDWordArray::~CDWordArray()
 {
 //ASSERT_VALID(this);
   delete[] (BYTE*)m_pData;
 }

void CDWordArray::SetSize(int nNewSize, int nGrowBy)
{
//        ASSERT_VALID(this);
//        ASSERT(nNewSize >= 0);

        if (nGrowBy != -1)
                m_nGrowBy = nGrowBy;  // set new size

        if (nNewSize == 0)
        {
                // shrink to nothing
                delete[] (BYTE*)m_pData;
                m_pData = NULL;
                m_nSize = m_nMaxSize = 0;
        }
        else if (m_pData == NULL)
        {
                // create one with exact size
#ifdef SIZE_T_MAX
                ASSERT(nNewSize <= SIZE_T_MAX/sizeof(ULONG));    // no overflow
#endif
                m_pData = (ULONG*) new BYTE[nNewSize * sizeof(ULONG)];

                memset(m_pData, 0, nNewSize * sizeof(ULONG));  // zero fill

                m_nSize = m_nMaxSize = nNewSize;
        }
        else if (nNewSize <= m_nMaxSize)
        {
                // it fits
                if (nNewSize > m_nSize)
                {
                        // initialize the new elements

                        memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(ULONG));

                }

                m_nSize = nNewSize;
        }
        else
        {
                // otherwise, grow array
                int nGrowBy = m_nGrowBy;
                if (nGrowBy == 0)
                {
                        // heuristically determine growth when nGrowBy == 0
                        //  (this avoids heap fragmentation in many situations)
                        nGrowBy = min(1024, max(4, m_nSize / 8));
                }
                int nNewMax;
                if (nNewSize < m_nMaxSize + nGrowBy)
                        nNewMax = m_nMaxSize + nGrowBy;  // granularity
                else
                        nNewMax = nNewSize;  // no slush

//                ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
                ASSERT(nNewMax <= SIZE_T_MAX/sizeof(ULONG)); // no overflow
#endif
                ULONG* pNewData = (ULONG*) new BYTE[nNewMax * sizeof(ULONG)];

                // copy new data from old
                memcpy(pNewData, m_pData, m_nSize * sizeof(ULONG));

                // construct remaining elements
//                ASSERT(nNewSize > m_nSize);

                memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(ULONG));


                // get rid of old stuff (note: no destructors called)
                delete[] (BYTE*)m_pData;
                m_pData = pNewData;
                m_nSize = nNewSize;
                m_nMaxSize = nNewMax;
        }
}

int CDWordArray::Append(const CDWordArray& src)
{
//        ASSERT_VALID(this);
//        ASSERT(this != &src);   // cannot append to itself

        int nOldSize = m_nSize;
        SetSize(m_nSize + src.m_nSize);

        memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(ULONG));

        return nOldSize;
}

void CDWordArray::Copy(const CDWordArray& src)
{
//        ASSERT_VALID(this);
//        ASSERT(this != &src);   // cannot append to itself

        SetSize(src.m_nSize);

        memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(ULONG));

}

void CDWordArray::FreeExtra()
{
//        ASSERT_VALID(this);

        if (m_nSize != m_nMaxSize)
        {
                // shrink to desired size
#ifdef SIZE_T_MAX
                ASSERT(m_nSize <= SIZE_T_MAX/sizeof(ULONG)); // no overflow
#endif
                ULONG* pNewData = NULL;
                if (m_nSize != 0)
                {
                        pNewData = (ULONG*) new BYTE[m_nSize * sizeof(ULONG)];
                        // copy new data from old
                        memcpy(pNewData, m_pData, m_nSize * sizeof(ULONG));
                }

                // get rid of old stuff (note: no destructors called)
                delete[] (BYTE*)m_pData;
                m_pData = pNewData;
                m_nMaxSize = m_nSize;
        }
}

/////////////////////////////////////////////////////////////////////////////

void CDWordArray::SetAtGrow(int nIndex, ULONG newElement)
{
//        ASSERT_VALID(this);
//        ASSERT(nIndex >= 0);

        if (nIndex >= m_nSize)
                SetSize(nIndex+1);
        m_pData[nIndex] = newElement;
}

void CDWordArray::InsertAt(int nIndex, ULONG newElement, int nCount)
{
//        ASSERT_VALID(this);
//        ASSERT(nIndex >= 0);    // will expand to meet need
//        ASSERT(nCount > 0);     // zero or negative size not allowed

        if (nIndex >= m_nSize)
        {
                // adding after the end of the array
                SetSize(nIndex + nCount);  // grow so nIndex is valid
        }
        else
        {
                // inserting in the middle of the array
                int nOldSize = m_nSize;
                SetSize(m_nSize + nCount);  // grow it to new size
                // shift old data up to fill gap
                memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
                        (nOldSize-nIndex) * sizeof(ULONG));

                // re-init slots we copied from

                memset(&m_pData[nIndex], 0, nCount * sizeof(ULONG));

        }

        // insert new value in the gap
//        ASSERT(nIndex + nCount <= m_nSize);
        while (nCount--)
                m_pData[nIndex++] = newElement;
}

void CDWordArray::RemoveAt(int nIndex, int nCount)
{
//        ASSERT_VALID(this);
//        ASSERT(nIndex >= 0);
//        ASSERT(nCount >= 0);
//        ASSERT(nIndex + nCount <= m_nSize);

        // just remove a range
        int nMoveCount = m_nSize - (nIndex + nCount);

        if (nMoveCount)
                memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount],
                        nMoveCount * sizeof(ULONG));
        m_nSize -= nCount;
}

void CDWordArray::InsertAt(int nStartIndex, CDWordArray* pNewArray)
{
//        ASSERT_VALID(this);
//        ASSERT(pNewArray != NULL);
//        ASSERT_KINDOF(CDWordArray, pNewArray);
//        ASSERT_VALID(pNewArray);
//        ASSERT(nStartIndex >= 0);

        if (pNewArray->GetSize() > 0)
        {
                InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
                for (int i = 0; i < pNewArray->GetSize(); i++)
                        SetAt(nStartIndex + i, pNewArray->GetAt(i));
        }
}

int   CDWordArray::GetSize() const
        { return m_nSize; }
int   CDWordArray::GetUpperBound() const
        { return m_nSize-1; }
void  CDWordArray::RemoveAll()
        { SetSize(0, -1); }
ULONG CDWordArray::GetAt(int nIndex) const
        {
//         ASSERT(nIndex >= 0 && nIndex < m_nSize);
         return m_pData[nIndex]; }
void  CDWordArray::SetAt(int nIndex, ULONG newElement)
        {
//         ASSERT(nIndex >= 0 && nIndex < m_nSize);
         m_pData[nIndex] = newElement; }
ULONG& CDWordArray::ElementAt(int nIndex)
        {
//         ASSERT(nIndex >= 0 && nIndex < m_nSize);
         return m_pData[nIndex]; }
const ULONG* CDWordArray::GetData() const
        { return (const ULONG*)m_pData; }
ULONG* CDWordArray::GetData()
        { return (ULONG*)m_pData; }
int CDWordArray::Add(ULONG newElement)
        { int nIndex = m_nSize;
          SetAtGrow(nIndex, newElement);
          return nIndex; }
ULONG CDWordArray::operator[](int nIndex) const
        { return GetAt(nIndex); }
ULONG& CDWordArray::operator[](int nIndex)
        { return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// Serialization
//void CDWordArray::Serialize(CArchive& ar)
//{
//        ASSERT_VALID(this);

//        CObject::Serialize(ar);

//        if (ar.IsStoring())
//        {
//                ar.WriteCount(m_nSize);
//#ifdef _MAC
//                if (!ar.IsByteSwapping())
//#endif
//                ar.Write(m_pData, m_nSize * sizeof(ULONG));
//#ifdef _MAC
//                else
//                {
//                        // write each item individually so that it will be byte-swapped
//                        for (int i = 0; i < m_nSize; i++)
//                                ar << m_pData[i];
//                }
//#endif
//        }
//        else
//        {
//                ULONG nOldSize = ar.ReadCount();
//                SetSize(nOldSize);
//                ar.Read(m_pData, m_nSize * sizeof(ULONG));
//#ifdef _MAC
//                if (ar.IsByteSwapping())
//                {
//                        for (int i = 0; i < m_nSize; i++)
//                                _AfxByteSwap(m_pData[i], (BYTE*)&m_pData[i]);
//                }
//#endif
//        }
//}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CDWordArray::Dump(CDumpContext& dc) const
{
        CObject::Dump(dc);

        dc << "with " << m_nSize << " elements";
        if (dc.GetDepth() > 0)
        {
                for (int i = 0; i < m_nSize; i++)
                        dc << "\n\t[" << i << "] = " << m_pData[i];
        }

        dc << "\n";
}

void CDWordArray::AssertValid() const
{
        CObject::AssertValid();

        if (m_pData == NULL)
        {
//                ASSERT(m_nSize == 0);
//                ASSERT(m_nMaxSize == 0);
        }
        else
        {
//                ASSERT(m_nSize >= 0);
//                ASSERT(m_nMaxSize >= 0);
//                ASSERT(m_nSize <= m_nMaxSize);
//                ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(ULONG)));
        }
}
#endif
/////////////////////////////////////////////////////////////////////////////
