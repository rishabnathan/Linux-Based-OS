/*
 * keyboard.c --> functions for the Keyboard
 */

#include "keyboard.h"
#include "terminal.h"
#include "i8259.h"

#include "lib.h"



volatile uint8_t isShift;
volatile uint8_t isCaps;
static uint8_t isAlt;
volatile uint8_t isCtrl;
volatile uint8_t isTab;




// arrays to translate hexadecimal values from keyboard to characters
static uint8_t scancodes00[NUM_KEYS] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
     'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
     'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};

static uint8_t scancodes01[NUM_KEYS] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
     'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
     'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

static uint8_t scancodes10[NUM_KEYS] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
     'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
     'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'};

static uint8_t scancodes11[NUM_KEYS] = {'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
     'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v',
     'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'};

/* initialize_keyboard
 *
 *      DESCRIPTION: Initialization for keyboard
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: enables interrupt on pic
 */
void initialize_keyboard(){
    enable_irq(KEYBOARD_IRQ); //enables keyboard on pic

    keyboard_buffer = terminals[active_terminal_id].buf;
    buffer_clear();

}

/* keyboard_handler
 *
 *      DESCRIPTION: handles keyboard interrupts, gets data from keyboard
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: sends keyboard data to be translated to char
 */
void keyboard_handler(){
    cli();
    disable_irq(KEYBOARD_IRQ);
    sti();
    uint8_t input = 0;
    input = inb(KEYBOARD_PORTNUM) & 0xFF; //get data from keyboard and masking for lower 8 bits
    cli();
    enable_irq(KEYBOARD_IRQ);
    sti();

    send_eoi(KEYBOARD_IRQ); //end of interrupt signal
    if(input == LEFTSHIFTD || input == RIGHTSHIFTD){        //check is left shift or right shift are pressed down
        isShift = 1;
    } else if(input == LEFTSHIFTU || input == RIGHTSHIFTU){ //check is left shift or right shift are pressed up
        isShift = 0;
    } else if(input == CAPS){   //check if caps is toggled

        if(isCaps == 1){        //toggle caps
            isCaps = 0;
        } else {
            isCaps = 1;
        }
    } else if(input == BACKSPACE){  //check if backspace
        backspace_handler();
    } else if(input == ENTER){  //check if enter
        enter_handler();
    } else if(input == CTRLD){ //check if ctrl press downed
        isCtrl = 1;
    } else if(input == CTRLU){  //check if ctrl press up
        isCtrl = 0;
    } else if(input == ALTD){ //check if alt pressed down
        isAlt = 1;

    } else if(input == ALTU){ //check if alt pressed up
        isAlt = 0;
    } else if(input == F1){
        if(isAlt == 1){
            switch_terminals(active_terminal_id, 0);
            //putc('h');
        }
    } else if(input == F2){
        if(isAlt == 1){
            switch_terminals(active_terminal_id, 1);
        }
    } else if(input == F3){
        if(isAlt == 1){
            switch_terminals(active_terminal_id, 2);
        }
    } else {

        if(input == 0x0F){ //0x0F is ascii value for tab, replace input with space value
            input = 0x39; //0x39 is space value
        }
        get_char(input); //helper function to get char
    }

}

/* get_char
 *
 *      DESCRIPTION: helper function to access scancodes array
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: NONE
 */
void get_char(uint8_t scancode_data){
    uint8_t temp_terminal_id = 0;
    //if hex value is outside of array, do nothing
    if (scancode_data >= NUM_KEYS){
        return;
    }

    //get char to be echoed
    uint8_t character;
    if(isCaps == 0 && isShift == 0){                //char is obtaineed based off shift and caps buttons
        character = scancodes00[scancode_data];
    } else if(isCaps == 0 && isShift == 1){
        character = scancodes01[scancode_data];
    } else if(isCaps == 1 && isShift == 0){
        character = scancodes10[scancode_data];
    } else if(isCaps == 1 && isShift == 1){
        character = scancodes11[scancode_data];
    }


    if (character == NULL){                         //dont do anything with null bytes
        return;
    }

    if(isCtrl == 1){                                //clear screen if ctrl+l or ctrl+L
        if(character == 'l' || character == 'L'){
            cli();
            temp_terminal_id = curr_terminal_id;
            curr_terminal_id = active_terminal_id;
            shell_clear();
            curr_terminal_id = temp_terminal_id;
            sti();
            //set_screen_position(0,0,0);

        } else if (character == 'c'){               //quit with ctrl+c
            return;
        }
    } else if(bufferIndex < BUFFER_SIZE - 1){
        buffer_insert(character);                   //add char to buffer
        cli();
        temp_terminal_id = curr_terminal_id;
        curr_terminal_id = active_terminal_id;
        putc(character);                            //echoes to screen
        curr_terminal_id = temp_terminal_id;
        sti();


    }

    return;

}

/* buffer_clear()
 *
 *      DESCRIPTION: helper function to clear keyboard_buffer
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: clears keyboard buffer
 */
void buffer_clear(){



    uint8_t i;

    for(i = 0; i < BUFFER_SIZE - 1; i++){
        keyboard_buffer[i] = '\0';               //puts empty space in buffer
    }

    bufferIndex = 0;
}

/* buffer_insert
 *
 *      DESCRIPTION: inserts character into repsective terminal keyboard buffer
 *      INPUTS: character, char to be inserted
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: keyboard buffer and buffer index is updated
 */
void buffer_insert(uint8_t character){
    if(bufferIndex < BUFFER_SIZE - 1){

        if(bufferIndex == 80){                      //if buffer reaches right side of screen (80), scroll up
        //keyboard_buffer[bufferIndex++] = '\n';
        //putc('\n');
        //vertical_scroll();
        }

        keyboard_buffer[bufferIndex] = character;
        bufferIndex++;
    }

}

/* enter_handler()
 *
 *      DESCRIPTION: helper function that handles enter key
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: adds newline to buffer
 */
void enter_handler(){
    uint8_t temp_terminal_id = 0;
    temp_terminal_id = curr_terminal_id;
    curr_terminal_id = active_terminal_id;
    putc('\0');    //set screen when enter is pressed
    curr_terminal_id = temp_terminal_id;
    sti();
    if(keyboard_buffer[0] != '\0'){
        keyboard_buffer[bufferIndex++] = '\n';          //adds newline to buffer
    }


    uint8_t i;

    for(i = bufferIndex; i < BUFFER_SIZE; i++){
        keyboard_buffer[i] = '\0';               //puts empty space in buffer
    }

    terminals[active_terminal_id].E_flag = 1;

    cli();
    temp_terminal_id = curr_terminal_id;
    curr_terminal_id = active_terminal_id;
    set_screen_position(0, 0, 1);                   //set screen when enter is pressed
    curr_terminal_id = temp_terminal_id;
    sti();

    bufferIndex = 0;                                //clearing buffer


}

/* backspace_handler()
 *
 *      DESCRIPTION: helper function that handles backspace key
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: clears last index, decrements buffer index
 */
void backspace_handler(){
    if(bufferIndex > 0){
        //handle backspace on terminal to remove character from screen
        backspace();
        bufferIndex--;
        keyboard_buffer[bufferIndex] = '\0';
    }
}
