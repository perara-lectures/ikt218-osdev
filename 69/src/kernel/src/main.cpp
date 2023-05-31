#include "memory/pit.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "descriptor_tables.h"
#include "interrupts.h"
#include "drivers/keyboard/keyboard.h"
#include "common.h"
#include "drivers/keyboard/timer.h"
#include <cstdlib>
#include "memory/paging.h"
#include "../../boot/src/boot.h"
#include "memory/kmalloc.h"
#include "system.h"
#include "print.h"



#define BUFSIZE 2200
// #define VGA_ADDRESS 0xB8000

// Define entry point in asm to prevent C++ mangling
extern "C"{
    void kernel_main();
}


#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif







enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static uint32_t next_line_index = 1;
uint8_t g_fore_color = VGA_COLOR_WHITE, g_back_color = VGA_COLOR_BLUE;
uint8_t vga_index;

uint16_t vga_entry_3(unsigned char ch, uint8_t fore_color, uint8_t back_color)
{
    uint16_t ax = 0;
    uint8_t ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_GREEN);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
void printf(const char* data){

}
void print_int(int data){
	char str[10];
	int i = 0;
	while(data > 0){
		str[i] = data % 10 + '0';
		data /= 10;
		i++;
	}
	str[i] = '\0';
	for(int j = i - 1; j >= 0; j--){
		terminal_putchar(str[j]);
	}
}


char hex_char(uint32_t val) {
    val &= 0xF; 
    if (val <= 9) {
        return '0' + val;
    } else {
        return 'A' + (val - 10);
    }
}

void print_hex(uint32_t d)
{
    terminal_writestring("0x");
    for(int i = 28; i >= 0; i-=4)
    {
        terminal_putchar(hex_char(d>>i));
    }

}

void print_new_line(){

	terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
}


void convert_unint8_to_str(uint8_t data, char* str){
	int i = 0;
	while(data > 0){
		str[i] = data % 10 + '0';
		data /= 10;
		i++;
	}
	str[i] = '\0';
	for(int j = i - 1; j >= 0; j--){
		terminal_putchar(str[j]);
	}
}


void clear_vga_buffer(uint16_t **buffer, uint8_t fore_color, uint8_t back_color)
{
    uint32_t i;
    for(i = 0; i < BUFSIZE; i++){
        (*buffer)[i] = vga_entry_3(0, fore_color, back_color);
    }
    next_line_index = 1;
    vga_index = 0;
}

class OperatingSystem {
    int tick = 0;

public:
bool key_bool = true;
    OperatingSystem(vga_color color) {
	


    }

    void init() {

        terminal_writestring("Initializing UiA Operating System....");
		print_new_line();
    }

    void debug_print(char *str) {
        terminal_writestring(str);
		print_new_line();
	}

	// void interrupt_handler_0(registers_t regs) {
	// 	terminal_writestring("Called Interrupt Handler 0!");
	// 	print_new_line();
	// }
    // void interrupt_handler_1(registers_t regs) {
    //     terminal_writestring("Called Interrupt Handler 1!");
	// 	print_new_line();
		 
    // }

    void interrupt_handler_3(registers_t regs) {
        terminal_writestring("Called Interrupt Handler 3!");
		print_new_line();
		 
    }

    void interrupt_handler_4(registers_t regs) {
        terminal_writestring("Called Interrupt Handler 4!");
		print_new_line();
		 
    }

	int get_current_tick()
	{
		tick++;
    	return tick;
	}

    void timer() {
        tick++;
        if (tick % 100 == 0) {
            // terminal_writestring("(Every Second) Tick: ");
			// print_new_line();
            // print_int(tick);
			// terminal_writestring(" yoooo ");
             
        }

    }
	

		void init_pit()
	{
		// Initialize the PIT by setting the desired divisor value
		// Here you can write the necessary code to initialize the PIT
		// For example, you can use the PIT_DEFAULT_DIVISOR to set the divisor value
		uint16_t divisor = DIVIDER;
		uint8_t low = (uint8_t)(divisor & 0xFF);
		uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

		// Send the command to set the PIT channel 0 divisor
		outb(PIT_CMD_PORT, 0x36); // 0x36 is the command byte to set Channel 0 to mode 3 (square wave generator) and use the specified divisor

		// Send the divisor value to the PIT channel 0 ports
		outb(PIT_CHANNEL0_PORT, low);
		outb(PIT_CHANNEL0_PORT, high);
	}

	void sleep_busy(int seconds)
	{
		key_bool = false;
		int start_tick = get_current_tick();
		int ticks_to_wait = seconds;
		int elapsed_ticks = 0;
		
		while (elapsed_ticks < ticks_to_wait)
		{
			elapsed_ticks++;
		}
		key_bool = true;
		
	}

	void sleep_interrupt(int seconds)
	{
		
		int current_tick = get_current_tick();
		int ticks_to_wait = seconds;
		int end_ticks = current_tick + ticks_to_wait;

		// Enable interrupts
		
		while (current_tick < end_ticks)
		{
			// enables interrupts
			asm volatile("sti");
			// Halt the CPU until the next interrupt
			asm volatile("hlt");
			current_tick = get_current_tick();

			
		}
	}
};



void printMemoryLayout()
{
    // Define the maximum memory address to display
    constexpr uint32_t maxAddress = 0xFFFFFFFF;

    // Start from the beginning of the memory
    uint32_t address = 0;

    // Iterate until the maximum address is reached
    while (address < maxAddress)
    {
        // Check if the current address is aligned
        bool aligned = (address % 4096 == 0);

        // Get the size of the current memory block
        uint32_t size = aligned ? 4096 : 1;

        // Print the memory block details
		terminal_writestring("Address: " + address );
        // std::cout << "Address: " << address << ", Size: " << size << std::endl;

        // Move to the next address
        address += size;
    }
}





uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void init_kernel_memory(uint32_t* kernel_end)
{
    last_alloc = (uint32_t)kernel_end + 0x1000;
    heap_begin = last_alloc;
    pheap_end = 0x400000;
    pheap_begin = pheap_end - (32 * 4096);
    heap_end = pheap_begin;
    memset((char *)heap_begin, 0, heap_end - heap_begin);
    pheap_desc = (uint8_t *)UiAOS::std::Memory::kmalloc(32);
    terminal_writestring("Kernel heap starts at 0x%x" + last_alloc);
}

void print_memory_layout()
{
    terminal_writestring("Memory used: %d bytes" + memory_used);
    terminal_writestring("Memory free: %d bytes" + heap_end - heap_begin - memory_used);
    terminal_writestring("Heap size: %d bytes" + heap_end - heap_begin);
    terminal_writestring("Heap start: 0x%x" + heap_begin);
    terminal_writestring("Heap end: 0x%x" + heap_end);
    terminal_writestring("PHeap start: 0x%xPHeap end: 0x%x" + pheap_begin + pheap_end);
}




void kernel_main(void)
{
    terminal_initialize();

	auto os = OperatingSystem(VGA_COLOR_GREEN);
	os.init();
	
 
	/* Newline support is left as an exercise. */
	init_gdt();
	terminal_writestring("Hello, you have now a GDT!");
	print_new_line();
	init_idt();
	terminal_writestring("Hello, you have now a IDT!");
	print_new_line();
	init_interrupts();
	idt_load();

	init_paging();
	terminal_writestring("Paging initialized");
	print_new_line();
	// Allocate 100 bytes of memory without alignment
	uint32_t normalAddress1 = UiAOS::std::Memory::kmalloc(100);
	
	// Allocate 100 bytes of memory without alignment
	uint32_t alignedAddress = UiAOS::std::Memory::kmalloc_a(100);
    
    // Allocate 200 bytes of memory and retrieve the physical address
    uint32_t physicalAddress;
    uint32_t normalAddress = UiAOS::std::Memory::kmalloc_p(200, &physicalAddress);
    
    // Allocate 150 bytes of memory with alignment and retrieve the physical address
    uint32_t alignedPhysicalAddress;
    uint32_t alignedAddress2 = UiAOS::std::Memory::kmalloc_ap(150, &alignedPhysicalAddress);
   
    // Allocate 50 bytes of memory without alignment
    //uint32_t normalAddress2 = UiAOS::std::Memory::kmalloc(50);
    


	//((uint32_t*)normalAddress2);
	//init_kernel_memory((uint32_t*)normalAddress2);
	// print_memory_layout();

	

	
	

	
	// Create operating system object
    
    
    // Do some printing!
    os.debug_print("Debug print!");
	print_new_line();
	

	// register_interrupt_handler(1,[](registers_t* regs, void* context){
    //     auto* os = (OperatingSystem*)context;
    //     os->interrupt_handler_1(*regs);
    // }, (void*)&os);

    // Create some interrupt handlers for 3
    register_interrupt_handler(3,[](registers_t* regs, void* context){
        auto* os = (OperatingSystem*)context;
        os->interrupt_handler_3(*regs);
    }, (void*)&os);

    // Create some interrupt handler for 4
	
    register_interrupt_handler(4,[](registers_t* regs, void* context){
        auto* os = (OperatingSystem*)context;
        os->interrupt_handler_4(*regs);
    }, (void*)&os);
	


    // Fire interrupts! Should trigger callback above
	// asm volatile ("int $0x1");
    // asm volatile ("int $0x3");
    // asm volatile ("int $0x4");

    // Disable interrutps
    asm volatile("sti");
	
    // Create a timer on IRQ0 - System Timer
    UiAOS::CPU::PIT::init_timer(1, [](registers_t*regs, void* context){
        auto* os = (OperatingSystem*)context;
        os->timer();
		//terminal_writestring("timer");
    }, &os);


	
    // Hook Keyboard
	
	print_new_line();
    UiAOS::IO::Keyboard::hook_keyboard([](uint8_t scancode, void* context){
        auto* os = (OperatingSystem*)context;
		if (!os->key_bool){
			return;
		}
        terminal_writestring("Keyboard Event: ");
		terminal_putchar(UiAOS::IO::Keyboard::scancode_to_ascii(scancode));
        terminal_writestring(" (");
        print_int(scancode);
        terminal_writestring(")");
		print_new_line();
        
    }, &os);
	
	// Enable interrupts
	asm volatile("sti");


	os.init_pit();
	terminal_writestring("start sleep_interrupt(one second)");
	print_new_line();
	
	os.sleep_interrupt(10000);
	terminal_writestring("end sleep_interrupt(one second)");
	print_new_line();
	

	
	print_new_line();
	terminal_writestring("start busy_interrupt(one second)");
	print_new_line();
	
	os.sleep_busy(1000);
	terminal_writestring("end busy_interrupt(one second)");
	print_new_line();

	while(1){}
	
}
