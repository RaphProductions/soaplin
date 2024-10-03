# Soaplin
Soaplin is an operating system kernel for the x86_64 architecture, meant to be used on Project Jupiter. 
You are free to use it as it uses the MIT License.

## Features
* x86_64 architecture implementation (GDT / TSS / legacy PIC / interrupts)
* Memory management (basic memops / PMM / VMM)

## Objectives
* Be able to run a small Linux userspace (a libc & busybox)

## Acknowledgements
Soaplin in it's current state would not be possible without all
these third-party projects:

* Limine (by Mintsuki and contributors): A great long-mode aware bootloader with it's own protocol.
* Flanterm (by Mintsuki): A terminal emulator that renders on a framebuffer array. Used in Limine
* NanoPrintF (by Charles Nicholson): A small printf implementation used in Soaplin for displaying kernel logs with formatting.
* freestnd-c-hdrs-0bsd (by Mintsuki and contributors): 0BSD-licensed (so MIT-compatible) freestanding headers
