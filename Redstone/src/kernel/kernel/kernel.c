#include <stdint.h>

// multiboot header things cuz it wont boot

__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x0,
    -(0x1BADB002)
};



#define VIDEO_MEMORY 0xB8000

unsigned short* vga_buffer = (unsigned short*) VIDEO_MEMORY;
unsigned int cursor_pos = 0;

uint8_t term_color;

#define VGA_WIDTH 80

// Terminal thingys like printC('$'); and printL("RedstoneOS");

void printC(char c) {
    vga_buffer[cursor_pos++] = (term_color << 8) | c;
}

void printL(const char* str) {
    for (unsigned int i = 0; str[i] != '\0'; i++) {

        if (str[i] == '\n') {
            cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
        } else {
            printC(str[i]);
        }

    }
}

#define VGA_HEIGHT 25

void clear() {
    uint16_t blank = (term_color << 8) | ' ';

    for (unsigned int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }

    cursor_pos = 0;
}

uint8_t make_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color_red) {
    return (uint16_t)c | (uint16_t)color_red << 8;
}

void kernel_main(void)
{
    
    term_color = make_color(4, 0);
    clear();
    printL("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
    printL("____   _____  _____    _____  _____   _____   __   _  _____\n");
    printL("|   \\  | ___  |    \\  /    /  |_ _|  /     \\  | \\ | | | ___\n");
    printL("|   /  | ___  | |  |  \\    \\   | |  |   |   | |  \\| | | ___\n");
    printL("|___\\  |____  |____/  /____/   |_|   \\_____/  |_/\\__| |____\n");
    
    printC('<');
    term_color = make_color(12, 0);
    printL("RedstoneOS");
    term_color = make_color(4, 0);
    printC('>');
    term_color = make_color(15, 0);
    printL(" $ ");
    

    while (1) {

    }
}