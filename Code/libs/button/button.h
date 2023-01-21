
#ifndef CODE_BUTTON_H
#define CODE_BUTTON_H


typedef struct {
    unsigned char *pin_x;
    unsigned char pin_number;

    void (*onPushed)(void);

    unsigned short flag;
} ButtonConfig;

void handleButton(ButtonConfig *);


#endif //CODE_BUTTON_H
