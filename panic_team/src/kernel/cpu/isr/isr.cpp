#include "isr.h"
#include "../screen/screen.h"
#include "hardware.h"
#include <stdint.h>
#include <cstring>
#include "string.h"
UiAOS::CPU::ISR::interrupt_t UiAOS::CPU::ISR::interrupt_handlers[256];

extern "C"
{
    void init_isr();
    void irq_handler(UiAOS::CPU::ISR::registers regs);
    void isr_handler(UiAOS::CPU::ISR::registers regs);
}

void init_isr()
{
    // memset(UiAOS::CPU::ISR::interrupt_handlers, 0, sizeof(UiAOS::CPU::ISR::interrupt_t) * 256);
}

void UiAOS::CPU::ISR::register_interrupt_handler(uint8_t n, isr_t handler, void *context)
{
    interrupt_handlers[n].handler = handler;
    interrupt_handlers[n].context = context;
}

void isr_handler(UiAOS::CPU::ISR::registers regs)
{
    uint8_t int_no = regs.int_no & 0xFF;
    UiAOS::CPU::ISR::interrupt_t intrpt = UiAOS::CPU::ISR::interrupt_handlers[int_no];
    if (intrpt.handler != nullptr)
    {
        intrpt.handler(&regs, intrpt.context);
    }
    else
    {
        for (;;)
            ;
    }
}
