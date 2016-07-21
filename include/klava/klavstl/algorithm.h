// algorithm.h
//

#ifndef algorithm_h_INCLUDED
#define algorithm_h_INCLUDED

#include <kl_types.h>

#include <klava/klavstl/allocator.h>
#include <klava/klavstl/function.h>

namespace klavstl {

template <class Iter, class T, class Comp>
Iter lower_bound (const Iter& first, const Iter& last, const T& val, const Comp& comp)
{
	Iter base = first;
	distance_type len = (distance_type)(last - first);
	distance_type half;
	Iter mid;

	while (len > 0)
	{
		half = len / 2;
		mid = base + half;
		if (comp (*mid, val))
		{
			base = mid + 1;
			len -= half + 1;
		}
		else
		{
			len = half;
		}
	}

	return base;
}

template <class Iter, class T, class Comp>
Iter upper_bound (const Iter& first, const Iter& last, const T& val, const Comp& comp)
{
	Iter base = first;
	distance_type len = (distance_type)(last - first);
	distance_type half;
	Iter mid;

	while (len > 0)
	{
		half = len / 2;
		mid = base + half;
		if (comp (val, *mid))
		{
			len = half;
		}
		else
		{
			base = mid + 1;
			len -= half + 1;
		}
	}
	return base;
}

// TEMPLATE FUNCTION find

template<class Iter, class Type>
inline
Iter do_find (Iter first, Iter last, const Type& val)
{
	for (; first != last; ++first)
		if (*first == val)
			break;
	return (first);
}

template<class Iter, class Type>
inline
Iter find (Iter first, Iter last, const Type& val)
{
	first = do_find (first, last, val);
	return (first);
}

template <class Iter1, class Iter2>
Iter1 search (
		Iter1 first1, Iter1 last1,
		Iter2 first2, Iter2 last2
	)
{
	// Test for empty ranges
	if (first1 == last1 || first2 == last2)
		 return first1;

	 // Test for a pattern of length 1.
	 Iter2 tmp(first2);
	 ++tmp;
	 if (tmp == last2)
		 return find(first1, last1, *first2);

	 // General case.
	 Iter2 p1 = first2;
	 ++p1;

	 Iter1 current;

	 while (first1 != last1)
	 {
		 first1 = find(first1, last1, *first2);
		 if (first1 == last1)
			 return last1;

		 Iter2 p = p1;
		 current = first1;
		 if (++current == last1)
			 return last1;

		 while (*current == *p)
		 {
			 if (++p == last2)
				 return first1;
			 if (++current == last1)
				 return last1;
		 }

		 ++first1;
	 }
	 return first1;
}

template<class InIter, class OutIter> inline
OutIter copy (InIter first, InIter last, OutIter out)
{
	for (; first != last; ++out, ++first)
		*out = *first;
	return (out);
}


} // namespace klavstl

#endif // algorithm_h_INCLUDED


