#ifndef AOS_ASSERT_H
#define AOS_ASSERT_H

#include <aos/macros.h>

#ifndef NODEBUG

	#define assert(exp) (exp) ? (void)0 : assert_failed(__STRING(exp), __FILE__, __LINE__);
	#define sys_assert(exp) (exp) ? (void)0 : sys_assert_failed(__STRING(exp), __FILE__, __LINE__);

#else

	#define assert(exp) 
	#define sys_assert(exp)

#endif

#ifdef __GNUC__
#define static_assert(test, msg) typedef char _static_assert_ ## msg [ (test) ? 1 : -1]
#endif

#ifdef WIN32
#define static_assert(test, msg)
#endif

#ifdef __cplusplus
	extern "C" {
#endif

void assert_failed(const char * const exp, const char* const file, unsigned int line);

#ifdef __cplusplus
	}
#endif

#endif
