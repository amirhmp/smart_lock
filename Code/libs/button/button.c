#include "delay.h"
#include "button.h"

void handleButton(ButtonConfig *config) {
    if ((*(config->pin_x) & (1 << config->pin_number)) == 0 && config->flag == 0)  //flag for repetition
    {
        config->flag = 1;
        delay_ms(20);  //bounce
        config->onPushed();
    } else if ((*(config->pin_x) & (1 << config->pin_number)) != 0) {
        config->flag = 0;
    }
}

