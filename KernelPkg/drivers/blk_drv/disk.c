#include <drivers/blk_drv/disk.h>

void delay_400ns()
{
#if defined(__i386__)
    in_byte_x86(0x3f6);
    in_byte_x86(0x3f6);
    in_byte_x86(0x3f6);
    in_byte_x86(0x3f6);
#elif defined(__x86_64__)
    in_byte_x64(0x3f6);
    in_byte_x64(0x3f6);
    in_byte_x64(0x3f6);
    in_byte_x64(0x3f6);
#endif
}

uint8_t read_status()
{
    uint8_t status = 0;
#if defined(__i386__)
    status = in_byte_x86(LBA_COMMAND_PORT);
#elif defined(__x86_64__)
    status = in_byte_x64(LBA_COMMAND_PORT);
#endif
    return status;
}

uint16_t read_data()
{
    uint16_t data = 0;
#if defined(__i386__)
    data = in_word_x86(LBA_DATA_PORT);
#elif defined(__x86_64__)
    data = in_word_x64(LBA_DATA_PORT);
#endif
    return data;
}

void write_data(uint16_t data)
{
#if defined(__i386__)
    out_word_x86(LBA_DATA_PORT, data);
#elif defined(__x86_64__)
    out_word_x64(LBA_DATA_PORT, data);
#endif
}

void wait_not_busy()
{
    uint8_t status = 0;
    do
    {
        status = read_status();
    } while (status & 0x80);
}

void wait_disk_ready()
{
    uint8_t status = 0;
    do
    {
        status = read_status();
    } while (!(status & 0x40));
}

void wait_data_ready_request()
{
    uint8_t status = 0;
    do
    {
        status = read_status();
    } while (!(status & 0x08));
}

void pre_send_command()
{
    wait_not_busy();
    wait_disk_ready();
}

void after_send_command()
{
    wait_not_busy();
    wait_data_ready_request();
}

void send_identify_device_command()
{
#if defined(__i386__)
    out_byte_x86(LBA_COMMAND_PORT, LBA_IDENTIFY_DEVICE_COMMAND);
#elif defined(__x86_64__)
    out_byte_x64(LBA_COMMAND_PORT, LBA_IDENTIFY_DEVICE_COMMAND);
#endif
}

phys_addr read_identify_device(mmzone_t *zone)
{
    page_t *blk = alloc_pages(zone, 0);
    phys_addr addr = page2phys(zone, blk);
    setmem((void *)addr, 512, 0);
    uint16_t *read_addr = (uint16_t *)addr;
    out_byte_x64(LBA_MORE_DISK_PORT, 0xf0);
    delay_400ns();
    pre_send_command();
    send_identify_device_command();
    after_send_command();
    for (size_t i = 0; i < 256; i++, read_addr++)
    {
        uint16_t data = read_data();
        *read_addr = data;
    }
    return addr;
}

uint32_t get_sector_bytes(phys_addr addr)
{
    // 这里直接先假定
    uint32_t bytes = 512;
    uint16_t *word_mess = (uint16_t *)addr + 106;
    // 如果无效，直接返回一个扇区512字节
    if (!((*word_mess & 0xc000) == 0x4000))
        return bytes;
    else
    {
        if ((*word_mess & 0x1000) == 0x0000)
            return bytes;
        else
        {
            // 这里去找word117 and word118
            uint16_t *word117 = (uint16_t *)addr + 117;
            uint16_t *word118 = (uint16_t *)addr + 118;
            uint32_t logical_word_sizes = ((uint32_t)*word118 << 16) | *word117;
            bytes = logical_word_sizes * 2;
        }
    }
    return bytes;
}

void lba28_select(uint64_t lba_num)
{
#if defined(__i386__)
    out_byte_x86(LBA_BLOCK_LOW_PORT, (uint8_t)lba_num);
    out_byte_x86(LBA_BLOCK_MID_PORT, (uint8_t)(lba_num >> 8));
    out_byte_x86(LBA_BLOCK_HIGH_PORT, (uint8_t)(lba_num >> 16));
#elif defined(__x86_64__)
    out_byte_x64(LBA_BLOCK_LOW_PORT, (uint8_t)lba_num);
    out_byte_x64(LBA_BLOCK_MID_PORT, (uint8_t)(lba_num >> 8));
    out_byte_x64(LBA_BLOCK_HIGH_PORT, (uint8_t)(lba_num >> 16));
#endif
}

DISK_STATUS write_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device)
{
    if (count == 0)
        count = 256;

    wait_not_busy();

#if defined(__i386__)
    out_byte_x86(LBA_MORE_DISK_PORT, (((uint8_t)(lba_num >> 24)) & 0x0f) | m_s_device);
#elif defined(__x86_64__)
    out_byte_x64(LBA_MORE_DISK_PORT, (((uint8_t)(lba_num >> 24)) & 0x0f) | m_s_device);
#endif
    delay_400ns();

    lba28_select(lba_num);

#if defined(__i386__)
    out_byte_x86(LBA_SECTOR_COUNT_PORT, (uint8_t)count);
#elif defined(__x86_64__)
    out_byte_x64(LBA_SECTOR_COUNT_PORT, (uint8_t)count);
#endif

    pre_send_command();
#if defined(__i386__)
    out_byte_x86(LBA_COMMAND_PORT, LBA28_WRITE_COMMAND);
#elif defined(__x86_64__)
    out_byte_x64(LBA_COMMAND_PORT, LBA28_WRITE_COMMAND);
#endif
    after_send_command();

    uint16_t *word_addr = (uint16_t *)buffer;
    for (size_t i = 0; i < count; i++)
    {
        wait_not_busy();
        wait_data_ready_request();
        for (size_t j = 0; j < 256; j++, word_addr++)
        {
            write_data(*word_addr);
        }
    }

    return 0;
}

DISK_STATUS read_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device)
{
    if (count == 0)
        count = 256;

    wait_not_busy();

#if defined(__i386__)
    out_byte_x86(LBA_MORE_DISK_PORT, (((uint8_t)(lba_num >> 24)) & 0x0f) | m_s_device);
#elif defined(__x86_64__)
    out_byte_x64(LBA_MORE_DISK_PORT, (((uint8_t)(lba_num >> 24)) & 0x0f) | m_s_device);
#endif
    delay_400ns();

    lba28_select(lba_num);

#if defined(__i386__)
    out_byte_x86(LBA_SECTOR_COUNT_PORT, (uint8_t)count);
#elif defined(__x86_64__)
    out_byte_x64(LBA_SECTOR_COUNT_PORT, (uint8_t)count);
#endif

    pre_send_command();
#if defined(__i386__)
    out_byte_x86(LBA_COMMAND_PORT, LBA28_READ_COMMAND);
#elif defined(__x86_64__)
    out_byte_x64(LBA_COMMAND_PORT, LBA28_READ_COMMAND);
#endif
    after_send_command();

    uint16_t *word_addr = (uint16_t *)buffer;
    for (size_t i = 0; i < count; i++)
    {
        wait_not_busy();
        wait_data_ready_request();
        for (size_t j = 0; j < 256; j++, word_addr++)
        {
            *word_addr = read_data();
        }
    }

    return 0;
}

DISK_STATUS read_more_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device)
{
    uint8_t *now_buffer = (uint8_t *)buffer;
    uint64_t now_lba_num = lba_num;
    while (count > 0)
    {
        if (count >= 256)
        {
            read_lba28_sectors((void *)now_buffer, now_lba_num, 0, m_s_device);
            now_buffer += 512;
            now_lba_num += 256;
            count -= 256;
        }
        else
        {
            read_lba28_sectors((void *)now_buffer, now_lba_num, count, m_s_device);
            count = 0;
        }
    }
}

DISK_STATUS write_more_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device)
{
    uint8_t *now_buffer = (uint8_t *)buffer;
    uint64_t now_lba_num = lba_num;
    while (count > 0)
    {
        if (count >= 256)
        {
            write_lba28_sectors((void *)now_buffer, now_lba_num, 0, m_s_device);
            now_buffer += 512;
            now_lba_num += 256;
            count -= 256;
        }
        else
        {
            write_lba28_sectors((void *)now_buffer, now_lba_num, count, m_s_device);
            count = 0;
        }
    }
}