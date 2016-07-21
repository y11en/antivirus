#ifndef ENUMERATORIMP_H
#define ENUMERATORIMP_H

namespace KasperskyLicensing {
namespace Implementation {

template <typename T> struct DefaultPredicate
{
	bool operator() (T) { return true; }
};

template <typename _Et, typename _It, typename _Pred = DefaultPredicate<_Et::ValueType> >
	class EnumeratorImp : public _Et::Implement
{
public:

	/**
	 * Constructs iterator for given range.
	 */
	EnumeratorImp(const _It& cur, const _It& end, const _Pred& pr = _Pred()) 
		: itcur(cur), itend(end), pred(pr) 
	{ Position(); }

	/**
	 * Clones object.
	 */
	virtual typename _Et::Implement* Clone() const
		{ return new EnumeratorImp(*this); }

	/**
	 * Dereferences iterator.
	 */
	virtual typename _Et::ValueType Item() const
		{ return *itcur; }

	/**
	 * Returns true if at the end of collection.
	 */
	virtual bool IsDone() const
		{ return itcur == itend; }

	/**
	 * Moves to next element.
	 */
	virtual void Next() 
	{ 
		++itcur; 
		Position(); 
	}

private:

	/**
	 * Constructs copy of object.
	 */
	EnumeratorImp(const EnumeratorImp& rhs)
		: itcur(rhs.itcur), itend(rhs.itend), pred(rhs.pred) {}

	/**
	 * Positions on first satisfying element.
	 */
	void Position()
	{
		for (; itcur != itend && !pred(*itcur); ++itcur);
	}

	// iterators
	_It itcur, itend;
	// predicate
	_Pred pred;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // ENUMERATORIMP_H