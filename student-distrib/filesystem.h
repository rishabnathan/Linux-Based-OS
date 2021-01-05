#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "lib.h"
#include "types.h"
#define FT_RTC_ACCESS 0
#define FT_DIRECTORY 1
#define FT_FILE 2

#define BLOCK_SIZE 4096
#define DENTRY_SIZE 64
#define BOOT_BLOCK_RESERVED_SIZE 52
#define FILE_NAME_LENGTH 32
#define DENTRY_RESERVED_SIZE 24



typedef struct file_sys{
    uint32_t num_dentry;
    uint32_t num_inodes;
    uint32_t num_datablocks;
    uint8_t reserved[BOOT_BLOCK_RESERVED_SIZE];
} file_sys_t;

typedef struct file_descriptor{
    uint32_t file_operations_ptr;
    uint32_t inode_num;
    uint32_t file_position;
    uint32_t flags;
} file_descriptor_t;

typedef struct dentry{
    uint8_t file_name[FILE_NAME_LENGTH];
    uint32_t file_type;
    uint32_t inode_num;
    uint8_t reserved[DENTRY_RESERVED_SIZE];
} dentry_t;

typedef struct inode{
  /* First 4 bytes is for len, rest is for dblock_indices */
    uint32_t len;
    uint32_t block_idx[BLOCK_SIZE / 4 - 1];
} inode_t;



void init_filesystem(uint32_t starting_addr);
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes);
int32_t write_file (int32_t fd, const void* buf, int32_t nbytes);
int32_t write_directory (int32_t fd, const void* buf, int32_t nbytes);
int32_t open_file (const uint8_t* file_name);
int32_t open_directory (const uint8_t* file_name);
int32_t close_file (int32_t fd);
int32_t close_directory (int32_t fd);
#endif
