
MEMORY
{
  flash    (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  ram      (rw) : ORIGIN = 0x40000000, LENGTH = 0x2000
  ramstack (rw) : ORIGIN = 0x40002000, LENGTH = 0x2000
	usbram   (rw) : ORIGIN = 0x7FD00000, LENGTH = 0x2000 
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

  .initcalls :
  {
  __initcalls_start__ = . ;
    KEEP(*(.bus_initcalls))
    KEEP(*(.class_initcalls))
    KEEP(*(.driver_initcalls))
    KEEP(*(.aos_initcalls))
  __initcalls_end__ = . ;
  } > flash

	

		/* .ctors .dtors are used for c++ constructors/destructors */
		/* added by Martin Thomas 4/2005 based on Anglia Design example */
		.ctors :
		{
						PROVIDE(__ctors_start__ = .);
						KEEP(*(SORT(.ctors.*)))
						KEEP(*(.ctors))
						PROVIDE(__ctors_end__ = .);
		} >flash

		.dtors :
		{
						PROVIDE(__dtors_start__ = .);
						KEEP(*(SORT(.dtors.*)))
						KEEP(*(.dtors))
						PROVIDE(__dtors_end__ = .);
		} >flash

  . = ALIGN(4);

  .rodata :
  {
    *(.rodata)
  } > flash

  . = ALIGN(4);

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } >flash

	_etext = . ;
	__end_of_text__  = .;
	__data_beg_src__ = .;
  PROVIDE (etext = .);

  .data : AT (_etext)
  {
  	__data_beg__     = .;
    _data = . ;
    *(.data)
		 KEEP(*(.jcr))
		*(.got.plt) *(.got)
		*(.shdata)
		*(.data .data.* .gnu.linkonce.d.*)
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
		*(.shbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
  } > ram
  . = ALIGN(4);
  __bss_end__ = . ;

  _end = .;
  PROVIDE (end = .);
  
	stacklayout :
	{
		__stack_usr_bottom__ = .;
		. = . + 0x0400*1; /* 1 kb */
		__stack_usr_top__    = .;
		__stack_svc_bottom__ = .;
		. = . + 0x0400*1; /* 1 kb */
		__stack_svc_top__    = .;
		__stack_irq_bottom__ = .;
		. = . + 0x0400; /* 1 kb */
		__stack_irq_top__    = .;
		__stack_fiq_bottom__ = .;
		. = . + 0x0000; /* 0 bytes */
		__stack_fiq_top__    = .;
		__stack_und_bottom__ = .;
		. = . + 0x0000; /* 0 bytes */
		__stack_und_top__    = .;
		__stack_iap_bottom__ = .;
	} >ramstack

}

