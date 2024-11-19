#include "sys/log.h"
#include <sys/acpi/madt.h>

madt_ioapic* madt_ioapic_list[128] = {0};
madt_iso* madt_iso_list[128] = {0};

uint32_t madt_ioapic_len = 0;
uint32_t madt_iso_len = 0;

uint64_t* lapic_addr = (uint64_t*)0;

void madt_init() {
  acpi_madt* madt = (acpi_madt*)acpi_find_table("APIC");

  if (!madt) {
    logln(err, "acpi/madt", "The MADT is not available. The legacy PIC & PIT will be used.\n");
    return;
  }

  uint64_t off = 0;
  int current_idx = 0;
  madt_ioapic_len = 0;
  madt_iso_len = 0;

  while (1) {
    if (off > madt->hdr.Length - sizeof(madt))
      break;
    
    madt_entry* entry = (madt_entry*)(madt->table + off);

    switch (entry->type) {
      case 0:
        current_idx++;
        break;
      case 1:
        madt_ioapic_list[madt_ioapic_len++] = (madt_ioapic*)entry;
        break;
      case 2:
        madt_iso_list[madt_iso_len++] = (madt_iso*)entry;
        break;
      case 5:
        lapic_addr = (uint64_t*)((madt_lapic_addr*)entry)->phys_lapic;
        break;
    }

    logln(info, "", "madt ioapic len: %d, off: %d, madt->hdr.len: %d, sizeof(madt): %d, entry->len: %d", madt_ioapic_len, off, madt->hdr.Length, sizeof(madt), entry->len);
    logln(info, "base", "%p", entry);

    off += entry->len;
  }

  logln(progress, "kinit stage 1", "ACPI MADT initialized!");
}