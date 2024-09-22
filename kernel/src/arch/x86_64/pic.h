#pragma once

#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_init();
void pic_ack(int intno);
void pic_disable(); // if we want APIC
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);