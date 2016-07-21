// function.h
//

#ifndef function_h_INCLUDED
#define function_h_INCLUDED

namespace klavstl {

template <class Arg, class Res>
struct unary_function
{
  typedef Arg argument_type;
  typedef Res result_type;
};

template <class Arg1, class Arg2, class Res>
struct binary_function
{
  typedef Arg1 first_argument_type;
  typedef Arg2 second_argument_type;
  typedef Res  result_type;
};      

template <class T>
struct equal_to : public binary_function<T, T, bool>
{
	inline bool operator() (const T& a, const T& b) const
		{ return a == b; }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool> 
{
	inline bool operator() (const T& a, const T& b) const
		{ return a != b; }
};

template <class T>
struct greater : public binary_function<T, T, bool> 
{
	inline bool operator() (const T& a, const T& b) const
		{ return a > b; }
};

template <class T>
struct less : public binary_function<T, T, bool> 
{
	inline bool operator() (const T& a, const T& b) const
		{ return a < b; }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool> 
{
	inline bool operator() (const T& a, const T& b) const
		{ return a >= b; }
};

template <class T>
struct less_equal : public binary_function<T, T, bool> 
{
	inline bool operator() (const T& a, const T& b) const
		{ return a <= b; }
};

} // namespace klavstl


#endif // function_h_INCLUDED

