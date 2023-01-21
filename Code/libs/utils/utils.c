
#include "utils.h"

void togglePin(unsigned char *port_x, unsigned char pin_x) {
    *port_x ^= (1 << pin_x);
}

void setPin(unsigned char *port_x, unsigned char pin_x) {
    *port_x |= (1 << pin_x);
}

void clearPin(unsigned char *port_x, unsigned char pin_x) {
    *port_x &= ~(1 << pin_x);
}

int readPin(const unsigned char *pin_addr, unsigned char pin_x) {
    return (*pin_addr >> pin_x) & 0x01;
}


bool trace_chars(char incomingChar, int *tracer_index, string keyword, bool isQuestionMarkWildCard) {
    int i = *tracer_index;
    if (incomingChar == keyword[i] || (isQuestionMarkWildCard && keyword[i] == '?')) { //tracing..
        if (keyword[i + 1] == '\0') { //trace done successfully!
            *tracer_index = 0;
            return true;
        } else { //prepare for next round
            *tracer_index = i + 1;
        }
    } else if (incomingChar == keyword[0] || (isQuestionMarkWildCard && keyword[i] == '?')) { //trace failed but new trace start again!
        *tracer_index = 1; //prepare for next round
    } else { //trace fully failed
        *tracer_index = 0;
    }
    return false;
}

