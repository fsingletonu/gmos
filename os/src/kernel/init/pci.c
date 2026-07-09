/*
 * pci.c
 * (C) 2026 Guo Fu
 */
#include <init/pci.h>

uint32_t PciConfigReadDword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t address = 0;
    uint32_t read_dword = 0;
    uint32_t kbus = (uint32_t)bus;
    uint32_t kdevice = (uint32_t)device;
    uint32_t kfunction = (uint32_t)function;
    address = (uint32_t)((kbus << 16) | (kdevice << 11) | (kfunction << 8) | ((uint32_t)offset & 0xfc) | ((uint32_t)0x80000000));
    out_dword(PCI_CONF_ADDR, address);
    read_dword = in_dword(PCI_CONF_DATA);
    return read_dword;
}

void set_pci_table_addr()
{
    uint32_t keep = bootmem_data.current_bump_ptr;
    keep = (keep + 4095) & (~4095);
    pci_table_addr = keep;
    bootmem_data.current_bump_ptr = keep;
    pci_table_addr_init_keep = keep;
}

void pci_scan_bus(uint8_t bus)
{
    for (size_t j = 0; j < 32; j++)
    {
        if ((uint16_t)PciConfigReadDword(bus, j, 0, 0x00) == 0xffff)
            continue;

        // 有设备，则至少有一个功能号
        uint32_t k1 = PciConfigReadDword(bus, j, 0, 0x0c);
        bool multi_flags = ((k1 >> 16) & 0x80) != 0;
        if (multi_flags)
        {
            for (size_t k = 0; k < 8; k++)
            {
                if ((uint16_t)PciConfigReadDword(bus, j, k, 0x00) == 0xffff)
                    continue;

                // 开始解析PCI设备表
                pci_device_id_t *item = (pci_device_id_t *)pci_table_addr_init_keep;
                uint32_t keep1 = PciConfigReadDword(bus, j, k, 0x00);
                item->vendor_id = (uint16_t)keep1;
                item->device_id = (uint16_t)((keep1 >> 16) & 0xffff);
                uint32_t keep2 = PciConfigReadDword(bus, j, k, 0x08);
                item->subclass = (uint8_t)((keep2 >> 16) & 0xff);
                item->class = (uint8_t)((keep2 >> 24) & 0xff);
                item->bar_info.flags = 0;
                for (size_t i = 0; i < 6; i++)
                {
                    uint32_t bar_val = PciConfigReadDword(bus, j, k, 0x10 + i * 4);
                    if (bar_val & 0x1)
                    {
                        item->bar_info.flags = item->bar_info.flags | (1 << i);
                        uint16_t io_addr = bar_val & 0xfffc;
                        item->bar_info.bar[i] = io_addr;
                    }
                    else
                    {
                        uint32_t mem_addr = bar_val & 0xfffffff0;
                        item->bar_info.bar[i] = mem_addr;
                    }
                }
                uint32_t keep3 = PciConfigReadDword(bus, j, k, 0x3c);
                item->interrupt_line = (uint8_t)keep3;
                item->interrupt_pin = (uint8_t)(keep3 >> 8);
                device_count++;
                pci_table_addr_init_keep += sizeof(pci_device_id_t);

                uint32_t k3 = PciConfigReadDword(bus, j, k, 0x08);
                uint16_t class = (uint16_t)(k3 >> 16);
                uint8_t next_bus = (uint8_t)(PciConfigReadDword(bus, j, k, 0x18) >> 8);
                if (class == ((CLASS_BRIAGE << 8) | SUBCLASS_BRIAGE) && next_bus != 0)
                {
                    pci_scan_bus(next_bus);
                }
            }
        }
        else
        {
            // 开始解析PCI设备表
            pci_device_id_t *item = (pci_device_id_t *)pci_table_addr_init_keep;
            uint32_t keep1 = PciConfigReadDword(bus, j, 0, 0x00);
            item->vendor_id = (uint16_t)keep1;
            item->device_id = (uint16_t)((keep1 >> 16) & 0xffff);
            uint32_t keep2 = PciConfigReadDword(bus, j, 0, 0x08);
            item->subclass = (uint8_t)((keep2 >> 16) & 0xff);
            item->class = (uint8_t)((keep2 >> 24) & 0xff);
            item->bar_info.flags = 0;
            for (size_t i = 0; i < 6; i++)
            {
                uint32_t bar_val = PciConfigReadDword(bus, j, 0, 0x10 + i * 4);
                if (bar_val & 0x1)
                {
                    item->bar_info.flags = item->bar_info.flags | (1 << i);
                    uint16_t io_addr = bar_val & 0xfffc;
                }
                else
                {
                    uint32_t mem_addr = bar_val & 0xfffffff0;
                }
            }
            uint32_t keep3 = PciConfigReadDword(bus, j, 0, 0x3c);
            item->interrupt_line = (uint8_t)keep3;
            item->interrupt_pin = (uint8_t)(keep3 >> 8);
            device_count++;
            pci_table_addr_init_keep += sizeof(pci_device_id_t);
        }
    }
}

void pci_device_table_init()
{
    // 如果没有MCFG表，就按传统PCI解析，用递归方式遍历PCI树
    if (!seek_acpi_header(MCFG))
    {
        device_count = 0;
        set_pci_table_addr();
        pci_scan_bus(0);
        pci_table_addr_init_keep = (pci_table_addr_init_keep + 4095) & (~4095);
        bootmem_data.current_bump_ptr = pci_table_addr_init_keep;
    }
}

pci_device_id_t *seek_device(uint8_t class, uint8_t subclass)
{
    pci_device_id_t *item = (pci_device_id_t *)pci_table_addr;
    for (size_t i = 0; i < device_count; i++)
    {
        if (item->class == class && item->subclass == subclass)
        {
            return item;
        }
        item++;
    }
    return nullptr;
}
