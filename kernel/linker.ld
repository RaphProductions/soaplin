/* Copyright (C) 2024 Sipaa Projects
   This code is part of the Soaplin kernel and is licensed under the terms of the MIT License. */

OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kmain_x86_64)

PHDRS
{
    requests PT_LOAD;
    text     PT_LOAD;
    rodata   PT_LOAD;
    data     PT_LOAD;
}

SECTIONS
{
    . = 0xffffffff80000000;

    reqs_start_ld = .;

    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :requests

    reqs_end_ld = .;
    
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    text_start_ld = .;
    .text BLOCK(4K) : ALIGN(4K) {
        *(.text .text.*)
    } :text
    text_end_ld = .;


    . = ALIGN(CONSTANT(MAXPAGESIZE));

    rodata_start_ld = .;
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata .rodata.*)
    } :rodata
    rodata_end_ld = .;


    . = ALIGN(CONSTANT(MAXPAGESIZE));

    data_start_ld = .;
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data .data.*)
    } :data


    .bss BLOCK(4K) : ALIGN(4K) {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    data_end_ld = .;

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
