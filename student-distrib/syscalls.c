#include "syscalls.h"
#include "filesystem.h"
#include "rtc.h"
#include "filesystem.h"
#include "terminal.h"
#include "paging.h"
#include "x86_desc.h"
#include "schedule.h"
#include "keyboard.h"

uint8_t processes[MAX_PROCESSES] = {0,0,0,0,0,0};
//uint8_t process_term[MAX_PROCESSES] = {0,0,0,0,0,0};
fop_t empty_fops_table = {empty_function, empty_function, empty_function, empty_function};
fop_t stdin_fops_table = {empty_function, terminal_read, empty_function, empty_function};
fop_t stdout_fops_table = {empty_function, empty_function, terminal_write, empty_function};
fop_t rtc_fops_table = {open_rtc, read_rtc, write_rtc, close_rtc};
fop_t file_fops_table = {open_file, read_file, write_file, close_file};
fop_t dir_fops_table = {open_directory, read_directory, write_directory, close_directory};

/*
 * empty_function
 *
 * DESCRIPTION: An invalid function for functions that shouldn't exist
 *
 * INPUT: None
 * RETURN VALUE: Always returns -1
 */
int32_t empty_function() {
  return -1;
}

/*
 * get_pcb_ptr
 *
 * DESCRIPTION: Gets the pointer to the current pcb
 *
 * INPUT: None
 * RETURN VALUE: Pointer to pcb
 */
pcb_t* get_pcb_ptr(){
    int32_t pcb_ptr;
    asm volatile("          \n\
        movl %%esp, %%eax      \n\
        "
        :"=a"(pcb_ptr)
    );
    return (pcb_t*)(pcb_ptr & PCB_MASK);
}

/*
 * get_pcb_ptr_num
 *
 * DESCRIPTION: Gets the pointer to the current pcb given the pid
 *
 * INPUT: Process id to find a pcb for
 * RETURN VALUE: Pointer to pcb
 */
pcb_t* get_pcb_ptr_num(uint8_t process_num) {
  return (pcb_t*) (_8_MB - (process_num + 1) * _8_KB);
}

/*
 * get_available_process
 *
 * DESCRIPTION: Gets the next available process
 *
 * INPUT: None
 * RETURN VALUE: Next available process
 */
int32_t get_available_process() {
  uint8_t i;
  for (i = 0; i < MAX_PROCESSES; i++) {
    if (!processes[i]) {
      processes[i] = PROCESS_IN_USE;
      return i;
    }
  }
  return -1;
}

/*
 * execute
 *
 * DESCRIPTION: This function accomplishes 6 things:
 *   1. Parse arguments
 *   2. Check if filename is valid and an executable
 *   3. Map new page for process
 *   4. Load the file from fs into memory
 *   5. Setup PCB
 *   6. Context switch
 *
 * INPUT: Command to execute
 * RETURN VALUE: -1 on failure, shouldn't return
 */
int32_t execute(const uint8_t* command){

    cli();

    int i;
    int8_t filename[MAX_FILENAME];
    
    int8_t arg[MAX_ARGS];
    uint8_t elf_buffer[ELF_BUFFER_SIZE];
    uint32_t entry_point;
    int8_t process_num;
    int8_t parent_process_num;

    memset(filename, 0, MAX_FILENAME);
    memset(arg, 0, MAX_ARGS);

    /* Step 1: Parse arguments */
    parseArgs(command, filename, arg);

    /* Check if trying to exit */
    if(strncmp("exit", filename, 4) == 0){
        asm volatile(
            "pushl $0;"
            "pushl $0;"
            "pushl  %%eax;"
            "call halt;"
            :
            );
    }

    /* Step 2: Check if filename is valid and if executable */
    dentry_t dentry_check;
    if (read_dentry_by_name((uint8_t*)filename, &dentry_check) == -1){
        return -1;
    }
    read_data(dentry_check.inode_num, 0, elf_buffer, ELF_BUFFER_SIZE);
    if ((elf_buffer[0] != ASCII_DEL) || (elf_buffer[1] != ASCII_E) ||
      (elf_buffer[2] != ASCII_L) || (elf_buffer[3] != ASCII_F)) {
        return -1;
    }
    read_data(dentry_check.inode_num, ENTRY_POINT, elf_buffer, ELF_BUFFER_SIZE);
    entry_point = *((uint32_t*) elf_buffer);
    process_num = get_available_process();
    if (process_num == -1) {
      return -1;
    }

    /* Step 3: Map new page for new process */
    map_page(_128_MB, _8_MB + process_num * _4_MB);

    /* Step 4: Load the file from fs into memory */
    read_data(dentry_check.inode_num, 0, (uint8_t*) (USER_PAGE_OFFSET), EXCESS_NUM);

    /* Step 5: Setup PCB */
    pcb_t* curr_pcb = get_pcb_ptr_num(process_num);
    curr_pcb->pid = process_num;
    curr_pcb->terminal_id = curr_terminal_id;
    parent_process_num = curr_proc_num;
    curr_proc_num = process_num;
    if (term_proc[curr_terminal_id] == -1) {
      curr_pcb->parent_pcb = curr_pcb;
    } else {
      curr_pcb->parent_pcb = get_pcb_ptr();
    }
    term_proc[curr_terminal_id] = process_num;
    asm volatile("                \n\
      movl %%ebp, %%ebx       \n\
      movl %%esp, %%eax       \n\
      "
      :"=a"(curr_pcb->parent_pcb->esp), "=b"(curr_pcb->parent_pcb->ebp)
    );
    strcpy((int8_t*) curr_pcb->args, (int8_t*) arg);

    for (i = 0; i < MAX_FD; i++) {
      curr_pcb->descriptor_array[i].file_op_ptr = empty_fops_table;
      curr_pcb->descriptor_array[i].inode = INVALID_INODE;
      curr_pcb->descriptor_array[i].file_position = 0;
      curr_pcb->descriptor_array[i].flags = FILE_NOT_IN_USE;
    }
    curr_pcb->descriptor_array[0].file_op_ptr = stdin_fops_table;
    curr_pcb->descriptor_array[1].file_op_ptr = stdout_fops_table;
    curr_pcb->descriptor_array[0].flags = FILE_IN_USE;
    curr_pcb->descriptor_array[1].flags = FILE_IN_USE;

    /* Step 6: Context switch */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * (process_num) - 4;
    sti();
    asm volatile("            \n\
          pushl %0            \n\
          pushl %1            \n\
          pushfl                   \n\
          pushl %2                   \n\
          pushl %3                  \n\
          iret                \n\
          return_from_iret:       \n\
          leave                   \n\
          ret                     \n\
          "
          : /* no outputs */
          : "r" (USER_DS), "r" (USER_ESP), "r" (USER_CS), "r" (entry_point)
      );
  return -1;
}


/*
 * parseArgs
 *
 * DESCRIPTION: helper function for execute, parses command to get filename and arguments
 *
 * INPUT: command, command to be parsed into filename and arg
 * RETURN VALUE: NONE
 */
void parseArgs(const uint8_t* command, int8_t* filename, int8_t* arg){

    int fdcount = 0;
    int fd_idx = 0;

    int argcount = 0;
    int arg_idx = 0;

    int i = 0;
    while (command[i] == ' '){                              // ignore leading spaces
        i++;
    }

    for(fd_idx = i; fd_idx < MAX_ARGS; fd_idx++){                // start at beginning of file name

        //check if command at fd_idx is of value
        if (command[fd_idx] == '\0' || command[fd_idx] == ' ' || command[fd_idx] == '\n'){
            break;
        }
        else{
            fdcount++;
        }
    }

    
    int j;
    for(j = 0; j < fdcount; j++){
        filename[j] = command[j + i];
    }
    //memcpy(filename, command, fdcount);         //copy fdcount bytes of command into filename

    i = fd_idx;
    filename[i] = '\0';

    while(command[i] == ' '){
        i++;
    }

    int argstart = i;

    for(arg_idx = i; arg_idx < MAX_ARGS; arg_idx++){          // start at end of file name

        //check if command is of value
        if (command[arg_idx] == '\0' || command[arg_idx] == ' ' || command[arg_idx] == '\n'){
            break;
        }
        else{
            argcount++;
        }
    }


    for(i = 0; i < argcount; i++){                      //copying argument parsed from command into arg
        arg[i] = command[i + argstart];
    }


    arg[argcount] = '\0';


}

/*
*   Function: open(const uint8_t* filename)
*   Description: find directory entry corresponding to named file, allocate unused file descriptor, and sets up
                 necessary data.
*   input:  filename -> name of file to be opened and put in current process block
*   output: index in file desc array or -1 if filename doesn't exist/no descriptors are free
*   effects: opens file
*/
int32_t
open(const uint8_t* filename){
    int fdesc_idx;
    dentry_t file_dentry;
    pcb_t* process_ctrl_block;

    /* check if file exists */
    if (filename == NULL || read_dentry_by_name(filename, &file_dentry) != 0){
        return -1;
    }

    process_ctrl_block = get_pcb_ptr();

    /* find the first open file descriptor array entry */
    for (fdesc_idx = 2; fdesc_idx <= MAX_FD_NUM; fdesc_idx++){
        if (process_ctrl_block -> descriptor_array[fdesc_idx].flags == FILE_NOT_IN_USE){
            process_ctrl_block -> descriptor_array[fdesc_idx].flags = FILE_IN_USE;
            process_ctrl_block -> descriptor_array[fdesc_idx].file_position = 0;
            break;
        }
    }

    if (fdesc_idx > MAX_FD_NUM){ // couldn't find an available file desc array entry
        return -1;
    }

    switch(file_dentry.file_type){
        case RTC_FILE:
            process_ctrl_block -> descriptor_array[fdesc_idx].inode = NULL;
            process_ctrl_block -> descriptor_array[fdesc_idx].file_op_ptr = rtc_fops_table;
            break;
        case DIRECTORY_FILE:
            process_ctrl_block -> descriptor_array[fdesc_idx].inode = NULL;
            process_ctrl_block -> descriptor_array[fdesc_idx].file_op_ptr = dir_fops_table;
            break;
        case FILE_FILE:
            process_ctrl_block -> descriptor_array[fdesc_idx].inode = file_dentry.inode_num;
            process_ctrl_block -> descriptor_array[fdesc_idx].file_op_ptr = file_fops_table;
            break;
    }

    /* call the corresponding open system call */
    process_ctrl_block -> descriptor_array[fdesc_idx].file_op_ptr.open(filename);

    return fdesc_idx;
}

/*
*   Function: read(int32_t fd, void* buf, int32_t nbytes)
*   Description: calls the read operation for devices, files, and directories
*   input:  fd --> file descriptor
            buf --> buffer to read
            nbytes --> number of bytes that will be returned
*   output: number of bytes read successfuly, -1 on error
*   effects: none
*/
int32_t
read(int32_t fd, void* buf, int32_t nbytes){

    sti();

    /* get current pcb pointer */
    pcb_t* process_ctrl_block = get_pcb_ptr();

    /* sanity checks */
    if (fd < MIN_FD_NUM || fd > MAX_FD_NUM || buf == NULL){
        return -1;
    }
    if (process_ctrl_block -> descriptor_array[fd].flags == FILE_NOT_IN_USE){
        return -1;
    }

    /* return the corresponding read system call */
    return process_ctrl_block -> descriptor_array[fd].file_op_ptr.read(fd, (char*)buf, nbytes);
}

/*
*   Function: write(int32_t fd, const void* buf, int32_t nbytes)
*   Description: calls the write operation for devices, files, and directories
*   input:  fd --> file descriptor
            buf --> buffer to read
            nbytes --> number of bytes that will be returned
*   output: number of bytes written successfuly, -1 on error
*   effects: none
*/
int32_t
write(int32_t fd, const void* buf, int32_t nbytes){
    /* get current pcb pointer */
    pcb_t* process_ctrl_block = get_pcb_ptr();

    /* sanity checks */
    if (fd < MIN_FD_NUM || fd > MAX_FD_NUM || buf == NULL){
        return -1;
    }
    if (process_ctrl_block -> descriptor_array[fd].flags == FILE_NOT_IN_USE){
        return -1;
    }

    /* return the corresponding write system call */
    return process_ctrl_block -> descriptor_array[fd].file_op_ptr.write(fd, buf, nbytes);
}

/*
*   Function: close(int32_t fd)
*   Description: closes specified file desc and makes it available again
*   input:  fd --> file descriptor
*   output: 0 on success, -1 on error
*   effects: closes file
*/
int32_t
close(int32_t fd){
    /* get current pcb pointer */
    pcb_t* process_ctrl_block = get_pcb_ptr();

    /* sanity checks */
    if (fd < 2 || fd > MAX_FD_NUM){
        return -1;
    }
    if (process_ctrl_block -> descriptor_array[fd].flags == FILE_NOT_IN_USE){
        return -1;
    }

    /* make the file desc array entry available again */
    process_ctrl_block -> descriptor_array[fd].file_position = 0;
    process_ctrl_block -> descriptor_array[fd].flags = FILE_NOT_IN_USE;

    /* call corresponding close system call */
    int success = process_ctrl_block->descriptor_array[fd].file_op_ptr.close(fd);

    if (success != 0){
        return -1;
    }
    else{
        return 0;
    }
}

/*
*   Function: halt(uint8_t status)
*   Description: Stops the execution of a process and frees up room in processes
*   inputs: status- value returning to parent process
*   outputs: status value
*   effects: Halts the current processes, remaps paging, and returns to parent proccess
*/
int32_t halt(uint8_t status){
    pcb_t* pcb = get_pcb_ptr(); //check if valid pcb was returned
    if(pcb == NULL)
        return -1;
    int idx;
    for(idx = 2; idx < 8; idx++) { //close all open processes
        close(idx);
    }
    processes[pcb->pid] = PROCESS_NOT_IN_USE;
    if(pcb == pcb->parent_pcb){
        execute((uint8_t*) "shell"); //if there are no other things running, execute shell. shell should always be running
    }
    pcb = pcb->parent_pcb;
    curr_proc_num = pcb->pid;
    term_proc[curr_terminal_id] = pcb->pid;
    map_page(_128_MB, _8_MB + pcb->pid * _4_MB); //remap to parent process


    buffer_clear();

    tss.ss0 = KERNEL_DS; //perform context switch
    tss.esp0 = pcb->esp;
    asm volatile (
        "mov %0, %%esp;"
        "mov %1, %%ebp;"
        "mov %2, %%eax;"
        "jmp return_from_iret;"
        :
        :"r" (pcb->esp), "r" (pcb->ebp), "r" ((uint32_t) status)

    );
    
    return 0;

}


/*
*   Function: getargs(int8_t* buf, int32_t nbytes)
*   Description: This function copys the arguments of a system call into the buffer
*   inputs: buf- where the args are being copied to
*           nbytes- how many bytes are being copied
*   outputs: 0 if sucessful, 1 if failed
*   effects: moves arguments into buffer
*/
int32_t getargs(int8_t* buf, int32_t nbytes){
    if (buf == NULL || nbytes <=0){ //input validation
        //printf("hello");
        return -1;
    }
    pcb_t* process_ctrl_block = get_pcb_ptr();
    if(process_ctrl_block->args[0] == NULL){ //check if args exist
        //printf("hello2");
        return -1;
    }
    memcpy(buf, process_ctrl_block->args, nbytes); //if there are args, copy them to buffer
    return 0;
}

/*
*   Function: vidmap (int8_t** screen_start)
*   Description: This function maps video memory into user space so programs like fish can use it
*   inputs: Double pointer to where the screen begins
*   outputs: Always returns 132 MB value
*   effects: allows users to access video memory
*/
int32_t vidmap (uint8_t** screen_start){
    uint8_t * video_mem_buf;
    if (active_terminal_id == curr_terminal_id) {
      video_mem_buf = (uint8_t *) VIDEO;
    } else {
      video_mem_buf = terminals[curr_terminal_id].video_mem;
    }
    if(screen_start == NULL) //checking for valud parameters
        return -1;
    if(screen_start == (uint8_t**)_4_MB) //make sure start of screen is in bounds
        return -1;
    page_dir[_132_MB / _4_MB] = ((uint32_t) (vidmap_table)) | (page_permission);
    vidmap_table[0] = ((uint32_t) video_mem_buf) | page_permission; //map video memory into user space so fish can use
    flush_tlb();
    *screen_start = (uint8_t*)_132_MB;
    return _132_MB;
}

/*
*   Function: set_handler (int32_t signum, void* handler_address)
*   Description: not implemented yet
*   inputs: not implemented yet
*   outputs: not implemented yet
*   effects: not implemented yet
*/
int32_t set_handler (int32_t signum, void* handler_address){
    //printf("not implemented yet\n");
    return 0;
}

/*
*   sigreturn (void)
*   Description: not implemented yet
*   inputs: not implemented yet
*   outputs: status values
*   effects: not implemented yet
*/
int32_t sigreturn (void){
    //printf("not implemented yet\n");
    return 0;
}

/*
*   Function: int32_t temp (void)
*   Description: not implemented yet
*   inputs: not implemented yet
*   outputs: status values
*   effects: not implemented yet
*/
int32_t temp (void){
    printf("unused\n");
    return 0;
}
