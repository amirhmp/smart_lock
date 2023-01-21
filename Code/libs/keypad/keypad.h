
#ifndef CODE_KEYPAD_H
#define CODE_KEYPAD_H

void init_keypad(unsigned char *port_register_addr, unsigned char *pin_register_addr, unsigned char *ddr_register_addr);

char scan_keypad();

#endif //CODE_KEYPAD_H
