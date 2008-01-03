#ifndef ASSERT_H
#define ASSERT_H

#include <aos_hooks.h>
#include <macros.h>

#ifndef NODEBUG

	#define assert(exp) (exp) ? (void)0 : assert_failed(__STRING(exp), __FILE__, __LINE__);
	#define sys_assert(exp) (exp) ? (void)0 : sys_assert_failed(__STRING(exp), __FILE__, __LINE__);

#else

	#define assert(exp) 
	#define sys_assert(exp)

#endif

#define static_assert(test, msg) typedef char _static_assert_ ## msg [ (test) ? 1 : -1]


void assert_failed(const char * const exp, const char* const file, unsigned int line);



#endif
