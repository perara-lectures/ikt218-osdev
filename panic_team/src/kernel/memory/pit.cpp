#include "pit.h"
//This is AI
#include <stdint.h>
#include <stdint.h>

// Read a byte from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Function to get the current tick value
uint32_t get_current_tick() {
    // Access the system timer or counter to obtain the current tick value
    // Replace this placeholder code with the actual implementation specific to your system

    // Example implementation for x86 architecture using the PIT
    // Read the value from the PIT channel 0 counter register

    uint32_t tick;

    // Read the low byte
    tick = inb(PIT_CHANNEL0_PORT);

    // Read the high byte
    tick |= ((uint32_t)inb(PIT_CHANNEL0_PORT)) << 8;

    // Return the current tick value
    return tick;
}


//this is AI
void sleep_busy(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;

    while (elapsed_ticks < ticks_to_wait) {
        while (get_current_tick() == start_tick + elapsed_ticks) {
            // Do nothing (busy wait)
        }

        elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks) {
        // Enable interrupts and halt the CPU until the next interrupt
        __asm__ __volatile__ ("sti; hlt");

        current_tick = get_current_tick();
    }
}

/*
Function: sleep_busy
	Set start_tick to get_current_tick()
	Set ticks_to_wait to milliseconds * TICKS_PER_MS
	Set elapsed_ticks to 0
	While elapsed_ticks is less than ticks_to_wait
		a. While get_current_tick() is equal to start_tick + elapsed_ticks
			i. Do nothing (busy wait)
		b. Increment elapsed_ticks


Function: sleep_interrupt
	Set current_tick to get_current_tick()
	Set ticks_to_wait to milliseconds * TICKS_PER_MS
	Set end_ticks to current_tick + ticks_to_wait
	While current_tick is less than end_ticks
		a. Execute the following assembly instructions:
			i. Enable interrupts (sti)
			ii. Halt the CPU until the next interrupt (hlt)
		b. Set current_tick to get_current_tick()
*/