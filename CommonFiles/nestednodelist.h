#if !defined (__NESTEDNODELIST_H__)
#define __NESTEDNODELIST_H__

#include <cstddef>
#include <iterator>

namespace NestedNodeListNS {

#if !defined (_MSC_VER)
  using std::size_t;
  using std::ptrdiff_t;
#endif

  template <typename T>
  class NestedNodeList;
  template <typename T, typename Ref, typename Ptr>
  class NestedNodeListIterator;

  template <typename T>
  class NestedNodeListNode_template_type {
    friend class NestedNodeList<T>;
    friend class NestedNodeListIterator<T, T&, T*>;
    friend class NestedNodeListIterator<T, const T&, const T*>;
    typedef NestedNodeListNode_template_type Self;

  public: 
    NestedNodeListNode_template_type () 
      : theNextNode ( 0 ), 
        thePrevNode ( 0 )
    {}

    NestedNodeListNode_template_type ( const Self& aValue )
      : theNextNode ( 0 ), 
        thePrevNode ( 0 )
    {}
    
    ~NestedNodeListNode_template_type () 
    { 
      if ( theNextNode && thePrevNode ) {
        theNextNode->thePrevNode = thePrevNode;
        thePrevNode->theNextNode = theNextNode;
        theNextNode = thePrevNode = 0;
      }
    }
  private: 

    Self* theNextNode;
    Self* thePrevNode;
  };

#define NESTED_NODE_LIST_ENTRY(type) \
NestedNodeListNS::NestedNodeListNode_template_type<type> theNestedNodeListNode_implicit_value; \
friend class NestedNodeListNS::NestedNodeList<type>;\
friend class NestedNodeListNS::NestedNodeListIterator<type, type&, type*>;\
friend class NestedNodeListNS::NestedNodeListIterator<type, const type&, const type*>;

  template <typename T, typename Ref, typename Ptr>
  class NestedNodeListIterator  {
  public:
    typedef NestedNodeListIterator<T, Ref, Ptr>           Self;
    typedef NestedNodeListNode_template_type<T>           NestedNode;
    typedef size_t                                        size_type;
    typedef ptrdiff_t                                     difference_type;
    typedef std::bidirectional_iterator_tag               iterator_category;
    typedef T                                             value_type;
    typedef Ptr                                           pointer;
    typedef Ref                                           reference;
    typedef NestedNodeListIterator<T, T&, T*>             iterator;
    typedef NestedNodeListIterator<T, const T&, const T*> const_iterator;

    NestedNodeListIterator ()
      : theNestedNode ( 0 )
    {}
    
    NestedNodeListIterator ( NestedNode* aNestedNode )
      : theNestedNode ( aNestedNode )
    {}

    NestedNodeListIterator ( const NestedNode* aNestedNode )
      : theNestedNode ( const_cast<NestedNode*> ( aNestedNode ) )
    {}

#if defined (_MSC_VER)
    NestedNodeListIterator ( Self& anIterator )
      : theNestedNode ( anIterator.theNestedNode )
    {}
#else  
    NestedNodeListIterator ( const Self& anIterator )
      : theNestedNode ( const_cast<Self&> ( anIterator ).theNestedNode )
    {}
#endif

    template <typename I>
    NestedNodeListIterator ( const I& anIterator )
      : theNestedNode ( const_cast<NestedNode*> ( &anIterator->theNestedNodeListNode_implicit_value ) )
    {}

    reference operator* () const
    { 
      return *reinterpret_cast<T*>( reinterpret_cast<unsigned char*> ( theNestedNode ) - 
                                    offsetof(T,theNestedNodeListNode_implicit_value) ); 
    }
  
    pointer operator-> () const
    { 
      return reinterpret_cast<T*>( reinterpret_cast<unsigned char*> ( theNestedNode ) - 
                                   offsetof(T,theNestedNodeListNode_implicit_value) ); 
    }
  
    Self& operator++ ()
    {
      theNestedNode = theNestedNode->theNextNode;
      return *this;
    }
  
    Self operator++ ( int )
    {
      Self aTmp = *this;
      theNestedNode = theNestedNode->theNextNode;
      return aTmp;
    }
  
    Self& operator-- ()
    {
      theNestedNode = theNestedNode->thePrevNode;
      return *this;
    }

    Self operator-- ( int )
    {
      Self aTmp = *this;
      theNestedNode = theNestedNode->thePrevNode;
      return aTmp;
    }

    bool operator== ( const Self& anIterator ) const
    { 
      return theNestedNode == anIterator.theNestedNode;
    }
  
    bool operator!= ( const Self& anIterator ) const
    { 
      return theNestedNode != anIterator.theNestedNode;
    }
  
    NestedNode* theNestedNode;
  };


  template <typename T>
  class NestedNodeList {
    typedef NestedNodeList<T>                             Self;
    typedef NestedNodeListNode_template_type<T>           NestedNode;

  public:
    typedef T                                         value_type;
    typedef value_type*                               pointer;
    typedef const value_type*                         const_pointer;
    typedef value_type&                               reference;
    typedef const value_type&                         const_reference;
    typedef NestedNodeListIterator<T, T&, T*>         iterator;
    typedef NestedNodeListIterator<T, const T&, const T*> const_iterator;
//    typedef std::reverse_iterator<iterator>           reverse_iterator;
//    typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;
    typedef size_t                                    size_type;
    typedef ptrdiff_t                                 difference_type;
  
    NestedNodeList ()
    {
      theListHead.theNextNode = &theListHead;
      theListHead.thePrevNode = &theListHead;
    }
    
    ~NestedNodeList () 
    {
      clear ();
    }
  
    iterator begin ()
    {
      return theListHead.theNextNode;
    }
  
    const_iterator begin () const 
    {
      return theListHead.theNextNode;
    }
  
    iterator end ()
    {
      return &theListHead;      
    }
  
    const_iterator end () const 
    {
      
      return &theListHead;      
    }
  
/*    reverse_iterator rbegin () 
    { 
      return reverse_iterator ( end () ); 
    }
  
    const_reverse_iterator rbegin () const 
    { 
      return const_reverse_iterator ( end () ); 
    }
  
    reverse_iterator rend () 
    { 
      return reverse_iterator ( begin () ); 
    }
  
    const_reverse_iterator rend () const
    { 
      return const_reverse_iterator ( begin () ); 
    }
*/  
    bool empty () const 
    { 
      return theListHead.theNextNode == &theListHead;
    }
  
    size_type size () const 
    { 
      size_type aSize = 0;
      for ( const_iterator anIter = begin(); anIter != end (); ++anIter )
        aSize++;
	  return aSize;
    }

    size_type max_size () const 
    { 
      return size_type ( -1 ); 
    }
  
    reference front () 
    { 
      return *begin (); 
    }
  
    const_reference front () const 
    { 
      return *begin (); 
    }
  
    reference back () 
    { 
      return *( --end () ); 
    }
  

    const_reference back () const 
    { 
      return *( --end () ); 
    }
  
    void push_front ( value_type& aValue ) 
    { 
      insert ( begin (), aValue ); 
    }
  
    void pop_front() 
    { 
      erase ( begin () ); 
    }
  
    void push_back ( value_type& aValue ) 
    { 
      insert ( end (), aValue );
    }
    
    void  pop_back()
    {
      iterator anIterator = end();
      erase ( --anIterator );
    }

    iterator insert ( iterator aPosition, value_type& aValue )
    {
      if ( aPosition == &aValue.theNestedNodeListNode_implicit_value )
        return aPosition;
      if ( aValue.theNestedNodeListNode_implicit_value.theNextNode && aValue.theNestedNodeListNode_implicit_value.thePrevNode )
        remove ( aValue );
      aValue.theNestedNodeListNode_implicit_value.theNextNode = aPosition.theNestedNode;
      aValue.theNestedNodeListNode_implicit_value.thePrevNode = aPosition.theNestedNode->thePrevNode;
      aValue.theNestedNodeListNode_implicit_value.theNextNode->thePrevNode = &aValue.theNestedNodeListNode_implicit_value;
      aValue.theNestedNodeListNode_implicit_value.thePrevNode->theNextNode = &aValue.theNestedNodeListNode_implicit_value;
      return &aValue.theNestedNodeListNode_implicit_value;
    }
  
    iterator erase ( iterator aPosition )
    {
      if ( empty () || ( aPosition == end () ) )
        return end ();
      NestedNode* aTmp = aPosition.theNestedNode->theNextNode;
      aPosition.theNestedNode->theNextNode->thePrevNode = aPosition.theNestedNode->thePrevNode;
      aPosition.theNestedNode->thePrevNode->theNextNode = aPosition.theNestedNode->theNextNode;
      aPosition.theNestedNode->thePrevNode = aPosition.theNestedNode->theNextNode = 0;
      return aTmp;
    }
  
    void swap ( NestedNodeList& aList ) 
    { 
      bool isEmpty = empty ();
      NestedNode* aTmpNextNode = theListHead.theNextNode;
      NestedNode* aTmpPrevNode = theListHead.thePrevNode;
      if ( aList.empty () ) {
        theListHead.theNextNode = &theListHead;
        theListHead.thePrevNode = &theListHead;
      }
      else {
        theListHead.theNextNode = aList.theListHead.theNextNode;
        theListHead.thePrevNode = aList.theListHead.thePrevNode;
        theListHead.theNextNode->thePrevNode = &theListHead;
        theListHead.thePrevNode->theNextNode = &theListHead;
      }
      if ( isEmpty ) {
        aList.theListHead.theNextNode = &aList.theListHead;
        aList.theListHead.thePrevNode = &aList.theListHead;
      }
      else {
        aList.theListHead.theNextNode = aTmpNextNode;
        aList.theListHead.thePrevNode = aTmpPrevNode;
        aList.theListHead.theNextNode->thePrevNode = &aList.theListHead;
        aList.theListHead.thePrevNode->theNextNode = &aList.theListHead;
      }
    }
  
    void clear () 
    { 
      while ( !empty () )
        erase ( iterator ( theListHead.theNextNode ) );
    }
  
    void remove ( reference aValue )
    {
      if ( !aValue.theNestedNodeListNode_implicit_value.theNextNode || !aValue.theNestedNodeListNode_implicit_value.thePrevNode )
        return;
      aValue.theNestedNodeListNode_implicit_value.theNextNode->thePrevNode = 
        aValue.theNestedNodeListNode_implicit_value.thePrevNode;
      aValue.theNestedNodeListNode_implicit_value.thePrevNode->theNextNode = 
        aValue.theNestedNodeListNode_implicit_value.theNextNode;
      aValue.theNestedNodeListNode_implicit_value.thePrevNode = aValue.theNestedNodeListNode_implicit_value.theNextNode = 0;
    }
  
  private:
    NestedNodeList ( const Self& aList );
    NestedNodeList& operator= ( const Self& aList );

    NestedNode theListHead;
  };
  
} // NestedNodeListNS


#endif
