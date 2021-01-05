/*
 * terminal.c --> functions for the Terminal
 */


#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "paging.h"




/*
*   Function: initialize_terminal()
*   Description: initializes the terminal
*   input:  none
*   output: none
*   effects: sets the enter flag to 0
*/

/*
void initialize_terminal() {
    int i, j;
    for (i =0; i < 3; i++){

    }

    active_terminal_id = 0;
    for (i = 0; i < MAX_TERMINALS; i++) {
      curr_terminal_id = i;
      start_terminal(i);
    }
    switch_terminals(0, 0);
}
*/

/*
*   Function: terminal_open(const uint8_t* filename)
*   Description: Opens the terminal
*   input:  none
*   output: none
*   effects: none
*/
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/*
*   Function: terminal_close(int32_t fd)
*   Description: Closes the terminal
*   input:  file descriptor being closed
*   output: none
*   effects: none
*/
int32_t terminal_close(int32_t fd) {
    return 0;
}

/*
*   Function: terminal_read(int32_t fd, void* buf, int32_t nbytes)
*   Description: Reads data from keyboard until enter has been pressed or buffer is full
*   input:  fd --> file descriptor
            buf --> buffer to read
            nbytes --> number of bytes that will be returned
*   output: nbytes
*   effects: clears keyboard buffer
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){

    /* parameter check on buf and nybtes */
    if (buf == NULL || nbytes < 0){
        return -1;
    }

    if(nbytes > 128){
        nbytes = 128;
    }

    /* spins until enter occurs */
    while (terminals[curr_terminal_id].E_flag != 1){
        if (terminals[curr_terminal_id].E_flag == 1){
            break;
        }
    }

    terminals[curr_terminal_id].E_flag = 0;

    //cli(); // begin critical section

    if (nbytes < BUFFER_SIZE - 1){ // if nbytes read is less than max size of buffer
        keyboard_buffer[nbytes - 1] = '\n'; // add newline character to end of buffer
        memcpy(buf, (void*)keyboard_buffer, nbytes); // copy the data in keyboard_buffer to the parameter buf
    }
    else{ // if nbytes is 1 less than the max buffer size (space must be saved for the newline character)
        keyboard_buffer[BUFFER_SIZE-1] = '\n'; // add newline character to end of buffer
        memcpy(buf, (void*)keyboard_buffer, BUFFER_SIZE); // copy the data in keyboard_buffer to the parameter buf
    }

    //sti(); // end critical section

    buffer_clear(); // clear the keyboard_buffer

    return nbytes;
}

/*
*   Function: terminal_write(int32_t fd, const void* buf, int32_t nbytes)
*   Description: writes a string to the terminal
*   input:  fd --> file descriptor
            buf --> buffer to write to
            nbytes --> number of bytes that will be returned
*   output: number of bytes written
*   effects: all data should be displayed to the screen immediately
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {

    /* valid parameter check */
    if (buf == NULL || nbytes < 0){
        return -1;
    }

    int i;
    cli(); // begin critical section

    /* display all non-null bytes to the screen */
    for (i = 0; i < nbytes; i++){
        if (*(uint8_t *)(buf +i) != '\0'){
            putc(*(uint8_t *)(buf + i));
        }
    }

    sti(); // end critical section

    return i;

}

/*
*   Function: start_terminal(uint8_t terminal_id)
*   Description: Starts a new terminal with in input id
*   input:  terminal_id- the id of the terminal being started 
*   output: returns 0 on success 
*   effects: Initializes values in the terminal struct for new terminal
*/
int32_t start_terminal(uint8_t terminal_id){
    cli();
    int j;
    for(j = 0; j < 128; j++){ //clears the buffer
        terminals[terminal_id].buf[j] = '\0';
    }
    terminals[terminal_id].term_id = terminal_id; //set screen position, process pid, enter flag, vidmem etc.
    terminals[terminal_id].screen_x = 0;
    terminals[terminal_id].screen_y = 0;
    terminals[terminal_id].bufferIndex = 0;
    terminals[terminal_id].E_flag = 0;
    terminals[terminal_id].running = 0;
    terminals[terminal_id].pid = -1;
    terminals[terminal_id].video_mem = (uint8_t*) ((0xB9 + terminal_id) << 12);

    //need to figure out paging and vidmem stuff here


    for (j = 0; j < NUM_ROWS * NUM_COLS; j++) { //initalizes the vidmap for the differetn terminals
        *(uint8_t *)(terminals[terminal_id].video_mem + (j << 1)) = ' ';

        if(terminal_id == 0){
            *(uint8_t *)(terminals[terminal_id].video_mem + (j << 1) + 1) = ATTRIB;
        }

        if(terminal_id == 1){
            *(uint8_t *)(terminals[terminal_id].video_mem + (j << 1) + 1) = ATTRIB;
        }

        if(terminal_id == 2){
            *(uint8_t *)(terminals[terminal_id].video_mem + (j << 1) + 1) = ATTRIB;
        }

    }
    terminals[terminal_id].running = 1;
    execute((uint8_t*)"shell"); //starts a shell
    //}
    sti();
    return 0;
}

/*
*   Function: save_terminal(uint8_t terminal_id)
*   Description: Saves essential values of current terminal in the struct
*   input:  terminal_id- the id of the terminal being stored 
*   output: returns 0 on success 
*   effects: Preserves the buffer index and video memory
*/
int32_t save_terminal(uint8_t terminal_id){
    //terminals[terminal_id].screen_x = ret_x();
    //terminals[terminal_id].screen_y = ret_y();
    terminals[terminal_id].bufferIndex = bufferIndex; //store the current buffer index for when we switch back to this terminal

    memcpy((uint8_t *)terminals[terminal_id].video_mem,(uint8_t*)VIDEO, _4_KB); //copy current video mem into struct

    return 0;
}

/*
*   Function: restore_terminal(uint8_t terminal_id)
*   Description: Moves values from struct to current local variables 
*   input:  terminal_id- the id of the terminal being switched to 
*   output: returns 0 on success 
*   effects: Sets up all values for the new terminal
*/
int32_t restore_terminal(uint8_t terminal_id){
    uint8_t temp_terminal_id = curr_terminal_id;
    bufferIndex = terminals[terminal_id].bufferIndex; //restores bufferidx
    sti();
    curr_terminal_id = active_terminal_id;
    set_screen_position(terminals[terminal_id].screen_x, terminals[terminal_id].screen_y, 0); //sets new cursor
    curr_terminal_id = temp_terminal_id; 
    cli();
    memset((void *)VIDEO, '\0', _4_KB); //clears vidmem and sets new values
    memcpy((uint8_t*)VIDEO, (uint8_t *)terminals[terminal_id].video_mem, _4_KB);

    return 0;
}

/*
*   Function: switch_terminals(uint8_t old_terminal_id, uint8_t new_terminal_id)
*   Description: Saves current terminal values and restores the values for new terminals
*   input:  old_terminal_id - terminal being switched being away from
*           new_terminal_id - terminal being switched to
*   output: returns 0 on success 
*   effects: Sets up all values for the new terminal saves old terminal values
*/
int32_t switch_terminals(uint8_t old_terminal_id, uint8_t new_terminal_id){
    save_terminal(old_terminal_id); //saves all current values to struct
    active_terminal_id = new_terminal_id;
    //curr_terminal_id = new_terminal_id;
    restore_terminal(new_terminal_id); //restores all values for new terminal
    keyboard_buffer = terminals[new_terminal_id].buf;
    return 0;
}
