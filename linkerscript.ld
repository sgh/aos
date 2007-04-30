
MEMORY
{
  flash    (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  ram      (rw) : ORIGIN = 0x40000000, LENGTH = 0x2000
  ramstack (rw) : ORIGIN = 0x40002000, LENGTH = 0x2000
 
}

/* 
 * Define entry point, found in startarm.s
 */
ENTRY(_reset_handler);

SECTIONS
{
	startup : 
  { 
    *(.startup)
  } > flash =0

  .text :
  {
    *(.text)                   /* remaining code */
    *(.glue_7t) *(.glue_7)
  } > flash

  . = ALIGN(4);

  .rodata :
  {
    *(.rodata)
  } > flash

  . = ALIGN(4);

	__initcalls_start__ = . ;
  .initcalls :
  {
    *(.bus_initcalls)
    *(.class_initcalls)
    *(.driver_initcalls)
    *(.aos_initcalls)
	} > flash
	__initcalls_end__ = . ;

  . = ALIGN(4);

	_etext = . ;
	__end_of_text__  = .;
	__data_beg_src__ = .;
  PROVIDE (etext = .);

  .data : AT (_etext)
  {
  	__data_beg__     = .;
    _data = . ;
    *(.data)
    SORT(CONSTRUCTORS)
  } > ram
  . = ALIGN(4);
  __data_end__     = .;

  _edata = . ;
   PROVIDE (edata = .);

  .bss :
  {
		__bss_beg__ = .;
    __bss_start = . ;
    __bss_start__ = . ;
    *(.bss)
    *(COMMON)
  } > ram
  . = ALIGN(4);
  __bss_end__ = . ;

  _end = .;
  PROVIDE (end = .);
  
	stacklayout :
	{
		__stack_usr_bottom__ = .;
		. = . + 0x0400*2; /* 2 kb */
		__stack_usr_top__    = .;
		__stack_svc_bottom__ = .;
		. = . + 0x0400*2; /* 2 kb */
		__stack_svc_top__    = .;
		__stack_irq_bottom__ = .;
		. = . + 0x0400*1; /* 1 kb */
		__stack_irq_top__    = .;
		__stack_fiq_bottom__ = .;
		. = . + 0x0080; /* 128 bytes */
		__stack_fiq_top__    = .;
		__stack_und_bottom__ = .;
		. = . + 0x0080; /* 128 bytes */
		__stack_und_top__    = .;
		__stack_iap_bottom__ = .;
	} >ramstack

}

