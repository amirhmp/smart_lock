#include <delay.h>
#include "../utils/utils.h"

unsigned char *keypad_port_addr;
unsigned char *keypad_pin_addr;
#define KEYPAD_BOUNCE_TIME 20

char keypad_chars[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
};

void
init_keypad(unsigned char *port_register_addr, unsigned char *pin_register_addr, unsigned char *ddr_register_addr) {
    keypad_port_addr = port_register_addr;
    keypad_pin_addr = pin_register_addr;
    //
    *ddr_register_addr = 0x0F;
    *keypad_port_addr = 0xF0;
}

char scan_keypad() { //4 paye paeen vorudi va pull up, 4 paye bala khoruji
    static bit flag = 1;
    static const unsigned char out[] = {
            0b11111110,
            0b11111101,
            0b11111011,
            0b11110111
    };
    short i;

    if ((*keypad_pin_addr & 0xf0) != 0xf0 && flag) {  //age har dokme ee feshar dade shod va flag dashtim
        delay_ms(KEYPAD_BOUNCE_TIME);
        flag = 0;
        for (i = 0; i <= 3; i++) {
            *keypad_port_addr = out[i];
            if (readPin(keypad_pin_addr, 4) == 0) return keypad_chars[i][0];
            else if (readPin(keypad_pin_addr, 5) == 0) return keypad_chars[i][1];
            else if (readPin(keypad_pin_addr, 6) == 0) return keypad_chars[i][2];
            else if (readPin(keypad_pin_addr, 7) == 0) return keypad_chars[i][3];
        }
    } else if ((*keypad_pin_addr & 0xf0) == 0xf0) {     //age dokme ee feshar dade nashode bashe
        delay_ms(KEYPAD_BOUNCE_TIME);
        flag = 1;
        *keypad_port_addr = 0xf0;   //dokme ee feshar dade nashode pas in meghdar ra migozarim ta agar dokme ee feshar dade shod befahmim va nakhad hame paye ha ro check konim va clock masraf she
    }
    return 0;
}
