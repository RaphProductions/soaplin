#pragma once

#include <stdint.h>

typedef struct {
  uint16_t length;
  uint16_t base;
  uint8_t  base1;
  uint8_t  flags;
  uint8_t  flags1;
  uint8_t  base2;
  uint32_t base3;
  uint32_t resv;
} __attribute__((packed)) tss_entry;

typedef struct {
  uint64_t gdt_entries[9];
  tss_entry tss_entry;
} __attribute__((packed)) gdt_table;

typedef struct {
  uint16_t size;
  uint64_t address;
} __attribute__((packed)) gdtr;


typedef struct {
  uint32_t resv;
  uint64_t rsp[3];
  uint64_t resv1;
  uint64_t ist[7];
  uint64_t resv2;
  uint16_t resv3;
  uint16_t iopb;
} __attribute__((packed)) tssr; // Per CPU

void gdt_init();