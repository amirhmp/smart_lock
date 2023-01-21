//
// Created by Amir on 9/25/2022.
//

#ifndef CODE_MCU_UTILS_H
#define CODE_MCU_UTILS_H

#define string char *

#define bool short
#define false 0
#define true 1

void togglePin(unsigned char *port_x, unsigned char pin_x);

void setPin(unsigned char *port_x, unsigned char pin_x);

void clearPin(unsigned char *port_x, unsigned char pin_x);

int readPin(const unsigned char *pin_addr, unsigned char pin_x);

bool trace_chars(char incomingChar, int *tracer_index, string keyword, bool isQuestionMarkWildCard);

#endif //CODE_MCU_UTILS_H
