/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: array.h
  Created: 2006/09/08
  Desc: Array implementation (by some reason std::vector is disallowed,
        thats why this classes was developed).
        
          * Class Container is collection of objects of type T.
            It is designed for objects of C++ classes.
            It is owner of contained objects and call constructor and destructor of them.
            Container<T> does not need T::T(const T&) and T::operator=(constT&).

          * Class MiniContainer is designed for integral types, pointers,
            and small POD-type objects which do not need constructor/destructor call.
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
//--------------------------------------------------------------------------------------------------------------------//
#ifndef __array_h__
#define __array_h__
//--------------------------------------------------------------------------------------------------------------------//
#include <stdlib.h>
#include <string.h>
//--------------------------------------------------------------------------------------------------------------------//
#ifdef _MSC_VER
# pragma warning( disable : 4514 4505 4710) 
#endif // _MSC_VER
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
class Container
{
public:

    Container (const unsigned int grow_size = 100);
    virtual ~Container ();

    T* Add();
    bool Remove (const unsigned int i, const unsigned int count = 1);
    inline bool Empty() {return Remove(0, used);}
    inline unsigned int Size() const {return used;};

    inline T* GetLast() {return (0 == used) ? 0 : data [used-1];}
    inline T* GetFirst() {return (0 == used) ? 0 : data [0];}
    inline T* Get(const unsigned int i) {return (i >= used) ? 0 : data [i];}
    T& operator[] (const unsigned int i) {return *data[i];}

    const inline T* GetLast() const {return (0 == used) ? 0 : data [used-1];}
    const inline T* GetFirst() const {return (0 == used) ? 0 : data [0];}
    const inline T* Get(const unsigned int i) const {return (i >= used) ? 0 : data [i];}
    const T& operator[] (const unsigned int i) const {return *data[i];}

    bool AllocMore (const unsigned int count);
    void Qsort (int (*compar)(const void *, const void *));

private:
    Container( const T& );       // T **data and T *data[i] are not a shared pointers
    T& operator=( const T& );

    unsigned int size;
    unsigned int used;
    unsigned int grow_size;
    T **data; // array of pointers to objects
};
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
Container<T>::Container(const unsigned int grow_size_arg)
{
    size = used = 0;
    grow_size = grow_size_arg;
    data = 0;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
Container<T>::~Container()
{
    if (data)
    {
        for (size_t i = 0; i < used; i ++)
            delete data[i];
        free (data);
    }
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
T* Container<T>::Add ()
{
    if ( used >= size )
    {
        T** new_data = (T**)realloc (data, (size + grow_size)*sizeof (T*));
        if (0 == new_data)
            return NULL;
        data = new_data;
        size += grow_size;
    }
    data [used] = new T;
    used ++;
    return data [used-1];
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool Container<T>::Remove (const unsigned int i, const unsigned int count)
{
    if ( 0 == count )
        return true;
    if (i + count > used || 0 == used)
        return false;

    unsigned int j=0;
    for ( j=0; j < count; j ++)
        delete data [i+j];

    if (i + count < used)
        memmove(data+i, data+i+count, (used - (i + count)) * sizeof (T*));
    used -= count;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool Container<T>::AllocMore (const unsigned int new_size)
{
    if (new_size <= size)
        return true;

    T** new_data = (T**)realloc (data, new_size*sizeof (T*));
    if (0 == new_data)
        return false;
    data = new_data;
    size = new_size;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
// compar get T** but not T*
template <typename T>
void Container<T>::Qsort(int (*compar)(const void *, const void *))
{
    qsort((void*) data, used, sizeof(T*), compar);
}
//--------------------------------------------------------------------------------------------------------------------//
//####################################################################################################################//
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
class MiniContainer
{
public:

    MiniContainer (const unsigned int grow_size = 100);
    virtual ~MiniContainer ();

    T* Add();
    bool Add (const T& t);
    inline bool Push (const T& t) {return Add(t);};
    bool Remove (const unsigned int i, const unsigned int count = 1);
    inline bool Empty() {return Remove(0, used);}
    inline bool Pop (T& t);
    inline unsigned int Size() const {return used;};

    inline T* GetLast() {return (0 == used) ? 0 : &data [used-1];}
    inline T* GetFirst() {return (0 == used) ? 0 : &data [0];}
    inline T* Get(const unsigned int i) {return (i >= used) ? 0 : &data [i];}
    T& operator[] (const unsigned int i) {return data[i];}

    const inline T* GetLast() const {return (0 == used) ? 0 : &data [used-1];}
    const inline T* GetFirst() const {return (0 == used) ? 0 : &data [0];}
    const inline T* Get(const unsigned int i) const {return (i >= used) ? 0 : &data [i];}
    const T& operator[] (const unsigned int i) const {return data[i];}

    bool AllocMore (const unsigned int new_size);
    void Qsort (int (*compar)(const void *, const void *));
    void QsortByLess();
    static int compare_by_less (const void *p1, const void *p2);

    bool Find (const T& t /*IN*/, unsigned int& pos /*OUT*/) const;

private:
    MiniContainer( const T& );   // data is not a shared pointer
    T& operator=( const T& );

    unsigned int size;
    unsigned int used;
    unsigned int grow_size;
    T *data; // array of objects
};
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
MiniContainer<T>::MiniContainer(const unsigned int grow_size_arg)
{
    size = used = 0;
    grow_size = grow_size_arg;
    data = 0;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
MiniContainer<T>::~MiniContainer()
{
    if (data)
        free (data);
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool MiniContainer<T>::Add (const T& t)
{
    T* new_obj = Add();
    if (0 == new_obj)
        return false;
    *new_obj = t;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
T* MiniContainer<T>::Add ()
{
    if ( used >= size )
    {
        T* new_data = (T*)realloc (data, (size + grow_size)*sizeof (T));
        if (0 == new_data)
            return NULL;
        data = new_data;
        size += grow_size;
    }
    used ++;
    return &data [used-1];
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool MiniContainer<T>::Remove (const unsigned int i, const unsigned int count)
{
    if ( 0 == count )
        return true;
    if (i + count > used || 0 == used)
        return false;

    if (i + count < used)
        memmove(data+i, data+i+count, (used - (i + count)) * sizeof (T));
    used -= count;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool MiniContainer<T>::Pop (T& t)
{
    if (0 == used)
        return false;
    t = data[used-1];
    used --;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool MiniContainer<T>::AllocMore (const unsigned int new_size)
{
    if (new_size <= size)
        return true;

    T* new_data = (T*)realloc (data, new_size*sizeof (T*));
    if (0 == new_data)
        return false;
    data = new_data;
    size = new_size;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------//
// compar get T*
template <typename T>
void MiniContainer<T>::Qsort(int (*compar)(const void *, const void *))
{
    qsort((void*) data, used, sizeof(T), compar);
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
int MiniContainer<T>::compare_by_less (const void *p1, const void *p2)
{
    if ( (*(T*)p1) < (*(T*)p2) )
        return -1;
    if ( (*(T*)p2) < (*(T*)p1) )
        return 1;
    return 0;
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
void MiniContainer<T>::QsortByLess()
{
    qsort((void*) data, used, sizeof(T), compare_by_less);
}
//--------------------------------------------------------------------------------------------------------------------//
template <typename T>
bool MiniContainer<T>::Find (const T& t /*IN*/, unsigned int& pos /*OUT*/) const
{
    unsigned int i = 0;
    for ( ; i < used; i ++)
        if ( 0 == memcmp ( &data[i], &t, sizeof (T)) )
        {
            pos = i;
            return true;
        }
    return false;
}
//--------------------------------------------------------------------------------------------------------------------//
#endif // __array_h__
//--------------------------------------------------------------------------------------------------------------------//
