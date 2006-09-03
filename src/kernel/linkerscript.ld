
MEMORY
{
 	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256k
  RAM (rw) : ORIGIN = 0x40000000, LENGTH = 16k
}
/*PROVIDE (Top_Stack = 0x40000000 + 16k - 4);*/
PROVIDE (Top_Stack = 0x40003B74 );


SECTIONS
{

  .text :
  {
    KEEP(kernel/startup.o (.text))         /* Startup code */
    *(.text)                   /* remaining code */

    *(.glue_7t) *(.glue_7)

  } > FLASH =0

  . = ALIGN(4);


  .rodata :
  {
    *(.rodata)
  } > FLASH

  . = ALIGN(4);


	.initcalls :
	{
		*(.initcalls)
	} > FLASH

  . = ALIGN(4);

  _etext = . ;
  PROVIDE (etext = .);


  .data : AT (_etext)
  {
    _data = . ;
    *(.data)
    SORT(CONSTRUCTORS)
  } >RAM
  . = ALIGN(4);

  _edata = . ;
   PROVIDE (edata = .);

  .bss :
  {
    __bss_start = . ;
    __bss_start__ = . ;
    *(.bss)
    *(COMMON)
  } >RAM
  . = ALIGN(4);
  __bss_end__ = . ;
  __bss_end__ = . ;

  _end = .;
  PROVIDE (end = .);

}

