#include <mega64a.h>
#include "alcd.h"
#include "delay.h"
#include "init.h"
#include <io.h>
#include <mega128_bits.h>
#include "../utils/utils.h"


// Voltage Reference: AREF pin
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))


void init() {
    //IO: first 4 bits are output and the other 4 bits are inputs => DDRA => [IIIIOOOO]
    DDRA = 0x0f;
    *ADDR_DIGITAL_OUT = 0xf0; // enable Pull up resistors for inputs
    //SIM800 PWR CONTROL PIN
    DDRE |= 1 << PIN_SIM800_PWR_CNT;   //SIM800 PWR CTRL PIN
    setPin(ADDR_SIM800_PWR_CNT_PORT, PIN_SIM800_PWR_CNT);
    //BLINKER LED
    DDRE |= 1 << PIN_BLINKING_LED;
    setPin(ADDR_BLINKING_LED_PORT, PIN_BLINKING_LED);

    // Device Input Buttons
    DDRG = 0x00;
    PORTG = 0xff;




    //Analog Inputs
    //TODO: circuit is not ready yet

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 250.000 kHz
    // Mode: Normal top=0xFF
    // OC0 output: Disconnected
    // Timer Period: 1 ms
    ASSR = 0 << AS0;
    TCCR0 = (0 << WGM00) | (0 << COM01) | (0 << COM00) | (0 << WGM01) | (1 << CS02) | (0 << CS01) | (0 << CS00);
    TCNT0 = 0x06;
    OCR0 = 0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK = (0 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) | (0 << OCIE1B) | (0 << TOIE1) | (0 << OCIE0) |
            (1 << TOIE0);
    ETIMSK = (0 << TICIE3) | (0 << OCIE3A) | (0 << OCIE3B) | (0 << TOIE3) | (0 << OCIE3C) | (0 << OCIE1C);

    // USART0 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART0 Receiver: On
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    // USART0 Baud Rate: 9600
    UCSR0A = (0 << RXC0) | (0 << TXC0) | (0 << UDRE0) | (0 << FE0) | (0 << DOR0) | (0 << UPE0) | (0 << U2X0) |
             (0 << MPCM0);
    UCSR0B =
            (1 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02) | (0 << RXB80) |
            (0 << TXB80);
    UCSR0C = (0 << UMSEL0) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00) | (0 << UCPOL0);
    UBRR0H = 0x00;
    UBRR0L = 0x67;

    // USART1 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART1 Receiver: On
    // USART1 Transmitter: On
    // USART1 Mode: Asynchronous
    // USART1 Baud Rate: 9600
    UCSR1A = (0 << RXC1) | (0 << TXC1) | (0 << UDRE1) | (0 << FE1) | (0 << DOR1) | (0 << UPE1) | (0 << U2X1) |
             (0 << MPCM1);
    UCSR1B =
            (1 << RXCIE1) | (0 << TXCIE1) | (0 << UDRIE1) | (1 << RXEN1) | (1 << TXEN1) | (0 << UCSZ12) | (0 << RXB81) |
            (0 << TXB81);
    UCSR1C = (0 << UMSEL1) | (0 << UPM11) | (0 << UPM10) | (0 << USBS1) | (1 << UCSZ11) | (1 << UCSZ10) | (0 << UCPOL1);
    UBRR1H = 0x00;
    UBRR1L = 0x67;




    // ADC initialization
    // ADC Clock frequency: 125.000 kHz
    // ADC Voltage Reference: AREF pin
    ADMUX = ADC_VREF_TYPE;
    ADCSRA = (1 << ADEN) | (0 << ADSC) | (0 << ADFR) | (0 << ADIF) | (0 << ADIE) | (1 << ADPS2) | (1 << ADPS1) |
             (1 << ADPS0);
    SFIOR = (0 << ACME);



    // Alphanumeric LCD initialization
    // Connections are specified in the
    // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
    // RS - PORTC Bit 0
    // RD - PORTC Bit 1
    // EN - PORTC Bit 2
    // D4 - PORTC Bit 4
    // D5 - PORTC Bit 5
    // D6 - PORTC Bit 6
    // D7 - PORTC Bit 7
    // Characters/line: 8
    lcd_init(16);
    lcd_clear();

// Global enable interrupts
#asm("sei")
}

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input) {
    ADMUX = adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
    delay_us(10);
// Start the AD conversion
    ADCSRA |= (1 << ADSC);
// Wait for the AD conversion to complete
    while ((ADCSRA & (1 << ADIF)) == 0);
    ADCSRA |= (1 << ADIF);
    return ADCW;
}


