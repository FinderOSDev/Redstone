#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// dont mess with this pls, i will be very sad because kernel will break ;C
__attribute__((section(".multiboot"), used))
const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x0,
    (uint32_t)-(0x1BADB002)
};

int pos = 0;
uint8_t color = 0x0F;
uint16_t* vga = (uint16_t*)0xB8000;
bool shift = false;

void scroll();

// lets hope my display isnt black or giving no input...

extern "C" void print_wrtCH(char c)
{
    if (c == '\n')
        pos += 80 - (pos % 80);
    else
        vga[pos++] = ((uint16_t)color << 8) | c;

    if (pos >= 80 * 25) {
        scroll();
        pos -= 80;
    }
}

void print(const char* out) {
    while (*out)
        print_wrtCH(*out++);
}

void scroll()
{
    uint16_t blank = (color << 8) | ' ';

    for (int i = 0; i < 80 * 24; i++)
        vga[i] = vga[i + 80];

    for (int i = 80 * 24; i < 80 * 25; i++)
        vga[i] = blank;
}

extern "C" void clear()
{
    uint16_t blank = (color << 8) | ' ';

    for (int i = 0; i < 80 * 25; i++)
        vga[i] = blank;

    pos = 0;
}

// usb ports needa works

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// i can has keys work

const char keymap[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

char scan()
{
    uint8_t scancode;

    while (1)
    {
        if (inb(0x64) & 1) {
            scancode = inb(0x60);

            if (scancode == 0x2A || scancode == 0x36) {
                shift = true;
                continue;
            }

            if (scancode == 0xAA || scancode == 0xB6) {
                shift = false;
                continue;
            }

            if (scancode & 0x80)
                continue;

            if (scancode >= 128)
                continue;

            char c = keymap[scancode];

            if (shift && c) {
                if (c >= 'a' && c <= 'z') {
                    c -= 32;
                } else {
                    switch (c) {
                        case '1': c = '!'; break;
                        case '2': c = '@'; break;
                        case '3': c = '#'; break;
                        case '4': c = '$'; break;
                        case '5': c = '%'; break;
                        case '6': c = '^'; break;
                        case '7': c = '&'; break;
                        case '8': c = '*'; break;
                        case '9': c = '('; break;
                        case '0': c = ')'; break;
                        case '-': c = '_'; break;
                        case '=': c = '+'; break;
                        case '[': c = '{'; break;
                        case ']': c = '}'; break;
                        case '\\': c = '|'; break;
                        case ';': c = ':'; break;
                        case '\'': c = '"'; break;
                        case ',': c = '<'; break;
                        case '.': c = '>'; break;
                        case '/': c = '?'; break;
                        case '`': c = '~'; break;
                    }
                }
            }

            return c;
        }
    }
}

int streq(const char* a, const char* b)
{
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}

void delay(int cycles)
{
    for (volatile int i = 0; i < cycles; i++);
}

void block(uint8_t bg)
{
    uint8_t col = (bg << 4);
    uint16_t val = (col << 8) | ' ';

    for (int i = 0; i < 80 * 25; i++)
        vga[i] = val;
}

extern "C" void kernel_main(void) 
{
    clear();

    color = 0x0C;
    print("root");
    color = 0x0F;
    print("@redstoneOS $ ");

    char input[128];
    int index = 0;

    while (1)
    {
        char c = scan();
        if (!c) continue;

        if (c == '\n') {
            print_wrtCH('\n');
            input[index] = '\0';

            if (streq(input, "quickfetch")) {
                print("\n");
                color = 0x0C; print("    11111     "); color = 0x0F; print("OS: Redstone x86_32\n");
                color = 0x0C; print("   1f sg 3    "); color = 0x0F; print("Kernel: 0.1\n");
                color = 0x0C; print("  4 cxhxjc3   "); color = 0x0F; print("CPU: Intel Core i5\n");
                color = 0x0C; print(" 7ncjkxi if3  "); color = 0x0F; print("Resolution: 300x200\n");
                color = 0x0C; print("whd xgudhih w "); color = 0x0F; print("Version: 1.0.0\n");
                color = 0x0C; print(" zwwwwwwwwwz  \n\n"); color = 0x0F;
            }
            else if (streq(input, "help")) {
                print("\n------GENERAL------\n");
                print("cls - Clears the screen.\n");
                print("rte - Text editor."); color = 0x0E; print(" note: To exit, just simply type /q\n"); color = 0x0F;
                print("quickfetch - System info.\n");
                print("sysabout - About system.\n");
                print("\n--------FUN--------\n");
                print("city - Artwork."); color = 0x0E; print(" note: The Command was somehow removed out of no where >:C\n"); color = 0x0F;
                print("\n----FILESYSTEM-----\n");
                print("dir - Shows a list of files.\n");
                print("dir /w - This is just testing the nesting of commands.\n\n");
            }
            else if (streq(input, "sysabout")) {
                print("\nRedstoneOS 1.0.0\n\n");
            }
            else if (streq(input, "cls")) {
                clear();
            }
            else if (streq(input, "rte")) {
                clear();

                color = 0x4F;
                print("                              Redstone Text Editor                              ");
                color = 0x70;
                print("File Edit                                                                       ");
                color = 0x0F;

                char editor[256];
                int eindex = 0;

                while (1) {
                    char ec = scan();
                    if (!ec) continue;

                    if (ec == '\n') {
                        print_wrtCH('\n');
                        editor[eindex] = '\0';

                        if (streq(editor, ":q"))
                            break;

                        eindex = 0;
                    }
                    else if (ec == '\b' && eindex > 0) {
                        eindex--;
                        if (pos > 0) {
                            pos--;
                            vga[pos] = (color << 8) | ' ';
                        }
                    }
                    else {
                        if (eindex < 255) {
                            editor[eindex++] = ec;
                            print_wrtCH(ec);
                        }
                    }
                }

                clear();
            }

            // dir commands
            else if (streq(input, "dir")) {
                print("\nFILENAME - CREATED 1980, EDITED 1981\n");
                print("FILENAME - CREATED 1981, EDITED 1982\n");
                print("FILENAME - CREATED 1982, EDITED 1983\n");
                print("FILENAME - CREATED 1983, EDITED 1984\n\n");
            }
            else if (streq(input, "dir /t")) {
                color = 0x02;    
                print("this was just to test nesting in the terminal\n");
                color = 0x0F;
            }
            // commands not found :C
            else if (streq(input, "")) {
                print("\nPlease enter something.\n");
            }
            else {
                print("command not recognized\n");
            }

            // reset prompt
            index = 0;
            color = 0x0C;
            print("root");
            color = 0x0F;
            print("@redstoneOS $ ");
        }
        else if (c == '\b' && index > 0) {
            index--;
            if (pos > 0) {
                pos--;
                vga[pos] = (color << 8) | ' ';
            }
        }
        else {
            if (index < 127) {
                input[index++] = c;
                print_wrtCH(c);
            }
        }
    }
}