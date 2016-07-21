////////////////////////////////////////////////////////////////////
//
//  cpointer.h
//  Smart pointer templates
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __CPOINTER_H
#define __CPOINTER_H

//
// A pair of smart pointer template classes. Provides basic conversion
// operator to T*, as well as dereferencing (*), and 0-checking (!).
// These classes assume that they alone are responsible for deleting the
// object or array unless Relinquish() is called.
//

//
// class CPointerBase
// ~~~~~ ~~~~~~~~~~~~
template<class T> class CPointerBase {
  public:
    T&   operator  *() {return *P;}
         operator T*() {return P;}
         operator const T*() const {return P;}
    int  operator  !() const {return P==0;}
    T*   Relinquish() {T* p = P; P = 0; return p;}

  protected:
    CPointerBase(T* pointer) : P(pointer) {}
    CPointerBase() : P(0) {}

    T* P;

  private:
    void* operator new(size_t) {return 0;}  // prohibit use of new
	protected:
    void operator delete(void* p) {((CPointerBase<T>*)p)->P=0;}
};

//
// class CPointer
// ~~~~~ ~~~~~~~~
// Pointer to a single object. Provides member access operator ->
//
template<class T> class CPointer : public CPointerBase<T> {
    //using CPointerBase<T>::P; this is a wrong using ADL (arqument dependent lookup)
  public:
    CPointer() : CPointerBase<T>() {}
    CPointer(T* pointer) : CPointerBase<T>(pointer) {}
   ~CPointer() {delete CPointerBase<T>::P;}
    CPointer<T>& operator =(T* src) {delete CPointerBase<T>::P; CPointerBase<T>::P = src; return *this;}
    T* operator->() {return CPointerBase<T>::P;}  // Could throw exception if P==0
    const T* operator->() const {return CPointerBase<T>::P;}  // Could throw exception if P==0
};

//
// class CAPointer
// ~~~~~ ~~~~~~~~~
// Pointer to an array of type T. Provides an array subscript operator and uses
// array delete[]
//
template<class T> class CAPointer : public CPointerBase<T> {
    //using CPointerBase<T>::P; this is a wrong using ADL (arqument dependent lookup)
  public:
    CAPointer() : CPointerBase<T>() {}
    CAPointer(T array[]) : CPointerBase<T>(array) {}
   ~CAPointer() {delete[] CPointerBase<T>::P;}
    CAPointer<T>& operator =(T src[]) {delete[] CPointerBase<T>::P; CPointerBase<T>::P = src; return *this;}
    T& operator[](int i) {return CPointerBase<T>::P[i];}  // Could throw exception if P==0
};

//----------------------------------------------------------------------------

//
// Envelope-letter type of smart pointers. In this implementation the envelope
// acts as a smart pointer to a reference-counted internal letter. This allows
// a lot of flexibility & safety in working with a single object (associated
// with the letter) being assigned to numerous envelopes in many scopes. An 'A'
// version is provided for use with arrays.
//
// Use like:
//   CEnvelope<T> e1 = new T(x,y,x);   // e1 now owns a T in a letter
//   e1->Func();                       // invokes Func() on the new T
//   Func(*e1);                        // passing a T& or a T this way
//   CEnvelope<T> e2 = e1;             // e2 safely shares the letter with e1
//
//   CAEnvelope<T> e1 = new T[99];     // e1 now owns T[] in a letter
//   e1[i].Func();                     // invokes Func() on a T element
//   Func(e1[i]);                      // passing a T& or a T this way
//   CAEnvelope<T> e2 = e1;            // e2 safely shares the letter with e1
//

//
// class CEnvelope
// ~~~~~ ~~~~~~~~~
template<class T> class CEnvelope {
  public:
    CEnvelope(T* object) : Letter(new CLetter(object)) {}
    CEnvelope(const CEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}
   ~CEnvelope() {Letter->Release();}

    CEnvelope& operator =(const CEnvelope& src);
    CEnvelope& operator =(T* object);

    T* operator->() { return Letter->Object; }
    const T* operator->() const { return Letter->Object; }
    T& operator *() { return *Letter->Object; }

  private:
    struct CLetter {
      CLetter(T* object) : Object(object), RefCount(1) {}
     ~CLetter() { delete Object; }

      void AddRef() { RefCount++; }
      void Release() { if (--RefCount == 0) delete this; }

      T*  Object;
      int RefCount;
    };
    CLetter* Letter;
};

template<class T>
CEnvelope<T>& CEnvelope<T>::operator =(const CEnvelope<T>& src) {
  Letter->Release();
  Letter = src.Letter;
  Letter->AddRef();
  return *this;
}

template<class T>
CEnvelope<T>& CEnvelope<T>::operator =(T* object) {
  Letter->Release();
  Letter = new CLetter(object);  // Assumes non-null! Use with new
  return *this;
}

//
// class CAEnvelope
// ~~~~~ ~~~~~~~~~~
template<class T> class CAEnvelope {
  public:
    CAEnvelope(T array[]) : Letter(new CLetter(array)) {}
    CAEnvelope(const CAEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}
   ~CAEnvelope() { Letter->Release(); }

    CAEnvelope& operator =(const CAEnvelope& src);
    CAEnvelope& operator =(T array[]);

    T& operator[](int i) { return Letter->Array[i]; }
    T* operator *() { return Letter->Array; }

  private:
    struct CLetter {
      CLetter(T array[]) : Array(array), RefCount(1) {}
     ~CLetter() { delete[] Array; }

      void AddRef() { RefCount++; }
      void Release() { if (--RefCount == 0) delete this; }

      T*  Array;
      int RefCount;
    };
    CLetter* Letter;
};

template<class T>
CAEnvelope<T>& CAEnvelope<T>::operator =(const CAEnvelope<T>& src) {
  Letter->Release();
  Letter = src.Letter;
  Letter->AddRef();
  return *this;
}

template<class T>
CAEnvelope<T>& CAEnvelope<T>::operator =(T array[]) {
  Letter->Release();
  Letter = new CLetter(array);  // Assumes non-null! Use with new
  return *this;
}

//
// class CSingleton
// ~~~~~ ~~~~~~~~~~
template<class T> class CSingleton {
public:
		CSingleton() {}
		static T* Instance( bool bAddRef = false );
		static void Release( bool bDirect = false );
private:
		static CPointer<T>	Insider;
		static int          RefCount;

    CSingleton(const CSingleton&);
    CSingleton& operator=(const CSingleton&);
};

template<class T>
T* CSingleton<T>::Instance( bool bAddRef ) {
	if ( (T*)Insider == 0 )	
		Insider = new T();
	RefCount += bAddRef ? 1 : 0;
	return Insider;
}

template<class T>
void CSingleton<T>::Release( bool bDirect ) {
	if ( RefCount ) {
		RefCount--;
	}
	if ( bDirect || !RefCount ) {
		Insider = 0;
		RefCount = 0;
	}
}

//
// class CSingleton1
// ~~~~~ ~~~~~~~~~~
template<class T, class P1> class CSingleton1 {
public:
		CSingleton1() {}
		static T* Instance( P1 = 0, bool bAddRef = false  );
		static void Release( bool bDirect = false );
private:
		static CPointer<T>	Insider;
		static int          RefCount;
		
    CSingleton1(const CSingleton1&);
    CSingleton1& operator=(const CSingleton1&);
};

template<class T, class P1>
T* CSingleton1<T, P1>::Instance( P1 p1, bool bAddRef ) {
	if ( (T*)Insider == 0 )
		Insider = new T(p1);
	RefCount += bAddRef ? 1 : 0;
	return Insider;
}

template<class T, class P1>
void CSingleton1<T, P1>::Release( bool bDirect ) {
	if ( RefCount ) {
		RefCount--;
	}
	if ( bDirect || !RefCount ) {
		Insider = 0;
		RefCount = 0;
	}
}


//
// class CSingleton2
// ~~~~~ ~~~~~~~~~~
template<class T, class P1, class P2> class CSingleton2 {
public:
		CSingleton2() {}
		static T* Instance( P1 = 0, P2 = 0, bool bAddRef = false  );
		static void Release( bool bDirect = false );
private:
		static CPointer<T>	Insider;
		static int          RefCount;
		
    CSingleton2(const CSingleton2&);
    CSingleton2& operator=(const CSingleton2&);
};

template<class T, class P1, class P2>
T* CSingleton2<T, P1, P2>::Instance( P1 p1, P2 p2, bool bAddRef ) {
	if ( (T*)Insider == 0 )
		Insider = new T(p1, p2);
	RefCount += bAddRef ? 1 : 0;
	return Insider;
}

template<class T, class P1, class P2>
void CSingleton2<T, P1, P2>::Release( bool bDirect ) {
	if ( RefCount ) {
		RefCount--;
	}
	if ( bDirect || !RefCount ) {
		Insider = 0;
		RefCount = 0;
	}
}


#endif  // CPOINTER_H
