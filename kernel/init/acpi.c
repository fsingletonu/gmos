#include <init/acpi.h>

// 这里之后要兼容BIOS BIOS方式采用KMP算法寻找"RSD PTR "
bool rsdp_init(BootInfoHead *Info)
{
    g_rdsp = (RSDPDescriptor_t *)Info->BootAcpiPhysicalAddress;
    if (*(uint64_t *)g_rdsp->Signature != 0x2052545020445352)
    {
        return FALSE;
    }
    return TRUE;
}

virt_addr seek_acpi_signature(char signature[4])
{
    if (g_rdsp->Revision == 0)
    {
        rsdt_t *rsdt = (rsdt_t *)((uint64_t)g_rdsp->RsdtAddress);
        size_t entry_num = (rsdt->header.length - ACPI_SDT_HEADER_LEN) / 4;
        for (size_t i = 0; i < entry_num; i++)
        {
            acpi_sdt_header_t *header = (acpi_sdt_header_t *)((uint64_t)rsdt->table_ptrs[i]);
            if (!memcmp(header->signature, signature, 4))
                return rsdt->table_ptrs[i];
        }
    }
    if (g_rdsp->Revision >= 2)
    {
        rsdt_t *xsdt = (rsdt_t *)((uint64_t)g_rdsp->XsdtAddress);
        size_t entry_num = (xsdt->header.length - ACPI_SDT_HEADER_LEN) / 8;
        for (size_t i = 0; i < entry_num; i++)
        {
            acpi_sdt_header_t *header = (acpi_sdt_header_t *)((uint64_t)xsdt->table_ptrs[i]);
            if (!memcmp(header->signature, signature, 4))
                return xsdt->table_ptrs[i];
        }
    }
    return NULL;
}