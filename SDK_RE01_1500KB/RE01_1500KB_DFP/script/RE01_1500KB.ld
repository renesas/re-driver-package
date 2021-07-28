/*
                  Linker Script File for R7F0E015/R7F0E017 parts
*/

/* Linker script to configure memory regions. */
MEMORY
{
  FLASH (rx)         : ORIGIN = 0x00000000, LENGTH = 0x0180000  /* 1.5M */
  OFS (rx)      : ORIGIN = 0x0100A150, LENGTH = 0x0000018  /* 24B */
  E2S_TRACE_BUF (rw) : ORIGIN = 0x20000000, LENGTH = 0x000400  /* 1K   */

  /* The e2studio trace buffer requires 0x400 bytes of RAM starting at address 0x20000000. Those have been allocated above.
   * If trace is not used then these 0x400 bytes can be used by the application by changing the below RAM ORIGIN to 0x20000000,
   * increasing the LENGTH by 0x400 and removing the E2S_TRACE_BUF definition. */
  RAM (rwx)          : ORIGIN = 0x20000400, LENGTH = 0x003FC00  /* 255K */
  QSPI_FLASH (rx)    : ORIGIN = 0x60000000, LENGTH = 0x8000000  /* 128M */
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 *   __qspi_flash_start__
 *   __qspi_flash_end__
 *   __qspi_flash_code_size__
 *   __qspi_region_max_size__
 *   __qspi_region_start_address__
 *   __qspi_region_end_address__
 */
ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		__Vectors_Start = .;

		KEEP(*(.intvec))
		KEEP(*(SORT_BY_NAME(.intvec.*)))
		__Vectors_End = .;
		__end__ = .;

		/* ROM Registers start at address 0x00000400 */
		. = 0x400;
		__OptionSetting_start = .;
		KEEP(*(.OptionSetting))
		KEEP(*(.OptionSetting.*))
		__OptionSetting_end = .;

		/* Reserving 0x100 bytes of space for ROM registers. */
		. = . + 0x100;

		*(EXCLUDE_FILE(*pin.o *\libgcc.a:* *\libc.a:* *\libg.a:* *\librdimon.a:* *\libm.a:* *\libnosys.a:* *\libc_nano.a:* *\libg_nano.a:* *\librdimon_nano.a:*) .text*)

		KEEP(*(.version))
		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		*(EXCLUDE_FILE(*pin.o *\libgcc.a:* *\libc.a:* *\libg.a:* *\librdimon.a:* *\libm.a:* *\libnosys.a:* *\libc_nano.a:* *\libg_nano.a:* *\librdimon_nano.a:*) .rodata*)

		__ROM_End = .;
	} > FLASH = 0xFF

    __Vectors_Size = __Vectors_End - __Vectors_Start;

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH
	__exidx_end = .;

	__etext = .;

	 .OptionSettingID 0x0100A150: AT(0x0100A150)
	 {
		  __OptionSettingID_start = .;
		  KEEP(*(.OptionSettingID))
		  KEEP(*(.OptionSettingID.*))
		  __OptionSettingID_end = .;
	 } > OFS
	 
	 .OptionSettingAWS 0x0100A164: AT(0x0100A164)
	 {
		  __OptionSettingAWS_start = .;
		  KEEP(*(.OptionSettingAWS))
		  KEEP(*(.OptionSettingAWS.*))
		  __OptionSettingAWS_end = .;
	 } > OFS

    /* secure vector area on RAM to be copied from ROM */
    .ramvect :
    {
        . = ALIGN(256);
        __ramvect_start = .;
        . = . + 0x400;
        . = ALIGN(4);
    } > RAM

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
		*(.data.*)
        
		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);

		/* All data end */
		__data_end__ = .;

	} > RAM AT> FLASH

    
     .ramfunc :
	{
		__ramfunc_start = .;
		KEEP(*(.ramfunc*))
		. = ALIGN(4);
		__ramfunc_end = .;
	} > RAM AT> FLASH
	__ramfunc_init_start = LOADADDR(.ramfunc);

	.ehc_ramfunc :
	{
		__ehc_ramfunc_start = .;
		KEEP(*(.ehc_ramfunc*))
		. = ALIGN(4);
		__ehc_ramfunc_end = .;
	} > RAM AT> FLASH
	__ehc_ramfunc_init_start = LOADADDR(.ehc_ramfunc);
	
	.ramdata : 
	{
		__ramdata_start = .;
		KEEP(*(.ramdata*))
		. = ALIGN(4);
		__ramdata_end = .;
	} > RAM AT> FLASH
	__ramdata_init_start = LOADADDR(.ramdata);
	
    .ramobj :
	{
		__ramobj_start = .;
		KEEP(*(.ramobj*))
		KEEP("*pin.o"(.text* .rodata*))
		KEEP("*\libgcc.a:*" (.text* .rodata* ))
		KEEP("*\libc.a:*" (.text* .rodata* ))
		KEEP("*\libg.a:*" (.text* .rodata* ))
		KEEP("*\librdimon.a:*" (.text* .rodata* ))
		KEEP("*\libm.a:*" (.text* .rodata* ))
		KEEP("*\libnosys.a:*" (.text* .rodata* ))
		KEEP("*\libc_nano.a:*" (.text* .rodata* ))
		KEEP("*\libg_nano.a:*" (.text* .rodata* ))
		KEEP("*\librdimon_nano.a:*" (.text* .rodata* ))
		. = ALIGN(4);
		__ramobj_end = .;
	} > RAM AT> FLASH
	__ramobj_init_start = LOADADDR(.ramobj);

    .noinit (NOLOAD):
    {
		. = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        __noinit_end = .;
    } > RAM AT> RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM

	.heap (NOLOAD):
	{
        . = ALIGN(8);
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > RAM

	/* Stacks are stored in this section. */
	.stack_dummy (NOLOAD):
	{
        . = ALIGN(8);
	    __StackLimit = .;
		KEEP(*(.stack*))
		__StackTop = .;
	} > RAM

	PROVIDE(__stack = __StackTop);

    /* QSPI_FLASH section to be downloaded via debugger */
    .qspi_flash :
    {
        __qspi_flash_start__ = .;
        KEEP(*(.qspi_flash*))
        __qspi_flash_end__ = .;
    } > QSPI_FLASH
    __qspi_flash_code_size__ = __qspi_flash_end__ - __qspi_flash_start__;

    /* QSPI_FLASH non-retentive section, creates a copy in internal flash that can be copied to QSPI */
    __qspi_flash_code_addr__ = __etext + (__data_end__ - __data_start__);
	.qspi_non_retentive : AT (__qspi_flash_code_addr__)
    {
        __qspi_non_retentive_start__ = .;
        KEEP(*(.qspi_non_retentive*))
        __qspi_non_retentive_end__ = .;
    } > QSPI_FLASH
	__qspi_non_retentive_size__ = __qspi_non_retentive_end__ - __qspi_non_retentive_start__;

    __qspi_region_max_size__ = 0x8000000;   /* Must be the same as defined in MEMORY above */
    __qspi_region_start_address__ = __qspi_flash_start__;
    __qspi_region_end_address__ = __qspi_flash_start__ + __qspi_region_max_size__;

}
