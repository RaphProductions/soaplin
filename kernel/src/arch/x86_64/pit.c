// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <arch/arch.h>
#include <arch/x86_64/pic.h>
#include <stdint.h>
#include <arch/x86_64/io.h>


uint32_t tick = 0;

#define PIT_FREQUENCY 1193182
#define HZ 1000

/*void Pit_Handler(stackframe *regs)
{
    tick++;

    Scheduler_Schedule(regs);
}*/

void pit_init(uint32_t frequency) {
  uint32_t divisor = PIT_FREQUENCY / frequency;
  outb(0x43, 0x34);
  outb(0x40, (uint8_t)(divisor & 0xFF));
  outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

  // Idt_SetIsrHandler(32, Pit_Handler);
}

void pit_enable() { pic_unmask_irq(0); }

void pit_disable() { pic_mask_irq(0); }

void pit_sleep(uint32_t seconds) {
  uint32_t eticks = tick + seconds * HZ;
  while (tick < eticks) {
    __asm__ __volatile__("hlt");
  }
}

void pit_sleepms(uint32_t milliseconds) {
  uint32_t eticks = tick + (milliseconds * HZ) / 1000;
  while (tick < eticks) {
    __asm__ __volatile__("hlt");
  }
}

// todo: unistd: add usleep function
void pit_usleep(uint32_t milliseconds) {
  uint32_t eticks = tick + (milliseconds * HZ);
  while (tick < eticks) {
    __asm__ __volatile__("hlt");
  }
}
