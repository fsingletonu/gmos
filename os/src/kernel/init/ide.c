#include <init/ide.h>

void enable_ide_int()
{
    out_byte(CONTROL_PORT, 0x00);
}

void disable_ide_int()
{
    out_byte(CONTROL_PORT, 0x02);
}

void send_identify_command()
{
    out_byte(DEVICE_MASTER_PORT, 0xa0);
    out_byte(STATUS_COMMAND_MASTER_PORT, IDE_IDENTIFY);
}

void get_base_storage_mess()
{
    storage_base_mess.lba_mode = NO_LBA;
    send_identify_command();
    while (in_byte(STATUS_COMMAND_MASTER_PORT) & 0x80)
        ;
    while (!(in_byte(STATUS_COMMAND_MASTER_PORT) & 0x08))
        ;
    uint16_t id[256];
    for (size_t i = 0; i < 256; i++)
    {
        id[i] = in_word(DATA_MASTER_PORT);
    }
    uint16_t support_lba = id[49] & (1 << 9);
    if (support_lba)
        storage_base_mess.lba_mode = LBA_28;
    uint16_t lba48 = id[83] & (1 << 10);
    storage_base_mess.sector_size = 512;
    storage_base_mess.total_sectors = ((uint32_t)id[61] << 16) | id[60];
}

void read_sections_polling(phys_addr_t data_area, uint8_t count)
{
    uint16_t *ptr = (uint16_t *)data_area;
    for (size_t i = 0; i < (storage_base_mess.sector_size >> 1) * count; i++, ptr++)
    {
        *ptr = in_word(DATA_MASTER_PORT);
    }
}

void write_sections_polling()
{
}

void send_request_lba_28(uint8_t count, uint32_t lba_addr, enum device_ide_type type, enum work_mode work, phys_addr_t data_area)
{
    uint32_t keep_lba_addr = lba_addr;
    out_byte(COUNT_PORT, count);
    out_byte(LBA_LOW_PORT, keep_lba_addr & 0xFF);
    out_byte(LBA_MID_PORT, (keep_lba_addr >> 8) & 0xFF);
    out_byte(LBA_HIGH_PORT, ((keep_lba_addr) >> 16) & 0xFF);
    uint32_t port_0x1f6 = (keep_lba_addr >> 24) & 0x0000000f;
    port_0x1f6 |= 0xe0;
    port_0x1f6 |= type;
    out_byte(DEVICE_MASTER_PORT, port_0x1f6);
    out_byte(STATUS_COMMAND_MASTER_PORT, work);
    if (count == 0)
        for (size_t i = 0; i < 256; i++)
        {
            while (in_byte(STATUS_COMMAND_MASTER_PORT) & 0x80)
                ;
            while (!(in_byte(STATUS_COMMAND_MASTER_PORT) & 0x08))
                ;
            read_sections_polling(data_area, count);
        }
    for (size_t i = 0; i < count; i++)
    {
        while (in_byte(STATUS_COMMAND_MASTER_PORT) & 0x80)
            ;
        while (!(in_byte(STATUS_COMMAND_MASTER_PORT) & 0x08))
            ;
        read_sections_polling(data_area, count);
    }
}

void send_request_dma(uint8_t count, uint32_t lba_addr, enum device_ide_type type, enum work_mode work)
{
    uint32_t keep_lba_addr = lba_addr;
    out_byte(COUNT_PORT, count);
    out_byte(LBA_LOW_PORT, keep_lba_addr & 0xFF);
    out_byte(LBA_MID_PORT, (keep_lba_addr >> 8) & 0xFF);
    out_byte(LBA_HIGH_PORT, ((keep_lba_addr) >> 16) & 0xFF);
    uint32_t port_0x1f6 = (keep_lba_addr >> 24) & 0x0000000f;
    port_0x1f6 |= 0xe0;
    port_0x1f6 |= type;
    out_byte(DEVICE_MASTER_PORT, port_0x1f6);
    out_byte(STATUS_COMMAND_MASTER_PORT, work);
    while (in_byte(STATUS_COMMAND_MASTER_PORT) & 0x80)
        ;
}

void send_ide_request(uint32_t lba_addr, uint8_t count, enum device_ide_type type, enum work_mode work, phys_addr_t data_area)
{
    // 前两个都是CPU轮询等待IDE准备好数据
    switch (work)
    {
    case WORK_READ_SECTIONS_28_POLLING:
        disable_ide_int();
        send_request_lba_28(count, lba_addr, type, work, data_area);
        break;

    case WORK_WRITE_SECTIONS_28_POLLING:
        disable_ide_int();
        send_request_lba_28(count, lba_addr, type, work, data_area);
        write_sections_polling();
        break;

    case WORK_READ_IDE_DMA:
        send_request_dma(count, lba_addr, type, work);
        break;

    case WORK_WRITE_IDE_DMA:
        send_request_dma(count, lba_addr, type, work);
        break;

    default:
        break;
    }
}

void ide_dma_r_iomap_init()
{
    pci_device_id_t *keep = seek_device(CLASS_STORAGE_CONTROLLER, SUBCLASS_IDE);
    uint16_t bar4_keep = keep->bar_info.bar[4];
    ide_dma_r.master_command_r = bar4_keep;
    ide_dma_r.master_status_r = bar4_keep + 0x02;
    ide_dma_r.master_prdt_addr_r = bar4_keep + 0x04;
}

void set_prdt(phys_addr_t data_addr, uint16_t byte_count)
{
    prdt.data_addr = nullptr;
    prdt.byte_count = 0;
    prdt.data_addr = data_addr;
    prdt.byte_count = byte_count;
    prdt.eot_flags = 0x8000;
}

void prdt_init()
{
    prdt.data_addr = nullptr;
    prdt.byte_count = 0;
    prdt.eot_flags = 0x8000;
}

// 这里的命令只有读/写 要包含启动
void set_ide_dma_r(uint8_t command, phys_addr_t prdt_addr)
{
    out_byte(ide_dma_r.master_command_r, command);
    out_dword(ide_dma_r.master_prdt_addr_r, prdt_addr);
    out_byte(ide_dma_r.master_status_r, (1 << 1) | (1 << 2));
}

void ide_init()
{
    get_base_storage_mess();
    ide_dma_r_iomap_init();
    prdt_init();
}

void stop_ide_dma()
{
    uint8_t keep = in_byte(ide_dma_r.master_command_r);
    keep &= 0xfe;
    out_byte(ide_dma_r.master_command_r, keep);
}

void start_ide_dma()
{
    uint8_t keep = in_byte(ide_dma_r.master_command_r);
    keep |= 0x01;
    out_byte(ide_dma_r.master_command_r, keep);
}

// 先设置DMA 再发ATA命令 最后启动DMA
void ide_dma_launch(uint8_t command, phys_addr_t prdt_addr, uint32_t lba_addr, uint8_t count, enum work_mode work)
{
    stop_ide_dma();
    // 对dma的设置
    set_ide_dma_r(command, prdt_addr);
    // 对ata的设置
    send_ide_request(lba_addr, count, IDE_TYPE_MASTER, work, nullptr);
    start_ide_dma();
}

void ide_master_dma_handler()
{
    DEBUG;
}
