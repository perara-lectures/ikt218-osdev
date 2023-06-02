#ifndef IKT218_ADVANCED_OPERATING_SYSTEM_TERMINAL2_H
#define IKT218_ADVANCED_OPERATING_SYSTEM_TERMINAL2_H

#define VGA_ADDRESS 0xB8000
#define BUFSIZE 2200

#include <stdint.h>

void monitor_write(int colour, const char *string);

void move_cursor_next_line();

void print_int(int num);

void print_string(char *str);

void init_vga(uint8_t fore_color, uint8_t back_color);

static uint16_t *vga_buffer;
static uint32_t vga_index;
void clear_screen(uint8_t fore_color, uint8_t back_color);

void clear_vga_buffer(uint16_t **buffer, uint8_t fore_color, uint8_t back_color);

void print_char(char ch);

uint16_t vga_entry(unsigned char ch, uint8_t fore_color, uint8_t back_color);
void print_ad(const char *str, int ad);
void print(const char *str);
int printf(const char *__restrict__ format, ...);
#endif // IKT218_ADVANCED_OPERATING_SYSTEM_TERMINAL2_H