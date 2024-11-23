#include "limine.h"
#include "sys/log.h"
#include <stddef.h>
#include <sys/acpi.h>
#include <sys/string.h>
#include <mm/memop.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_req = {
    .revision = 0,
    .id = LIMINE_RSDP_REQUEST
};

rsdp_t *rsdp;
int acpi_available = 0;

int is_xsdt = 0;
xsdt_t *xsdt;
rsdt_t *rsdt;
int item_count = 0;

static int acpi_validate_rsdp(char *byte_array, size_t size) {
    uint32_t sum = 0;
    for(int i = 0; i < size; i++) {
        sum += byte_array[i];
    }
    return (sum & 0xFF) == 0;
}

void *acpi_find_table(char *sign) {
    if (!acpi_available)
        return NULL;

    if (is_xsdt) {
        for (int i = 0; i < item_count; i++) {
            acpi_sdt_hdr_t *ptr = (acpi_sdt_hdr_t*)xsdt->sdtAddresses[i];
            if (memcmp(ptr->Signature, sign, strlen(sign)))
                return (void *)ptr;
        }
    } else {
        for (int i = 0; i < item_count; i++) {
            acpi_sdt_hdr_t *ptr = (acpi_sdt_hdr_t*)rsdt->sdtAddresses[i];
            if (memcmp(ptr->Signature, sign, strlen(sign)))
                return (void *)ptr;
        }
    }

    return NULL;
}

void acpi_init() {
    rsdp = rsdp_req.response->address;

    if (!rsdp)
    {
        logln(err, "kinit stage 1", "ACPI is not available: RSDP is NULL!\n");
        return;
    }

    if (rsdp->Revision < 2) {

        if (!acpi_validate_rsdp((char*)rsdp, sizeof(rsdp_t))) {
            logln(err, "kinit stage 1", "ACPI is not available: Was the RSDP hijacked?\n");
            return;
        }
        rsdt = (rsdt_t*)(uint64_t*)rsdp->RsdtAddress;
        item_count = (rsdt->sdtHeader.Length - sizeof(acpi_sdt_hdr_t)) / 4;

    } else {

        is_xsdt = 1;
        if (!acpi_validate_rsdp((char*)rsdp, sizeof(xsdp_t))) {
            logln(err, "kinit stage 1", "ACPI is not available: Was the XSDP hijacked?\n");
            return;
        }

        xsdt = (xsdt_t*)((xsdp_t*)rsdp)->XSDTAddress;
        item_count = (xsdt->sdtHeader.Length - sizeof(acpi_sdt_hdr_t)) / 8;
    }

    // Try finding a FADT


#ifndef SOAPLIN_FORCE_ACPI
    logln(err, "kinit stage 1", "ACPI is badly implemented as of now. Please update\n");
    logln(err, "kinit stage 1", "the kernel, or if it's already to the latest,\n");
    logln(err, "kinit stage 1", "wait...\n");
#else
    acpi_available = 1;

    void *facp = acpi_find_table("FACP");
    if (!facp) {
        logln(err, "kinit stage 1", "ACPI is not available: Didn't find any FADT\n");
        acpi_available = 0;
        return;
    }

    logln(progress, "kinit stage 1", "ACPI has been initialized\n");
#endif
}