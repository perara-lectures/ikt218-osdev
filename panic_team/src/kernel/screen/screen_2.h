// monitor.h -- Defines the interface for monitor.h
//              From JamesM's kernel development tutorials.

#ifndef SCREEN_2H
#define SCREEN_2H

#include "common.h"
#include <stdint.h>
#include <stddef.h>

void screen_initialize(void);
void screen_setcolor(void);
// void monitor_putentryat(char c, uint8_t color, size_t x, size_t y);

void screen_put(char c);
void screen_clear();
void screen_write(const char *data, size_t size);
void screen_write_hex(uint32_t n);
void screen_write_dec(uint32_t n);

#endif // SCREEN_2H