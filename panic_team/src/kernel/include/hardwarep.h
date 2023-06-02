#ifndef UIAOS_HARDWAREP_H
#define UIAOS_HARDWAREP_H

#include "hardware.h"

#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

#endif // UIAOS_HARDWAREP_H