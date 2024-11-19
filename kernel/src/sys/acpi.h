#pragma once

#include <stdint.h>

typedef struct rsdp {
    char sign[8];
    uint8_t chksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed)) rsdp_t;

typedef struct xsdp {
    char sign[8];
    uint8_t chksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;

    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__ ((packed)) xsdp_t;

typedef struct acpi_sdt_hdr {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} acpi_sdt_hdr_t;

typedef struct rsdt {
    acpi_sdt_hdr_t sdtHeader; //signature "RSDP"
    uint32_t sdtAddresses[];
} rsdt_t;

typedef struct xsdt {
    acpi_sdt_hdr_t sdtHeader; //signature "XSDT"
    uint64_t sdtAddresses[];
} xsdt_t;

extern int acpi_available;

void *acpi_find_table(char *sign);
void acpi_init();