#include <core/smp.h>

size_t get_core_count_init()
{
    size_t count = 0;
    acpi_madt_t *madt_addr = (acpi_madt_t *)seek_acpi_signature(APIC);
    acpi_subtable_header_t *entry = (acpi_subtable_header_t *)((uint8_t *)madt_addr + sizeof(acpi_madt_t));
    acpi_subtable_header_t *end = (acpi_subtable_header_t *)((uint8_t *)madt_addr + madt_addr->header.length);

    if (madt_addr->header.length < sizeof(acpi_madt_t))
        return 1;

    while (entry < end)
    {
        size_t type = entry->type;
        size_t length = entry->length;

        if (length == 0)
            break;

        if (type == 0x00)
        {
            acpi_madt_local_apic_t *xapic = (acpi_madt_local_apic_t *)entry;
            uint32_t flags = xapic->flags & 0x01;
            if (flags == 0x01)
                count++;
        }

        if (type == 0x09)
        {
            acpi_madt_local_x2apic_t *x2apic = (acpi_madt_local_x2apic_t *)entry;
            uint32_t flags = x2apic->flags & 0x01;
            if (flags == 0x01)
                count++;
        }
        entry = (acpi_subtable_header_t *)((uint8_t *)entry + length);
    }

    return count > 0 ? count : 1;
}