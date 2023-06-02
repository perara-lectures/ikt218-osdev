#ifndef UIAOS_HARDWARE_H
#define UIAOS_HARDWARE_H

#include "stdint.h"

void outb1(uint16_t port, uint8_t value);
uint8_t inb1(uint16_t port);
uint16_t inw1(uint16_t port);

#endif // UIAOS_HARDWARE_H
