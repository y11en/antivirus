/*
 * pointerator - iterator to T* that behaves like iterator to T
 *
 * Example usage:
 *
 *     std::vector<int*> v;
 *     v.push_back(new int(42));
 *     v.push_back(new int(17));
 *
 *     typedef pointerator<std::vector<int*>::iterator> Iter;
 *     // Note: if your compiler does not support partial template
 *     // specialization, you should write:
 *     // typedef pointerator<std::vector<int*>::iterator, int> Iter;
 *
 *     Iter i = std::find(Iter(v.begin()), Iter(v.end()), 17);
 *     // finds the second element of v
 *
 *     std::cout << *i.get_iterator() << " points to " << *i;
 *     // *i is 17
 *
 * Note: For ANSI-challenged compilers, you may want to #define
 * NO_PARTIAL_SPECIALIZATION.
 *
 * Written 07-Feb-1999 by Yonat Sharon <yonat@ootips.org>
 */

#ifndef POINTERATOR_H
#define POINTERATOR_H

#ifndef NO_PARTIAL_SPECIALIZATION
#include <iterator> // for iterator_traits
#else
#include <cstddef> // for ptrdiff_t
#endif

#ifndef NO_PARTIAL_SPECIALIZATION
template <typename Iter>
#else
template <typename Iter, typename Val>
#endif // NO_PARTIAL_SPECIALIZATION
class pointerator
{
public:

#ifndef NO_PARTIAL_SPECIALIZATION
    typedef pointerator<Iter> its_type;
    template <typename T> struct dereference { typedef void type;};
    template <typename T> struct dereference<T*> {typedef T type;};
    typedef typename dereference<typename Iter::value_type>::type   Val;
    typedef typename std::iterator_traits<Iter>::iterator_category  iterator_category;
    typedef typename std::iterator_traits<Iter>::difference_type      difference_type;
#else
    typedef pointerator<Iter,Val> its_type;
    typedef ptrdiff_t difference_type;
#endif // NO_PARTIAL_SPECIALIZATION

    typedef Val         value_type;
    typedef Val&        reference;
    typedef const Val&  const_reference;
    typedef Val*        pointer;
    typedef const Val*  const_pointer;

    pointerator() {}
    pointerator(Iter i) : itsIter(i) {}
    Iter get_iterator() const {return itsIter;}

    reference operator*() const {return **itsIter;}
    pointer operator->() const {return *itsIter;}
    reference operator[](difference_type n) const {return **itsIter[n];}

    its_type& operator++()          {++itsIter; return *this;}
    its_type& operator--()          {--itsIter; return *this;}
    its_type operator++(int)        {its_type t(*this); ++itsIter; return t;}
    its_type operator--(int)        {its_type t(*this); --itsIter; return t;}
    its_type& operator+=(difference_type n)     {itsIter+=n; return *this;}
    its_type& operator-=(difference_type n)     {itsIter-=n; return *this;}
    its_type operator+(difference_type n) const {return its_type(itsIter+n);}
    its_type operator-(difference_type n) const {return its_type(itsIter-n);}

    bool operator==(const its_type& r) const    {return itsIter == r.itsIter;}
    bool operator!=(const its_type& r) const    {return itsIter != r.itsIter;}
    bool operator<(const its_type& r) const     {return itsIter < r.itsIter;}

private:
    Iter itsIter;
};

#ifndef NO_PARTIAL_SPECIALIZATION
#   define POINTERATOR pointerator<Iter>
#   define TEMPLATE_ARGS template <typename Iter>
#else
#   define POINTERATOR pointerator<Iter, T>
#   define TEMPLATE_ARGS template <typename Iter, typename T>
#endif

TEMPLATE_ARGS inline POINTERATOR
operator+(POINTERATOR ::difference_type n, const POINTERATOR& r)
{
    return POINTERATOR(x.get_iterator() - n);
}

TEMPLATE_ARGS inline POINTERATOR ::difference_type
operator-(const POINTERATOR& l, const POINTERATOR& r)
{
    return l.get_iterator() - r.get_iterator();
}

TEMPLATE_ARGS inline POINTERATOR
make_pointerator(Iter it)
{
    return POINTERATOR(it);
}

#undef POINTERATOR
#undef TEMPLATE_ARGS

#endif // POINTERATOR_H
