#ifndef _MACROS_H_
#define _MACROS_H_

#include <types.h>

#define offsetof(TYPE,MEMBER) (size_t)&(((TYPE*)0)->MEMBER)

#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

#endif // _MACROS_H_
