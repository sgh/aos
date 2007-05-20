#ifndef STACK_H
#define STACK_H

#include <aos_module.h>
#include <bits.h>

#define STACK_USR BIT0
#define STACK_IRQ BIT1
#define STACK_SVC BIT2

void init_runtime_check(void);
void check_stack(void);

#endif
