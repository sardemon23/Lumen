ENTRY(start)
OUTPUT_FORMAT("binary")
phys = 0x00100000;

SECTIONS
{
    . = phys;

    .entry   : { __entry_start  = .;  *(.entry)  }
    .text    : { __text_start   = .;  *(.text)   }

    /* Section modules + symboles début/fin (pluriel !) */
    .modules :
    {
        __start_modules = .;
        KEEP(*(.modules))
        __stop_modules  = .;
        . = ALIGN(4);
    }

    .data    : { __data_start   = .;  *(.data)   }
    .rodata  : { __rodata_start = .;  *(.rodata) }
    .bss     : { __bss_start    = .;  *(.bss)    }

    __end = .;
}
