[bits 64]

section .text
    global _start

_start:
    ; Write syscall (sys_write) - syscall number 4
    mov rax, 1            ; Syscall number for sys_write (4)
    mov rdi, 1            ; File descriptor 1 (stdout)
    
    ; Load the address of the string "Hello, World\n"
    lea rsi, [msg]        ; Load effective address of the string into ecx
    mov rdx, 14           ; The length of "Hello, World\n" is 14 bytes

    ; Make the syscall to write the string to stdout using int 0x80
    int 0x80              ; Interrupt to make the syscall

    ; Infinite loop to keep the program running
.loop:
    jmp .loop             ; Jump back to the start of the loop

section .data
    ; The string "Hello, World\n"
msg db 'Hello, World', 10  ; '10' is the newline character (ASCII LF)
