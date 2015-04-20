/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
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
	#ifndef __cplusplus // static_assert keyword is already included with the same name in c++11, so we only need to rename it for C
		#define static_assert _Static_assert
	#endif
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
