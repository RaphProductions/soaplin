#pragma once

#include <stdint.h>
#include <sys/acpi.h>

typedef struct {
  acpi_sdt_hdr_t hdr;

  /* MADT Specs */
  uint32_t lapic_address;
  uint32_t flags;

  char table[];
} acpi_madt;

typedef struct {
  uint8_t type;
  uint8_t len;
} madt_entry;

typedef struct {
  madt_entry un;
  uint8_t cpu_id;
  uint8_t apic_id;
  uint32_t flags;
} madt_cpu_lapic;

typedef struct {
  madt_entry un;
  uint8_t apic_id;
  uint8_t resv;
  uint32_t apic_addr;
  uint32_t gsi_base;
} madt_ioapic;

typedef struct {
  madt_entry un;
  uint8_t bus_src;
  uint8_t irq_src;
  uint32_t gsi;
  uint16_t flags;
} madt_iso;

typedef struct {
  madt_entry un;
  uint16_t resv;
  uint64_t phys_lapic;
} madt_lapic_addr;

extern madt_ioapic* madt_ioapic_list[128];
extern madt_iso* madt_iso_list[128];

extern uint32_t madt_ioapic_len;
extern uint32_t madt_iso_len;

extern uint64_t* lapic_addr;

void madt_init();