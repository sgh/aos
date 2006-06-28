#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOP_H

#include <types.h>


struct file_operations {

	int32 (*init)();
	
	int32 (*sleep)();

	int32 (*read)(void* buf, uint32);
	
	int32 (*write)(void* buf, uint32);
};


enum file_descriptor_enum {UART0,UART1}  file_descriptors;


#endif

