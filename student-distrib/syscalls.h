#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "types.h"

#define PCB_MASK    0xFFFFE000

#define ELF_BUFFER_SIZE 4
#define ENTRY_POINT 24
#define MAX_PROCESSES 6
#define MAX_FD 8
#define USER_ESP 0x83FFFFC
#define page_permission 0x07
#define EXCESS_NUM 10000
#define MAX_FILENAME 128
#define MAX_ARGS 128

#define RTC_FILE 0
#define DIRECTORY_FILE 1
#define FILE_FILE 2

#define MIN_FD_NUM 0
#define MAX_FD_NUM 7

#define MAX_TERMINALS 3

#define PROCESS_IN_USE 1
#define PROCESS_NOT_IN_USE 0

#define INVALID_INODE -1
#define FILE_IN_USE 1
#define FILE_NOT_IN_USE 0


typedef struct file_operations{
    int32_t (*open) (const uint8_t * file_name);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*close) (int32_t fd);
} fop_t;

typedef struct file_desc{
    fop_t file_op_ptr;
    int32_t inode;
    uint32_t file_position;
    uint32_t flags;
} file_desc_t;

typedef struct pcb{
    file_desc_t descriptor_array[MAX_FD_NUM + 1]; //8 files open at an given time
    int32_t esp; //processes esp
    uint32_t ebp;
    struct pcb* parent_pcb; //pointer parent task's pcb
    uint8_t args[MAX_ARGS];
    uint8_t pid;
    uint8_t terminal_id;
    int32_t rtc_freq;
    int32_t rtc_counter;
} pcb_t;

/* Syscall Function Definitions */

uint32_t curr_proc_num;

int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t open(const uint8_t* filename);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t close(int32_t fd);
int32_t getargs(int8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
int32_t temp (void);

void parseArgs(const uint8_t* command, int8_t* filename, int8_t* arg);
pcb_t* get_pcb_ptr();
pcb_t* get_pcb_ptr_num(uint8_t);
int32_t get_available_process();
int32_t empty_function();

#endif
