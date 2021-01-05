#include "lib.h"
#include "filesystem.h"
#include "syscalls.h"

uint8_t* boot_block_addr;
uint8_t* cur_dentry_addr;
uint8_t* cur_inode_addr;
uint8_t* cur_data_addr;
//uint32_t dentry_count;
dentry_t dir_entry[63];
file_sys_t boot_data;
/*
 * init_filesystem
 *
 * DESCRIPTION: initializes all global variables and file system structures
 *
 * INPUT: starting_addr - the address where the filesystem starts
 * OUTPUT: none
 * RETURN VALUE: none
 *
 * SIDE EFFECTS: initializes all of the global variables
 */
void init_filesystem(uint32_t starting_addr){
    /* Copy boot block addr into local global var */
    boot_block_addr = (uint8_t*) starting_addr;
    /* Calculate starting address of directory entries */
    cur_dentry_addr = boot_block_addr + DENTRY_SIZE;
    /* Calculate starting address of inodes */
    cur_inode_addr = boot_block_addr + BLOCK_SIZE;
    //dentry_count = 0;
    /* Copy data entries into local data entry array */
    memcpy(dir_entry, (void*) boot_block_addr + DENTRY_SIZE, BLOCK_SIZE - DENTRY_SIZE);
    /* Get total number of directory entries */
    memcpy(&boot_data.num_dentry, (void *) boot_block_addr, sizeof(boot_data.num_dentry));
    /* Get total number of inodes */
    memcpy(&boot_data.num_inodes, ((void*) boot_block_addr) + sizeof(boot_data.num_dentry), sizeof(boot_data.num_inodes));
    /* Get total number of data blocks */
    memcpy(&boot_data.num_datablocks, ((void*) boot_block_addr) + sizeof(boot_data.num_dentry) + sizeof(boot_data.num_inodes), sizeof(boot_data.num_datablocks));
    /* Get starting address of the data blocks */
    cur_data_addr = cur_inode_addr + BLOCK_SIZE * boot_data.num_inodes;
}


/*
 * read_dentry_by_index
 *
 * DESCRIPTION: Given the index of a directory entry, find it and return it
 *
 * INPUT: index - the dentry struct your are looking for in the array
 *        dentry - the struct where the information will be compied to
 * OUTPUT: 0 if sucessful, -1 if fails
 * RETURN VALUE: success or failure
 *
 * SIDE EFFECTS: copies one of the dentry structs
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry){
    if(index >= boot_data.num_dentry){ //if index is out of bounds, can't be found
        return -1;
  }
    else{ //if index is in range, copy the data
      *dentry = dir_entry[index];
      return 0;
  }
}
/*
 * read_dentry_by_name
 *
 * DESCRIPTION: Given a directory name, find its directory entry in the filesystem
 *
 * INPUT: fname - the directory name to look for
 *        dentry - the struct where the information will be compied to
 * OUTPUT: 0 if sucessful, -1 if fails
 * RETURN VALUE: success or failure
 *
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    if (dentry == NULL || strlen((int8_t*)fname) > 32 || fname == NULL || fname[0] == (uint8_t) '\0'){
        return -1;
    }
        
    int i;
    /* Go through directory entries, but not the 0th one because it is the boot block */
    for (i = 0; i < 63; i++) {
      if (!strncmp((int8_t*) fname, (int8_t*) dir_entry[i].file_name, FILE_NAME_LENGTH)) {
        //printf("File name: %s\n", (int8_t*) dir_entry[i].file_name);
        memcpy(dentry, &dir_entry[i], sizeof(dentry_t));
        return 0;
      }
    }
    return -1;
}

/*
 * read_data
 *
 * DESCRIPTION: Read and copy data of a specific inode into a buffer given an offset and length
 *
 * INPUT: inode - inode that has information on the data that we want
 *        offset - data that we want to begin reading from
 *        buf - buffer to which data is copied to
 *        length - number of bytes of data that need to be copied
 * OUTPUT: 0 if sucessful, -1 if fails
 * RETURN VALUE: bytes copied into buffer
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
  //printf("Reading data...\n");
  if (inode < 0 || inode > boot_data.num_inodes) {
    return -1;
  }
  /* Get starting address of inode */
  uint32_t* inode_addr = (uint32_t*) (cur_inode_addr + inode * BLOCK_SIZE);
  uint32_t data_bytes = *inode_addr;
  //printf("Size: %d\n", data_bytes);
  inode_addr++;
  /* Don't go over file length */
  if (offset >= data_bytes) {
    return 0;
  } else if (length + offset >= data_bytes) {
    length = data_bytes - offset;
  }
  /* Which data block are we starting from */
  uint32_t inode_dblock_idx_begin = offset / BLOCK_SIZE;
  uint32_t inode_dblock_idx_end = (offset + length) / BLOCK_SIZE;
  uint32_t dblock_idx = 0;
  uint32_t bytes_copied = 0;

  uint32_t dblock_begin;
  uint32_t dblock_end;
  uint32_t i;
  /* Iterate through the data blocks */
  for (i = inode_dblock_idx_begin; i <= inode_dblock_idx_end; i++) {
    dblock_idx = *(inode_addr + i);
    //printf("index %d: %d\n", i, dblock_idx);
    dblock_begin = 0;
    dblock_end = BLOCK_SIZE;
    /* Figure out which data block is being read */
    if (i * BLOCK_SIZE < offset) {
      dblock_begin = offset - i * BLOCK_SIZE;
    }
    if ((i + 1) * BLOCK_SIZE > offset + length) {
      dblock_end = offset + length - i * BLOCK_SIZE;
    }
    /* Copy into buffer */
    memcpy((int8_t*) buf + bytes_copied,
      (int8_t*) cur_data_addr + BLOCK_SIZE * dblock_idx + dblock_begin,
      dblock_end - dblock_begin);
    bytes_copied += dblock_end - dblock_begin;
  }
  return bytes_copied;
}

/*
 * read_file
 *
 * DESCRIPTION: If the name of the input file exists, this function uses the read data function to output the files
 *              data into the buffer
 *
 * INPUT: fname - name of the file we want to read
 *        offset - data that we want to begin reading from
 *        buf - buffer to which data is copied to
 *        nbytes - number of bytes of data that need to be copied
 * OUTPUT: output of read_data function
 * RETURN VALUE: number bytes copied into buffer
 */
int32_t read_file(int32_t fd, void* buf, int32_t nbytes){
  //dentry_t temp;
  pcb_t* pcb = get_pcb_ptr();

  uint32_t offset = pcb->descriptor_array[fd].file_position;
  /*
  if(read_dentry_by_name(fname, &temp) == -1) { //checks if file we want to open exists
    return -1;
  }
  */
  if (pcb->descriptor_array[fd].inode == -1) {
    return -1;
  }
  //printf("Inode num: %d\n", temp.inode_num);
  int32_t bytes_read = read_data(pcb->descriptor_array[fd].inode, offset, buf, nbytes); //return number of bytes read if it is valid
  pcb->descriptor_array[fd].file_position += bytes_read;
  return bytes_read;
}


/*
 * read_directory
 *
 * DESCRIPTION: compiles the filenames for all of the folders/files in the current directory
 *
 * INPUT: buf - where the characters of the file name are copied
 *        nbytes - the maximum bytes that can be sent to the buffer
 * OUTPUT: none
 * RETURN VALUE: the size of the filename in the buffer
 *
 * SIDE EFFECTS: fills buffer with the filenames
 */
int32_t read_directory(int32_t fd, void* buf, int32_t nbytes){
    int i;
    int length;
    int ret_val  = 0;
    pcb_t * pcb = get_pcb_ptr();
    uint32_t dentry_count = pcb->descriptor_array[fd].file_position;
    if(dentry_count == boot_data.num_dentry){ //if all the directories have been read, return
      return 0;
    }

    length = strlen((int8_t*) dir_entry[dentry_count].file_name);
    if (nbytes > length) //if len isn't bigger than copy size we can reduce copy size
      ret_val = length;
    else
      ret_val = nbytes;
    if(ret_val > FILE_NAME_LENGTH) //if length is greater than max buffer size it will have to be truncated
      ret_val = FILE_NAME_LENGTH;

    for(i= 0; i < ret_val; i++) { //move chars into the buffer one by one
      ((uint8_t*) buf)[i] = dir_entry[dentry_count].file_name[i];
    }
    dentry_count++; //interate dentry_count and return length of buffer
    pcb->descriptor_array[fd].file_position = dentry_count;
    return ret_val;
}

/*
 * write_file
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns -1
 *
 * SIDE EFFECTS: none
 */
int32_t write_file (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/*
 * write_directory
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns -1
 *
 * SIDE EFFECTS: none
 */

int32_t write_directory (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/*
 * open_file
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns 0
 *
 * SIDE EFFECTS: none
 */
int32_t open_file (const uint8_t* file_name){
    return 0;
}

/*
 * open_directory
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns 0
 *
 * SIDE EFFECTS: none
 */
int32_t open_directory (const uint8_t* file_name){
    return 0;
}

/*
 * close_file
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns 0
 *
 * SIDE EFFECTS: none
 */
int32_t close_file (int32_t fd){
    return 0;
}

/*
 * close_directory
 *
 * DESCRIPTION: does nothing
 *
 * INPUT: None
 * OUTPUT: not significant
 * RETURN VALUE: always returns 0
 *
 * SIDE EFFECTS: none
 */
int32_t close_directory (int32_t fd){
    return 0;
}
