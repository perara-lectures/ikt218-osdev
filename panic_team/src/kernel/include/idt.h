#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

// Define the IDT entry structure
struct idt_entry_t
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// Define the IDT pointer structure
struct idt_ptr_t
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Initialize the IDT
void init_idt();

// Load the IDT
void idt_load();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

#endif // IDT_H
