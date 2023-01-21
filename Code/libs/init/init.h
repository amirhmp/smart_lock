#ifndef INIT_H
#define INIT_H


#define DATA_REGISTER_EMPTY (1 << UDRE0)
#define RX_COMPLETE (1 << RXC0)
#define FRAMING_ERROR (1 << FE0)
#define PARITY_ERROR (1 << UPE0)
#define DATA_OVERRUN (1 << DOR0)

#define PIN_SIM800_PWR_CNT 5
#define ADDR_SIM800_PWR_CNT_PORT &PORTE

#define ADDR_DIGITAL_OUT &PORTA
#define ADDR_DIGITAL_IN &PINA

#define PIN_LOCK_CMD 0
#define PIN_OUT_2 1
#define PIN_OUT_3 2
#define PIN_OUT_4 3
#define PIN_DOOR_STATE 4
#define PIN_IN_6 5
#define PIN_IN_7 6
#define PIN_IN_8 7

#define ADDR_DEVICE_IN &PING
#define  PIN_LOCK_TOGGLE_BTN 0

#define ADDR_BLINKING_LED_PORT &PORTE
#define PIN_BLINKING_LED 4

#define ADDR_KEYPAD_PORT &PORTB
#define ADDR_KEYPAD_PIN &PINB
#define ADDR_KEYPAD_DDR &DDRB

#define MQ_SENSOR_PIN 0

void init();
unsigned int read_adc(unsigned char adc_input);

#endif // INIT_H