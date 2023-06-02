#include "screen.h"
#include <math.h>
#include "monitor.h"

static uint32_t next_line_index = 1;
uint8_t g_fore_color_1 = WHITE, g_back_color_1 = BLUE;
void full_screen()
{
    char *video = (char *)0xB8000;

    for (int i = 0; i < 4000; i += 2)
    {
        video[i] = video[i + 160];
        video[i + 1] = video[i + 161];
    }
}

void clear_vga_buffer(uint16_t **buffer, uint8_t fore_color, uint8_t back_color)
{
    uint32_t i;
    for (i = 0; i < BUFSIZE; i++)
    {
        (*buffer)[i] = vga_entry(0, fore_color, back_color);
    }
    next_line_index = 1;
    vga_index = 0;
}

void init_vga(uint8_t fore_color, uint8_t back_color)
{
    vga_buffer = (uint16_t *)VGA_ADDRESS;
    clear_vga_buffer(&vga_buffer, fore_color, back_color);
    g_fore_color_1 = fore_color;
    g_back_color_1 = back_color;
}

void monitor_write(int colour, const char *string)
{
    char *video = (char *)0xB8000;
    int after_grub = 0;

    for (int i = 0; i < 4000; i += 2)
    {
        if (video[i] != ' ')
        {
            after_grub = i;
        }
    }
    int indent = after_grub % 160;
    after_grub += 160 - indent;
    int counter = 0;

    if (after_grub > 3840)
    {
        full_screen();
    }
    while (string[counter] != 0)
    {
        video[after_grub] = string[counter];
        video[after_grub + 1] = colour;
        counter++;
        after_grub += 2;
    }
}

void print_char(char ch)
{
    vga_buffer[vga_index] = vga_entry(ch, g_fore_color_1, g_back_color_1);
    vga_index++;
}

void print_string(char *str)
{
    uint32_t index = 0;
    while (str[index])
    {
        print_char(str[index]);
        index++;
    }
}

uint16_t vga_entry(unsigned char ch, uint8_t fore_color, uint8_t back_color)
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

void clear_screen(uint8_t fore_color, uint8_t back_color)
{
    clear_vga_buffer(&vga_buffer, fore_color, back_color);
}

void move_cursor_next_line()
{
    if (next_line_index >= 55)
    {
        next_line_index = 0;
        clear_vga_buffer(&vga_buffer, g_fore_color_1, g_back_color_1);
    }
    vga_index = 80 * next_line_index;
    next_line_index++;
}

void print_hex(uint32_t n)
{
    uint32_t tmp;

    print_string("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }

        if (tmp >= 0xA)
        {
            noZeroes = 0;
            print_char(tmp - 0xA + 'a');
        }
        else
        {
            noZeroes = 0;
            print_char(tmp + '0');
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        print_char(tmp - 0xA + 'a');
    }
    else
    {
        print_char(tmp + '0');
    }
}

void print_int(int32_t num)
{
    char num_str[12];

    int i = 0;

    if (num == 0)
    {
        print("0");
        return;
    }

    if (num < 0)
    {
        print("-");
        num = -num;
    }

    while (num > 0)
    {
        num_str[i] = '0' + num % 10;
        num /= 10;
        i++;
    }

    for (int j = i - 1; j >= 0; j--)
    {
        char digit_str[2] = {num_str[j], '\0'};

        print_ad(digit_str, -158 * (i - j - 1));
    }
}

void print_ad(const char *str, int ad)
{
    // Set cursor position to next line
    char *vidmem = (char *)0xb8000;
    int last_line = 0;
    for (int i = 0; i < 80 * 25 * 2; i += 160)
    {
        if (vidmem[i] != ' ')
        {
            last_line = i;
        }
    }

    int i = 0;
    last_line += 160 + ad;
    if (last_line >= 80 * 25 * 2)
    {
        full_screen();
        last_line -= 160;
    }
    while (str[i] != '\0')
    {
        vidmem[last_line + i * 2] = str[i];
        vidmem[last_line + i * 2 + 1] = 0x0ff;
        i++;
    }
}

void print(const char *str)
{
    print_ad(str, 0);
}
