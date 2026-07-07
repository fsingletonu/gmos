#include <fs/fat32/fat32.h>

void fat_global_init()
{
    global_fat.flags = 0;
    global_fat.fat_entry_total_count = 0;
    global_fat.global_fat_addr = 0;
}

void fat32_global_fat_init(mmzone_t *zone)
{
    if (global_fat.flags)
        return;

    page_t *blk = alloc_pages(zone, 0);
    phys_addr blk_addr = page2phys(zone, blk);
    fat32_bootsector_t *fat32_boot = (fat32_bootsector_t *)blk_addr;

    read_lba28_sectors((void *)fat32_boot, 0, 1, LBA28_SLAVE_DISK);

    uint32_t sectors_per_fat_32 = fat32_boot->sectors_per_fat_32;
    uint16_t bytes_per_sector = fat32_boot->bytes_per_sector;
    uint64_t total_bytes = bytes_per_sector * sectors_per_fat_32;
    size_t alloc_order = get_power_two(total_bytes) - PAGE_ORDER;

    page_t *fat_blk = alloc_pages(zone, alloc_order);
    phys_addr fat_blk_addr = page2phys(zone, fat_blk);

    read_more_lba28_sectors((void *)fat_blk_addr, fat32_boot->reserved_sectors, sectors_per_fat_32, LBA28_SLAVE_DISK);

    global_fat.flags = 1;
    global_fat.fat_entry_total_count = total_bytes / 4;
    global_fat.global_fat_addr = fat_blk_addr;

    free_pages(zone, blk);
}

uint32_t get_free_cluster(fat32_global_fat_t fat)
{
    uint32_t fat_entry_total_count = global_fat.fat_entry_total_count;
    uint32_t *fat_entry = (uint32_t *)fat.global_fat_addr;
    for (size_t i = 0; i < fat_entry_total_count; i++, fat_entry++)
    {
        if (((*fat_entry) & 0x0fffffff) == FREE_CLUSTER)
            return i;
    }
    return 0;
}