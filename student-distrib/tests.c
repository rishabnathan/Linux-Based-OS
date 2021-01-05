#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "terminal.h"
#include "filesystem.h"


#define PASS 1
#define FAIL 0

#define FRAME0_SIZE 187
#define FRAME1_SIZE 187
#define LARGE_SIZE 5277
#define CAT_SIZE 5445
#define FISH_SIZE 36164

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* rtc_test
 *
 * Enable RTC and test if it is recieving interrupts
 * Input: None.
 * Output: prints a flashing of characters on the screen
 * Side Effects: none
 * Coverage: RTC
 * Files: rtc.c
 */
int rtc_test(){
	TEST_HEADER;

	while(1){
		test_interrupts();
	}
	return PASS;
}

// add more tests here

/* test_divide_error
 * DESCRIPTION: Dividing by zero, should cause divide exception
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int test_divide_error(){
	TEST_HEADER;
	int test_ret;
	int div = 0;
	test_ret = 1/div;
	div = test_ret;
	return PASS;

}
/* invalid_opcode_test
 * DESCRIPTION: Calls invalid instruction which triggers exception
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int invalid_opcode_test() {
	TEST_HEADER;
	asm (
		"rsm"
	);
	return PASS;
}

// add more tests here
/* overflow_test
 * DESCRIPTION: Creates an overflow in the eax register
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int overflow_test() {
	TEST_HEADER;
	asm (
		"movl $0x80000000, %eax;"
		"addl %eax, %eax;"
		"into;"
	);
	return PASS;
}

/* deref_vid_mem
 * DESCRIPTION: Dereference an address in the video memory range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_vid_mem() {
	TEST_HEADER;
	int* test_ptr = (int*) (VIDEO + 0x20);	// Random address
	int test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_lower_vid_mem_edge_pass
 * DESCRIPTION: Dereference an address just inside the lower video memory range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_lower_vid_mem_edge_pass() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (VIDEO);	// Should pass
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_lower_vid_mem_edge_pass_fault
 * DESCRIPTION: Dereference an address just outside the lower video memory range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_lower_vid_mem_edge_fault() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (VIDEO - 0x1);	// Should pass
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return FAIL;
}

/* deref_upper_vid_mem_edge_pass
 * DESCRIPTION: Dereference an address just inside the upper video memory range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_upper_vid_mem_edge_pass() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (VIDEO + 0xFFF);	// Should pass
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_upper_vid_mem_edge_fault
 * DESCRIPTION: Dereference an address just outside the upper video memory range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_upper_vid_mem_edge_fault() {
	TEST_HEADER;
	int* test_ptr = (int*) (VIDEO + 0x1000);	// Should fail
	int test_var = *test_ptr;
	test_var = test_var;
	return FAIL;
}

/* deref_kernel
 * DESCRIPTION: Dereference an address in the kernel range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_kernel() {
	TEST_HEADER;
	int* test_ptr = (int*) (KERNEL_ADDRESS + 0x2000);	// Random address
	int test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_lower_kernel_edge_pass
 * DESCRIPTION: Dereference an address just inside the lower kernel range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_lower_kernel_edge_pass() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (KERNEL_ADDRESS);	// Random address
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_lower_kernel_edge_fault
 * DESCRIPTION: Dereference an address just outside the lower kernel range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_lower_kernel_edge_fault() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (KERNEL_ADDRESS - 0x1);	// Random address
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return FAIL;
}

/* deref_upper_kernel_edge_pass
 * DESCRIPTION: Dereference an address just inside the upper kernel range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_upper_kernel_edge_pass() {
	TEST_HEADER;
	uint8_t* test_ptr = (uint8_t*) (KERNEL_ADDRESS + 0x3FFFFF);	// Random address
	uint8_t test_var = *test_ptr;
	test_var = test_var;
	return PASS;
}

/* deref_upper_kernel_edge_fault
 * DESCRIPTION: Dereference an address just outside in the upper kernel range with paging on
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_upper_kernel_edge_fault() {
	TEST_HEADER;
	int* test_ptr = (int*) (KERNEL_ADDRESS + 0x400000);	// Random address
	int test_var = *test_ptr;
	test_var = test_var;
	return FAIL;
}

/* deref_undefined_mem
 * DESCRIPTION: Dereference an address in an undefined range with paging
 * on. Should trigger a page fault exception.
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int deref_undefined_mem() {
	TEST_HEADER;
	int* test_ptr = (int*)(KERNEL_ADDRESS << 2);	// Random address
	int test_var = *test_ptr;
	test_var = test_var;
	return FAIL;
}

/* Checkpoint 2 tests */


/* read_directory_test
 * DESCRIPTION: Read the directory entries in the filesystem
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
int read_directory_test(){
	uint8_t buf[32];
	uint32_t num_chars = 0;

	while (1) {
		num_chars = read_directory(0, buf, 32);
		if(num_chars == -1)
			break;
		int i;

		printf("File name: ");
		for (i = 0; i < num_chars; i++) {
			putc(buf[i]);
		}
		putc('\n');
	}
	return PASS;
}

/*
* read_text_test_0
 * DESCRIPTION: Read a text file, specifically "frame0.txt"
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 *
int read_text_test_0() {
	TEST_HEADER;

	uint8_t buf[BLOCK_SIZE];
	uint32_t num_chars = read_file((uint8_t*)"frame0.txt", 0, buf, FRAME0_SIZE);

	if (num_chars <= 0)
		return FAIL;

	int i;
	for (i = 0; i < num_chars; i++) {
		putc(buf[i]);
	}
	return PASS;
}

* read_text_test_1
 * DESCRIPTION: Read a text file, specifically "verylargetextwithverylongname.txt"
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 *
int read_text_test_1() {
	TEST_HEADER;

	uint8_t buf[BLOCK_SIZE * 2];
	uint32_t num_chars = read_file((uint8_t*)"verylargetextwithverylongname.tx", 0, buf, LARGE_SIZE);

	if (num_chars <= 0)
		return FAIL;

	int i;
	for (i = 0; i < num_chars; i++) {
		putc(buf[i]);
	}
	return PASS;
}

* read_executable_test_0
 * DESCRIPTION: Read an executable file, specifically "cat"
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 *
int read_executable_test_0() {
	TEST_HEADER;

	uint8_t buf[BLOCK_SIZE * 2];
	uint32_t num_chars = read_file((uint8_t*)"cat", 0, buf, CAT_SIZE);

	if (num_chars <= 0)
		return FAIL;

	int i;
	for (i = 0; i < 100; i++) {
		putc(buf[i]);
	}
	return PASS;
}

* read_partial_text_test_0
 * DESCRIPTION: Read a text file, but only a part of it to test the offset
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 *
int read_partial_text_test_0() {
	TEST_HEADER;

	uint8_t buf[BLOCK_SIZE];
	uint32_t num_chars = read_file((uint8_t*)"frame1.txt", 96, buf, FRAME1_SIZE - 45);

	if (num_chars <= 0)
		return FAIL;

	int i;
	for (i = 0; i < num_chars; i++) {
		putc(buf[i]);
	}
	return PASS;
}
*/

int term_read_test(){
	TEST_HEADER;

	char buf[200];


	/* Test invalid parameters */
	//char* ptr = NULL;
	//if(terminal_read(0, ptr, 5) != -1) return FAIL;


	/* Make sure to type at least 10 chars */
	//memset(buf, 0, 200);
	//if(terminal_read(0, buf, 10) != 10) return FAIL;

	//if(buf[9] != '\n') return FAIL;

	/* Make sure to type a full buffer */
	/*
	memset(buf, 0, 200);
	if(terminal_read(0, buf, 128) != 128){
		printf("hello");
		return FAIL;
	}
	if(buf[127] != '\n'){
		printf("hi 2");
		return FAIL;
	}
	*/
	/* Make sure to type a full buffer */
	memset(buf, 0, 200);
	if(terminal_read(0, buf, 200) != 128) return FAIL;
	if(buf[127] != '\n') return FAIL;

	printf("Passed!\n");
	return PASS;
}

int term_write_test() {
	TEST_HEADER;
	/* check for appropriate return values of bytes typed */

	if (terminal_write(0, "asdf", 4) != 4){
		printf("hi 1");
		return FAIL;
	}

	if (terminal_write(0, "asdf", 3) != 3){
		printf("hi 2");
		return FAIL;
	}

	if (terminal_write(0, "asdf", 5) != 4) return FAIL;

	if (terminal_write(0, "asdf", -1) != -1) return FAIL;

	if (terminal_write(0, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", 200) != 200)
		return FAIL;

	printf("\nPassed!\n");
	return PASS;
}

int term_readwrite_test(){
	TEST_HEADER;

	uint8_t buffer[200];
	while(1){
		memset(buffer, 0, 200);

		terminal_write(0, buffer, terminal_read(0, buffer, 200));
	}
}

int rtc_freq_test(){
	TEST_HEADER;
	uint32_t test_freq = 2;
	open_rtc(0);
	while(test_freq <= 1024){
		int i;
		for (i = 0; i < 100; i++){
			read_rtc(0, &test_freq, 0);
			test_interrupts();
		}
		test_freq <<= 1;
		write_rtc(0, &test_freq, 4);
	}
	return PASS;
}
/* read_large_text_test_0
 * DESCRIPTION: Read a large file so that we can make sure multiple
 * data blocks are accessed properly
 * INPUTS: none
 * RETURN VALUE: PASS/FAIL
 */
 /*
int read_large_text_test_0() {
	TEST_HEADER;



	uint8_t buf[BLOCK_SIZE * 10];
	uint32_t num_chars = read_file((uint8_t*)"fish", 0, buf, FISH_SIZE);

	if (num_chars <= 0)
		return FAIL;

	int i;
	for (i = 0; i < num_chars; i++) {
		putc(buf[i]);
	}
	return PASS;
}
*/

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
 void launch_tests(){
 	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("divide zero", test_divide_error());
	//TEST_OUTPUT("test inv opecode", invalid_opcode_test());
	//TEST_OUTPUT("test overflow", overflow_test());
	//TEST_OUTPUT("rtc_test", rtc_test());
	/*
	//TEST_OUTPUT("deref vid mem range", deref_vid_mem());
	//TEST_OUTPUT("deref inside upper vid mem range", deref_upper_vid_mem_edge_pass());
	//TEST_OUTPUT("deref inside lower vid mem range", deref_lower_vid_mem_edge_pass());
	//TEST_OUTPUT("deref outside upper vid mem range", deref_upper_vid_mem_edge_fault());
	//TEST_OUTPUT("deref outside lower vid mem range", deref_upper_vid_mem_edge_fault());
	*/
	// TEST_OUTPUT("deref inside kernel mem range", deref_kernel());
	// TEST_OUTPUT("deref inside upper kernel mem range", deref_upper_kernel_edge_pass());
	// TEST_OUTPUT("deref inside lower kernel mem range", deref_lower_kernel_edge_pass());
	//TEST_OUTPUT("deref outside upper kernel mem range", deref_upper_kernel_edge_fault());
	//TEST_OUTPUT("deref outside lower kernel mem range", deref_lower_kernel_edge_fault());
	//TEST_OUTPUT("Dereferencing an address outside of first 8 MB", deref_undefined_mem());

	//TEST_OUTPUT("terminal read test", term_read_test());
	//TEST_OUTPUT("terminal write test", term_write_test());
	//TEST_OUTPUT("terminal read/write test", term_readwrite_test());
	//TEST_OUTPUT("rtc freq test", rtc_freq_test());
 	// launch your tests here
	//TEST_OUTPUT("read_directory_test", read_directory_test());
	//TEST_OUTPUT("read_text_test_0", read_text_test_0());
	//TEST_OUTPUT("read_text_test_1", read_text_test_1());
	//TEST_OUTPUT("read_executable_test_0", read_executable_test_0());
	//TEST_OUTPUT("read_partial_text_test_0", read_partial_text_test_0());
	//TEST_OUTPUT("read_large_text_test_0", read_large_text_test_0());
	 // launch your tests here
	/*
	 uint8_t buf[BLOCK_SIZE * 2];
	 uint32_t num_chars = read_file((uint8_t*)"verylargetextwithverylongname.tx", 0, buf, CAT_SIZE);

	 if (num_chars <= 0)
		 return;

	 int i;
	 for (i = 0; i < 100; i++) {
		 putc(buf[i]);
	 }
	 */

 }
