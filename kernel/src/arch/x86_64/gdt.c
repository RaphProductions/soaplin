// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#ifdef __x86_64__

#include <arch/x86_64/gdt.h>
#include <sys/log.h>

__attribute__((aligned(4096))) gdt_entry gdt[0xff];
gdtr gdt_ptr;
tss_entry tss;

extern void gdt_load(gdtr *);
extern void tss_load(void);

void gdt_set_entry(int i, uint16_t limit, uint16_t base, uint8_t access,
                   uint8_t flags) {
  gdt[i].base0 = base & 0xFFFF;
  gdt[i].base1 = (base >> 16) & 0xFF;
  gdt[i].base2 = (base >> 24) & 0xFF;
  gdt[i].limit0 = limit & 0xFFFF;
  gdt[i].access = access;
  gdt[i].limit1_flags = ((limit >> 16) & 0x0F) | (flags & 0xF0);
}

void tss_set_gate(int32_t num, uint64_t base, uint32_t limit) {
  gdt[num].base0 = base & 0xFFFF;
  gdt[num].base1 = (base >> 16) & 0xFF;
  gdt[num].base2 = (base >> 24) & 0xFF;
  gdt[num].limit0 = limit & 0xFFFF;
  gdt[num].access = 0x89;
  gdt[num].limit1_flags = ((limit >> 16) & 0x0F) | 0x00;
}

void gdt_init() {
  gdt_ptr.size = sizeof(gdt) - 1;
  gdt_ptr.offset = (uint64_t)&gdt;

  // 0: NULL
  gdt_set_entry(GDT_NULL, 0, 0, 0, 0);

  // 1: Kernel Code Segment
  gdt_set_entry(GDT_KERNEL_CODE, 0xFFFF, 0, 0x9A, 0xAF); // exec/read, ring 0

  // 2: Kernel Data Segment
  gdt_set_entry(GDT_KERNEL_DATA, 0xFFFF, 0, 0x92, 0xAF); // read/write, ring 0

  // 3: User Code Segment
  gdt_set_entry(GDT_USER_CODE, 0xFFFF, 0, 0x9A, 0xAF); // exec/read, ring 3

  // 4: User Data Segment
  gdt_set_entry(GDT_USER_DATA, 0xFFFF, 0, 0x92, 0xAF); // read/write, ring 3

  tss_set_gate(6, (uint64_t)&tss, sizeof(tss_entry));

  gdt_load(&gdt_ptr);
  tss_load();

  logln(progress, "arch/gdt",
        "A new GDT & TSS has been loaded into the CPU.\n");
}

void gdt_set_kernel_stack(uint64_t stack) { tss.rsp0 = (uint64_t)stack; }

#endif
