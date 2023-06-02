#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "common.h"
#include <cstdlib>
#include <stdio.h>
#include "system.h"
#include "../memory/memory.h"
#include "../screen/screen.h"
#include "monitor.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../drivers/keyboard/keyboard.h"
#include "isr.h"
#include "hardware.h"
#include "timer.h"
extern uint32_t end; // This is defined in linker.ld
// Overload the new operator for single object allocation

void *operator new(std::size_t size)
{
    return malloc(size); // Call the C standard library function malloc() to allocate memory of the given size and return a pointer to it
}

// Overload the delete operator for single object deallocation
void operator delete(void *ptr) noexcept
{
    free(ptr); // Call the C standard library function free() to deallocate the memory pointed to by the given pointer
}

// Overload the new operator for array allocation
void *operator new[](std::size_t size)
{
    return malloc(size); // Call the C standard library function malloc() to allocate memory of the given size and return a pointer to it
}

// Overload the delete operator for array deallocation
void operator delete[](void *ptr) noexcept
{
    free(ptr); // Call the C standard library function free() to deallocate the memory pointed to by the given pointer
}

// Define entry point in asm to prevent C++ mangling
extern "C"
#include <system.h>
#include "../memory/memory.h"
#include "monitor.h"
#include "../drivers/keyboard/keyboard.h"
#include "isr.h"
{

    void kernel_main();
}
class OperatingSystem
{
    int tick = 0;

public:
    OperatingSystem()
    {
    }

    void init()
    {

        monitor_write(0xA, "Initializing UiA Operating System....");
        move_cursor_next_line();
    }

    void debug_print(char *str)
    {
        monitor_write(0xF, str);
        move_cursor_next_line();
    }

    void interrupt_handler_4(UiAOS::CPU::ISR::registers regs)
    {
        monitor_write(0xA, "Called Interrupt Handler 4!");
        move_cursor_next_line();
    }
    void interrupt_handler_9(UiAOS::CPU::ISR::registers regs)
    {
        monitor_write(0xA, "Called Interrupt Handler 9!");
        move_cursor_next_line();
    }

    void timer()
    {
        tick++;
        if (tick % 100 == 0)
        {
        }
    }
};

void demonstrate_page_fault()
{

    void (*callback)(registers_t &) = [](registers_t &regs)
    {
        // A page fault has occurred.
        // The faulting address is stored in the CR2 register.
        uint32_t faulting_address;
        asm volatile("mov %%cr2, %0"
                     : "=r"(faulting_address));

        // The error code gives us details of what happened.
        int present = !(regs.err_code & 0x1); // Page not present
        int rw = regs.err_code & 0x2;         // Write operation?
        int us = regs.err_code & 0x4;         // Processor was in user-mode?
        int reserved = regs.err_code & 0x8;   // Overwritten CPU-reserved bits of page entry?
        int id = regs.err_code & 0x10;        // Caused by an instruction fetch?

        // Output an error message.
        printf("Page fault! ( ");
        if (present)
        {
            printf("present ");
        }
        if (rw)
        {
            printf("read-only ");
        }
        if (us)
        {
            printf("user-mode ");
        }
        if (reserved)
        {
            printf("reserved ");
        }
        printf(") at 0x%x", faulting_address);
        printf("\n");
        // panic("Page fault");
    };
}

void kernel_main()
{
    monitor_write(0xF, "Hello, World!");
    // Initialize kernel memory manager with the end of the kernel image
    // init_kernel_memory(&end);
    // init_vga(WHITE, RED);

    // Create operating system object

    // Create some interrupt handlers for
    // monitor_initialize();

    // monitor_write(text, l);
    //  init_kernel_memory(&end);

    // Initialize Global Descriptor Table (GDT)
    init_gdt();

    // Initialize Interrupt Descriptor Table (IDT)
    init_idt();

    // Initialize Interrupt Requests (IRQs)
    init_irq();

    auto os = OperatingSystem();

    os.init();
    // Do some printing!
    os.debug_print("Hello Welcome to my operetingssystem");
    // Initialize Paging
    // init_paging();

    // Print memory layout
    // print_memory_layout();
    /*
        // Allocate some memory using kernel memory manager
        void *some_memory = malloc(12345);
        void *memory2 = malloc(54321);
        void *memory3 = malloc(13331);
        char *memory4 = new char[1000]();
    */
    // demonstrate_page_fault();
    demonstrate_page_fault();

    // Create some interrupt handlers for 9
    UiAOS::CPU::ISR::register_interrupt_handler(
        9, [](UiAOS::CPU::ISR::registers *regs, void *context)
        {
        auto* os = (OperatingSystem*)context;
        os->interrupt_handler_9(*regs); },
        (void *)&os);

    // Create some interrupt handler for 8
    UiAOS::CPU::ISR::register_interrupt_handler(
        4, [](UiAOS::CPU::ISR::registers *regs, void *context)
        {
        auto* os = (OperatingSystem*)context;
        os->interrupt_handler_4(*regs); },
        (void *)&os);

    // Trigger interrupts 3 and 4 which should call the respective handlers
    asm volatile("int $0x9");
    asm volatile("int $0x4");
    // asm volatile("int $0x4");

    // Disable interrupts temporarily
    asm volatile("sti");

    // Create a timer on IRQ0 - System Timer

    UiAOS::CPU::PIT::init_timer(
        1, [](UiAOS::CPU::ISR::registers *regs, void *context)
        {
        auto* os = (OperatingSystem*)context;
        os->timer(); },
        &os);

    monitor_write(0xF, "Merhaba Doris");
    monitor_write(0xF, "Hello Grup Panic");
    UiAOS::IO::Keyboard::hook_keyboard([](uint8_t scancode, void *context)
                                       { auto *os = (OperatingSystem *)context; },
                                       &os);
}
s