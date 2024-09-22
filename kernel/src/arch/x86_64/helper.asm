[bits 64]

section .text

global tss_load
tss_load:
    mov ax, 0x30
    ltr ax
    ret

global gdt_load
gdt_load:
    lgdt [rdi]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp .flush

.flush:
    push 0x08
    push .reload_cs
    retfq

.reload_cs:
    ret

global idt_flush
idt_flush:
    lidt [rdi]
    ret