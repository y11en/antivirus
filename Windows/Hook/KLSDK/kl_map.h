#ifndef _KL_MAP_H_
#define _KL_MAP_H_

template <class _T1, class _T2> struct CKl_Pair : public CKl_Object
{
    LIST_ENTRY                  m_ListEntry;        // связь в списки

    typedef _T1 first_type;
    typedef _T2 second_type;

    _T1 first;
    _T2 second;

    CKl_Pair() : first(_T1()), second(_T2()) {}
    CKl_Pair(const _T1& X, const _T2& Y) : first(X), second(Y) {}
};

template<class _T1, class _T2> inline
bool __cdecl operator == ( const CKl_Pair<_T1, _T2>& _X, const CKl_Pair<_T1, _T2>& _Y )
{
    return (_X.first == _Y.first && _X.second == _Y.second); 
}
template<class _T1, class _T2> inline
bool __cdecl operator != ( const CKl_Pair<_T1, _T2>& _X, const CKl_Pair<_T1, _T2>& _Y )
{
    return (!(_X == _Y)); 
}

template<class _T1, class _T2> inline
bool __cdecl operator < ( const CKl_Pair<_T1, _T2>& _X,  const CKl_Pair<_T1, _T2>& _Y )
{
    return ( _X.first < _Y.first ); 
}

template<class _T1, class _T2> inline
bool __cdecl operator > ( const CKl_Pair<_T1, _T2>& _X,  const CKl_Pair<_T1, _T2>& _Y )
{
    return ( _X.first > _Y.first ); 
}

template<class _T1, class _T2> inline
bool __cdecl operator <= ( const CKl_Pair<_T1, _T2>& _X, const CKl_Pair<_T1, _T2>& _Y )
{
    return !( _X.first > _Y.first ); 
}

template<class _T1, class _T2> inline
bool __cdecl operator >= ( const CKl_Pair<_T1, _T2>& _X, const CKl_Pair<_T1, _T2>& _Y )
{
    return !( _X.first < _Y.first );  
}

template<class _T1, class _T2> inline
CKl_Pair<_T1, _T2> __cdecl kl_make_pair(const _T1& _X, const _T2& _Y)
{
    return ( CKl_Pair<_T1, _T2>(_X, _Y) ); 
}

// Key - индекс, по которому выдается значение T
template <class Key, class T> class CKl_Map : public CKl_Object
{
    CKl_List< CKl_Pair<Key, T> >*    pPairList;  
public:
    void Lock()    {
        pPairList->Lock();
    }
    void Unlock()  {
        pPairList->Unlock();
    }

    CKl_Map()
    {
        pPairList = new CKl_List< CKl_Pair<Key, T> >();
    }
    ~CKl_Map()
    {
        delete pPairList;
    }

    void Add(Key key, T Item)
    {
        CKl_Pair<Key, T>* NewPair = new  CKl_Pair<Key, T>(key, Item);
        pPairList->InsertTail( NewPair );
    }

    void AddNoLock(Key key, T Item)
    {
        CKl_Pair<Key, T>* NewPair = new  CKl_Pair<Key, T>(key, Item);
        pPairList->InsertTailNoLock( NewPair );
    }
    
    T Get(Key key)
    {
        CKl_Pair<Key, T>* P = NULL;        
        while (P = pPairList->Next(P))
        {
            if (P->first == key)
            {                
                return P->second;
            }
        }        
        return 0;
    }

    T Remove(Key key)
    {
        T   Res = 0;
        CKl_Pair<Key, T>* P = NULL;        
        while (P = pPairList->Next(P))
        {
            if (P->first == key)
            {
                pPairList->Remove(P);
                Res = P->second;
                delete P;
                break;
            }
        }        
        return Res;
    }
    
    T InterlockedRemove(Key key)
    {
        T   Res = 0;
        CKl_Pair<Key, T>* P = NULL;  
        pPairList->Lock();
        while (P = pPairList->Next(P))
        {
            if (P->first == key)
            {
                pPairList->Remove(P);
                Res = P->second;
                delete P;
                break;
            }
        } 
        pPairList->Unlock();
        return Res;
    }
};

#endif // #endif