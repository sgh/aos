#ifndef ASSERT_H
#define ASSERT_H

#include <aos_hooks.h>
#include <macros.h>

#define assert(exp) (exp) ? (void)0 : assert_failed(__STRING(exp), __FILE__, __LINE__);


void assert_failed(const char * const exp, const char* const file, unsigned int line);



#endif
