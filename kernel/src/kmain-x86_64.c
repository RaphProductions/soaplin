// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.


void kmain_x86_64() {

  // The kernel finished it's boot things. Now halt the machine. Interrupts will
  // still be received & handled.
  while (1)
    __asm__ volatile("hlt");
}
