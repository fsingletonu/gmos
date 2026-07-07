#include <fs/fat32/fat32.h>

phys_addr fat32_open_root_dir(mmzone_t *zone)
{
    page_t *blk = alloc_pages(zone, 0);
    phys_addr blk_addr = page2phys(zone, blk);
    fat32_bootsector_t *root = (fat32_bootsector_t *)blk_addr;
    read_lba28_sectors((void *)root, 0, 1, LBA28_SLAVE_DISK);
}

void fat32_close_root_dir()
{
}

phys_addr fat32_open_dir()
{
}

void fat32_close_dir()
{
}

phys_addr fat32_open_file()
{
}

void fat32_close_file()
{
}