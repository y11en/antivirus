#ifndef assert_hpp
#define assert_hpp

extern bool assert_impl(const wchar_t* message, const wchar_t* file, unsigned line);
#define ASSERT(expression)                                                                     \
				do {                                                                           \
					if (!(expression)) {                                                       \
						if (assert_impl(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), __LINE__))\
							__asm { int 3 };                                                   \
					}                                                                          \
				} while (false)

#endif //assert_hpp
