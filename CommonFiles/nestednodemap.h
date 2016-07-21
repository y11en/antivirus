#ifndef _NESTEDNODEMAP_H_
#define _NESTEDNODEMAP_H_

#include <functional>
#include <utility>
#include <iterator>

namespace NestedNodeMapNS {

#if !defined (_MSC_VER)
  using std::size_t;
  using std::ptrdiff_t;
#endif

  template <typename Key, typename T, typename Compare>
  class NestedNodeMap;
  template <typename Key, typename T, typename Ref, typename Ptr>
  class NestedNodeMapIterator;

#define NESTED_NODE_MAP_HEAD (1<<7)
#define NESTED_NODE_MAP_BLACK (1<<0)
#define NESTED_NODE_MAP_RED (1<<1)

  template <typename Key, typename T>
  class NestedNodeMapNode_template_type {
    //    friend class NestedNodeMap<Key,T>;
    friend class NestedNodeMapIterator<Key, T, T&, T*>;
    friend class NestedNodeMapIterator<Key, T, const T&, const T*>;
    typedef NestedNodeMapNode_template_type Self;

  public: 
    NestedNodeMapNode_template_type () 
      : theLeftNode ( 0 ), 
        theRightNode ( 0 ),
        thePrevNode ( 0 ),
        theFlags ( 0 )
    {}

    NestedNodeMapNode_template_type ( const Self& aValue )
      : theLeftNode ( 0 ), 
        theRightNode ( 0 ),
        thePrevNode ( 0 ),
        theFlags ( 0 )
    {}
    
    ~NestedNodeMapNode_template_type () 
    {
      if ( !theLeftNode && !theRightNode && !thePrevNode )
        return;
      if ( theFlags & NESTED_NODE_MAP_HEAD ) {
        theLeftNode = theRightNode = thePrevNode = this;
        return;
      }
      Self* aReplacement = 0;
      if ( !( theLeftNode->theFlags & NESTED_NODE_MAP_HEAD ) ) {
        aReplacement = theLeftNode;
        theLeftNode->thePrevNode = thePrevNode;
        Self* aNode = theLeftNode;
        while ( ! ( aNode->theRightNode->theFlags & NESTED_NODE_MAP_HEAD ) )
          aNode = aNode->theRightNode;
        aNode->theRightNode = theRightNode;
        aNode->theRightNode->thePrevNode = aNode;
      } 
      else if ( ! ( theRightNode->theFlags & NESTED_NODE_MAP_HEAD ) ) {
        theRightNode->thePrevNode = thePrevNode;
        aReplacement = theRightNode;
      }
      else
        aReplacement = theRightNode;

      if ( thePrevNode->theLeftNode == this )
        thePrevNode->theLeftNode = aReplacement;
      else if ( thePrevNode->theRightNode == this )
        thePrevNode->theRightNode = aReplacement;  
      theLeftNode = theRightNode = thePrevNode = 0;
    }

    //  private: 
    Key           theKey;
    Self*         theLeftNode;
    Self*         theRightNode;
    Self*         thePrevNode;
    unsigned char theFlags;

  private:
  };


#define NESTED_NODE_MAP_ENTRY(key,type) \
NestedNodeMapNS::NestedNodeMapNode_template_type<key,type> theNestedNodeMapNode_implicit_value; \
friend class NestedNodeMapNS::NestedNodeMap<key,type>;\
friend class NestedNodeMapNS::NestedNodeMapIterator<key, type, type&, type*>;\
friend class NestedNodeMapNS::NestedNodeMapIterator<key, type, const type&, const type*>;

#define NESTED_NODE_MAP_ENTRY_WITH_COMPARE(key,type,compare) \
NestedNodeMapNS::NestedNodeMapNode_template_type<key,type> theNestedNodeMapNode_implicit_value; \
friend class NestedNodeMapNS::NestedNodeMap<key,type,compare>;\
friend class NestedNodeMapNS::NestedNodeMapIterator<key, type, type&, type*>;\
friend class NestedNodeMapNS::NestedNodeMapIterator<key, type, const type&, const type*>;
  
  template <typename Key, typename T, typename Ref, typename Ptr>
  class NestedNodeMapIterator  {
  public:
    typedef NestedNodeMapIterator<Key, T, Ref, Ptr>           Self;
    typedef NestedNodeMapNode_template_type<Key, T>           NestedNode;
    typedef size_t                                            size_type;
    typedef ptrdiff_t                                         difference_type;
    typedef std::bidirectional_iterator_tag                   iterator_category;
    typedef Key                                               key_value;
    typedef T                                                 value_type;
    typedef Ptr                                               pointer;
    typedef Ref                                               reference;
    typedef NestedNodeMapIterator<Key, T, T&, T*>             iterator;
    typedef NestedNodeMapIterator<Key, T, const T&, const T*> const_iterator;
    
    NestedNodeMapIterator ( NestedNode* aNestedNode )
      : theNestedNode ( aNestedNode )
    {}

    NestedNodeMapIterator ( const NestedNode* aNestedNode )
      : theNestedNode ( const_cast <NestedNode*> (  aNestedNode ) )
    {}  

    NestedNodeMapIterator ()
      : theNestedNode ( 0 )
    {}
  
    NestedNodeMapIterator ( Self& anIterator )
      : theNestedNode ( anIterator.theNestedNode )
    {}

    template <typename I>
    NestedNodeMapIterator ( const I& anIterator )
      : theNestedNode ( &anIterator->theNestedNodeMapNode_implicit_value )
    {}

    reference operator* () const
    { 
      
      return *reinterpret_cast<T*>( reinterpret_cast<unsigned char*> ( theNestedNode ) - 
                                    offsetof(T,theNestedNodeMapNode_implicit_value) ); 
    }
  
    pointer operator-> () const
    { 
      return reinterpret_cast<T*>( reinterpret_cast<unsigned char*> ( theNestedNode ) - 
                                   offsetof(T,theNestedNodeMapNode_implicit_value) ); 
    }

    Self& operator++ ()
    {
      step_forward ();
      return *this;
    }
  
    Self operator++ ( int )
    {
      Self aTmp = *this;
      step_forward ();
      return aTmp;
    }
  
    Self& operator-- ()
    {
      step_backward ();
      return *this;
    }

    Self operator-- ( int )
    {
      Self aTmp = *this;
      step_backward ();
      return aTmp;
    }

    const Key key () const
    {
      return theNestedNode->theKey;
    }

    bool operator== ( const Self& anIterator ) const
    { 
      return theNestedNode == anIterator.theNestedNode;
    }
  
    bool operator!= ( const Self& anIterator ) const
    { 
      return theNestedNode != anIterator.theNestedNode;
    }

    void step_forward () 
    {
      if ( theNestedNode->theRightNode->theRightNode != theNestedNode->theRightNode->thePrevNode ) {
        theNestedNode = theNestedNode->theRightNode;
        while ( theNestedNode->theLeftNode->theRightNode != theNestedNode->theLeftNode->thePrevNode )
          theNestedNode = theNestedNode->theLeftNode;
      }
      else {
        NestedNode* aPrevNode = theNestedNode->thePrevNode;
        while ( theNestedNode == aPrevNode->theRightNode ) {
          theNestedNode = aPrevNode;
          aPrevNode = aPrevNode->thePrevNode;
        }
        if ( theNestedNode->theRightNode != aPrevNode )
          theNestedNode = aPrevNode;
      }
    }

    void step_backward () 
    {
      if ( theNestedNode->theLeftNode->theRightNode != theNestedNode->theLeftNode->thePrevNode ) {
        theNestedNode = theNestedNode->theLeftNode;
        while ( theNestedNode->theRightNode->theRightNode != theNestedNode->theRightNode->thePrevNode )
          theNestedNode = theNestedNode->theRightNode;
      }
      else {
        NestedNode* aPrevNode = theNestedNode->thePrevNode;
        while ( theNestedNode == aPrevNode->theLeftNode ) {
          theNestedNode = aPrevNode;
          aPrevNode = aPrevNode->thePrevNode;
        }
        if ( theNestedNode->theLeftNode != aPrevNode )
          theNestedNode = aPrevNode;
      }
    }

    NestedNode* theNestedNode;
  };


  template <typename Key, typename T, typename Compare = std::less<Key> >
  class NestedNodeMap {
    typedef NestedNodeMap<Key, T>                             Self;
    typedef NestedNodeMapNode_template_type<Key, T>           NestedNode;

  public:
    typedef Key                                               key_type;
    typedef T                                                 mapped_type;
    typedef std::pair<const Key, T&>                          value_type;
    typedef Compare                                           key_compare;
    typedef value_type*                                       pointer;
    typedef const value_type*                                 const_pointer;
    typedef value_type&                                       reference;
    typedef const value_type&                                 const_reference;
    typedef NestedNodeMapIterator<Key, T, T&, T*>             iterator;
    typedef NestedNodeMapIterator<Key, T, const T&, const T*> const_iterator;
//    typedef std::reverse_iterator<iterator>                   reverse_iterator;
//    typedef std::reverse_iterator<const_iterator>             const_reverse_iterator;
    typedef size_t                                            size_type;
    typedef ptrdiff_t                                         difference_type;

    class value_compare : public std::binary_function <value_type, value_type, bool>
    {
      friend class NestedNodeMap<Key,T,Compare>;
    protected:
      Compare theCompare;
      
      value_compare( Compare aCompare )
        : theCompare ( aCompare ) 
        {}
      
    public:
      bool operator () ( const value_type& aLeft, const value_type& aRight ) const
      { 
        return theCompare ( aLeft.first, aRight.first ); 
      }
    };

    NestedNodeMap ()
    { 
      theMapHead.theLeftNode = &theMapHead;
      theMapHead.theRightNode = &theMapHead;
      theMapHead.thePrevNode = &theMapHead;
      theMapHead.theFlags = NESTED_NODE_MAP_HEAD;
    }
    
    ~NestedNodeMap () 
    {
      clear ();
    }
    
    iterator begin ()
    {
      NestedNode* aNode = theMapHead.theLeftNode;
      while ( aNode->theLeftNode != &theMapHead )
        aNode = aNode->theLeftNode;
      return aNode;
    }
    
    const_iterator begin () const 
    {
      NestedNode* aNode = theMapHead->theLeftNode;
      while ( aNode->theLeftNode != &theMapHead )
        aNode = aNode->theLeftNode;
      return aNode;
    }
  
    iterator end ()
    {
      return &theMapHead;
    }
  
    const_iterator end () const 
    {
      return &theMapHead;
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
      return theMapHead.thePrevNode == &theMapHead;
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
    
    void erase ( const key_type& aValue )
    {
      iterator anIterator = find ( aValue );
      if ( anIterator == end () )  
        return;  
      erase ( anIterator );
    }
  
    void erase ( iterator aPosition )
    {
      NestedNode* aNode = aPosition.theNestedNode;
      aNode->NestedNode::~NestedNode ();
    }
  
    void swap ( NestedNodeMap& aMap ) 
    {
      NestedNode* aNode = theMapHead->theLeftNode;
      theMapHead->theLeftNode = aMap.theMapHead.theLeftNode;
      if ( theMapHead->theLeftNode )
        theMapHead->theLeftNode->thePrevNode = &theMapHead;
      aMap.theMapHead.theLeftNode = aNode;
      if ( aMap.theMapHead.theLeftNode )
        aMap.theMapHead.theLeftNode->thePrevNode = &aMap.theMapHead;
    }

    void clear () 
    { 
      while ( !empty () )
        erase ( begin () );
    }
 
    key_compare key_comp () const
    {
      return theKeyCompare;
    }
    
    value_compare value_comp () const
    {
      return value_compare ( key_comp () );
    }
    
    iterator find ( const key_type& aKey )
    {
      return find_node ( aKey );
    }

    const_iterator find ( const key_type& aKey ) const
    {
      return find_node ( aKey );
    }

    std::pair<iterator,bool> insert ( const key_type& aKey , mapped_type& aValue )
    {
      NestedNode* aNode = theMapHead.theLeftNode;
      NestedNode* aPrevNode = &theMapHead;
      bool is_less = true;
      while ( aNode != &theMapHead ) {
        aPrevNode = aNode;
        if ( theKeyCompare ( aNode->theKey, aKey ) ) {
          is_less = false;
          aNode = aNode->theRightNode;
        }
        else if ( theKeyCompare ( aKey, aNode->theKey ) ) {
          is_less = true;
          aNode = aNode->theLeftNode;
        }
        else
          return std::make_pair ( aNode, false );
      }
      aValue.theNestedNodeMapNode_implicit_value.theKey = aKey;
      aValue.theNestedNodeMapNode_implicit_value.thePrevNode = aPrevNode;
      if ( is_less )
        aPrevNode->theLeftNode = &aValue.theNestedNodeMapNode_implicit_value;
      else
        aPrevNode->theRightNode = &aValue.theNestedNodeMapNode_implicit_value;
      aValue.theNestedNodeMapNode_implicit_value.theLeftNode = &theMapHead;
      aValue.theNestedNodeMapNode_implicit_value.theRightNode = &theMapHead;
      return std::make_pair ( &aValue.theNestedNodeMapNode_implicit_value, true );
    }

  private:
    NestedNode* find_node ( const key_type& aKey )
    {
      NestedNode* aNode = theMapHead.theLeftNode;
      while ( aNode != &theMapHead )
        if ( theKeyCompare ( aNode->theKey, aKey ) )
          aNode = aNode->theRightNode;
        else if ( theKeyCompare ( aKey, aNode->theKey ) )
          aNode = aNode->theLeftNode;          
        else
          return aNode;
      return &theMapHead;
    }

    NestedNodeMap ( const Self& aMap );
    NestedNodeMap& operator= ( const Self& aMap );
    
    NestedNode theMapHead;
    Compare    theKeyCompare;
  };
  

} // NestedNodeMapNS

#endif // _NESTEDNODEMAP_H_
