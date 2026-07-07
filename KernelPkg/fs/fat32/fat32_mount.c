#include <fs/fat32/fat32.h>

void fat32_fill_bootsectors(mmzone_t *zone)
{
    page_t *blk = alloc_pages(zone, 0);
    phys_addr blk_addr = page2phys(zone, blk);
    fat32_bootsector_t *fat32_boot = (fat32_bootsector_t *)blk_addr;
    fat32_boot->jump[0] = 0xeb;
    fat32_boot->jump[1] = 0x58;
    fat32_boot->jump[2] = 0x90;
    memcopy((void *)fat32_boot->oem, "GMOSDISK", 8);
    ata_identify_device_t *ata = (ata_identify_device_t *)read_identify_device((mmzone_t *)g_first_node->mmzone[2]);
    fat32_boot->bytes_per_sector = get_sector_bytes((phys_addr)ata);
    fat32_boot->sectors_per_cluster = 8; // 这里采用八块一簇 贴合内存分配最小粒度
    fat32_boot->reserved_sectors = 32;   // 一切计算的基准
    fat32_boot->num_fats = 1;
    fat32_boot->root_entries = 0;
    fat32_boot->total_sectors_16 = 0;
    fat32_boot->media_descriptor = 0xf8;
    fat32_boot->sectors_per_fat_16 = 0;
    fat32_boot->sectors_per_track = 0;
    fat32_boot->heads = 0;
    fat32_boot->hidden_sectors = 0;
    fat32_boot->total_sectors_32 = ata->lba28_sectors;
    fat32_boot->sectors_per_fat_32 = ata->lba28_sectors;
    fat32_boot->extended_flags = 0;
    fat32_boot->fs_version = 0;
    fat32_boot->root_cluster = 0;
    fat32_boot->fs_info = 0;
    fat32_boot->backup_boot = 0;
    fat32_boot->drive_number = 0;
    fat32_boot->boot_signature = 0x29;
    fat32_boot->volume_id = 0;
    memcopy((void *)fat32_boot->fs_type, "FAT32   ", 8);
    fat32_boot->signature = 0xaa55;
    write_lba28_sectors((void *)fat32_boot, 0, 1, LBA28_SLAVE_DISK);
    free_pages(zone, blk);
}

void fat32_fill_fat(mmzone_t *zone)
{
    page_t *blk = alloc_pages(zone, 0);
    phys_addr blk_addr = page2phys(zone, blk);
    fat32_bootsector_t *fat32_boot = (fat32_bootsector_t *)blk_addr;

    read_lba28_sectors((void *)fat32_boot, 0, 1, LBA28_SLAVE_DISK);

    uint8_t sectors_per_cluster = fat32_boot->sectors_per_cluster;
    uint32_t total_sectors_32 = fat32_boot->total_sectors_32;

    uint32_t total_cluster = total_sectors_32 / sectors_per_cluster;

    uint16_t reserved_sectors = fat32_boot->reserved_sectors;

    uint32_t reserved_cluster = reserved_sectors / sectors_per_cluster;

    uint32_t fat_start_lba = reserved_sectors;

    uint64_t fat_size = total_cluster * 4;
    uint64_t sectors_per_fat_32 = fat_size / 512;

    uint8_t buddy_order = get_power_two(fat_size) - 12;

    uint32_t fat_occupied_cluster = sectors_per_fat_32 / 8;

    page_t *fat_blk = alloc_pages(zone, buddy_order);
    phys_addr fat_blk_addr = page2phys(zone, fat_blk);
    uint32_t *fat_entry = (uint32_t *)fat_blk_addr;

    for (size_t i = 0; i < reserved_cluster + fat_occupied_cluster + ROOT_DIR_INIT_CLUSTER; i++, fat_entry++)
    {
        *fat_entry = RESERVED_CLUSTER;
    }

    for (size_t i = 0; i < total_cluster - (reserved_cluster + ROOT_DIR_INIT_CLUSTER + fat_occupied_cluster); i++, fat_entry++)
    {
        *fat_entry = FREE_CLUSTER;
    }

    fat32_boot->sectors_per_fat_32 = sectors_per_fat_32;
    fat32_boot->root_cluster = reserved_cluster + fat_occupied_cluster;
    global_fat.flags = 1;
    global_fat.fat_entry_total_count = total_cluster;
    global_fat.global_fat_addr = fat_blk_addr;

    write_more_lba28_sectors((void *)fat_blk_addr, fat_start_lba, sectors_per_fat_32, LBA28_SLAVE_DISK);
    write_lba28_sectors((void *)fat32_boot, 0, 1, LBA28_SLAVE_DISK);

    free_pages(zone, blk);
}

void fat32_fill_fsinfo(mmzone_t *zone)
{
    page_t *blk = alloc_pages(zone, 0);
    phys_addr blk_addr = page2phys(zone, blk);
    fat32_fsinfo_t *fsinfo = (fat32_fsinfo_t *)blk_addr;
    fsinfo->lead_signature = 0x41615252;
    fsinfo->struct_signature = 0x61417272;
    fsinfo->free_count = 0;
    fsinfo->next_free = 0;
    fsinfo->trail_signature = 0xaa550000;
    write_lba28_sectors((void *)fsinfo, 1, 1, LBA28_SLAVE_DISK);
    free_pages(zone, blk);
}

void fat32_mount(mmzone_t *zone)
{
    // 必须先有bootsectors
    fat32_fill_bootsectors(zone);
    fat32_fill_fat(zone);
}

FILE_STATUS get_fat32_status(mmzone_t *zone)
{
    fat_global_init();
    page_t *boot = alloc_pages(zone, 0);
    phys_addr boot_addr = page2phys(zone, boot);
    fat32_bootsector_t *fat32_boot = (fat32_bootsector_t *)boot_addr;
    read_lba28_sectors((void *)fat32_boot, 0, 1, LBA28_SLAVE_DISK);

    uint8_t name[8] = {0};
    memcopy((void *)name, (void *)fat32_boot->fs_type, 8);
    free_pages(zone, boot);

    if (!(memcmp((char *)name, "FAT32   ", 8)))
        return 0;

    return -1;
}