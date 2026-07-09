#include <datatype.h>

#define BLOCK_SIZE_1K
#ifdef BLOCK_SIZE_1K
#define BLOCK_SIZE 1024
#endif

// #define BLOCK_SIZE_2K
#ifdef BLOCK_SIZE_2K
#define BLOCK_SZIE 2048
#endif

// #define BLOCK_SIZE_4K
#ifdef BLOCK_SIZE_4K
#define BLOCK_SZIE 4096
#endif

enum fs_state
{
    FS_STATE_CLEAN = 0,
    FS_STATE_DIRTY,
    FS_STATE_ERROR
};

typedef struct super_block
{
    uint8_t magic_number[8];
    enum fs_state state;
    uint32_t block_size;
    uint64_t total_blocks;
    uint64_t free_blocks;
    uint64_t total_inodes;
} super_block_t;