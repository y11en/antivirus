#if !defined(__KL_TODO_MACRO_H)
#define __KL_TODO_MACRO_H

#define MAKESTR(x)		#x
#define MAKESTR2(x)		MAKESTR(x)
#define TODO(msg) message(__FILE__"(" MAKESTR2(__LINE__) "): TODO: " msg)

#endif   // !defined(__KL_TODO_MACRO_H)
