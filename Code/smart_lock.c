
#include <mega64a.h>
#include <alcd.h>
#include "libs/init/init.h"
#include "libs/wait/wait.h"
#include "libs/at_command_interpreter/at_command_interpreter.h"
#include "libs/at_command_interpreter/utils/at_utils.h"
#include <delay.h>
#include <stdio.h>
#include "libs/at_command_interpreter/string_buffer/string_buffer.h"
#include <stdlib.h>
#include "libs/button/button.h"
#include "libs/utils/utils.h"
#include "libs/keypad/keypad.h"
#include "smart_lock.h"
#include "libs/menu_builder/menu_builder.h"
#include "libs/menu_builder/menus.h"
#include "libs/mq_sensor/mq.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

struct {
    unsigned long curTime;
    unsigned long blink_timer;
    unsigned long deepHangTimer;
    unsigned long check_door_timer;
    unsigned long sensor_read_timer;
} timers;


struct {
    int readSmsIndex;
    bool shouldSendSms;
    bool phoneRinged;
} flags = {-1, false, false};

char callerID[15];
string master_phone = "+989362163813";
char lcd_text[33];


eeprom char master_phone_number[11];

struct {
    unsigned char length;
    char value[11];
} master_phone_temp = {0, ""};


bool init_master_phone_from_eeprom(eeprom char *eepromAddr, char *destAddr, unsigned char charCount) {
    int i;
    if (eepromAddr[0] < '0' && eepromAddr[0] > '9') { // no saved phone exists
        // TODO: should print enter_master_phone menu
        return false;
    }
    for (i = 0; i < charCount; ++i) {
        if (eepromAddr[i] >= '0' && eepromAddr[i] <= '9') {
            destAddr[i] = eepromAddr[i];
        } else {
            destAddr[i] = '0';
            eepromAddr[i] = '0';
        }
    }
    return true;
}

void save_master_phone_in_eeprom(char *srcAddr, eeprom char *eepromAddr, unsigned char charCount) {
    int i;
    for (i = 0; i < charCount; ++i) { //save phone number into eeprom
        eepromAddr[i] = srcAddr[i];
    }
}

// USART1 Receiver interrupt service routine
interrupt[USART1_RXC]

void usart1_rx_isr(void) {
    /*TODO: tavajoh: be hich vaj tavabe blocking dar inttrupt routine ha seda za nashavand chun clock system az kar miofte (dar ISR inttrupt ha disable hastand pas timer ha vared isr marbute nemishe va clock mikhabe)*/   char status, data;
    status = UCSR1A;
    data = UDR1;
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
        unsigned char id = data - '0';
        if (id >= 1 && id <= 9) {
            flags.readSmsIndex = data - '0';
        }
    }
}

// Timer 0 overflow interrupt service routine //1ms
interrupt[TIM0_OVF]

void timer0_ovf_isr(void) {
    TCNT0 = 0x06; // Reinitialize Timer 0 value
    ++timers.curTime;
}


typedef enum {
    MODE_PASSWORD,
    MODE_MAIN_MENU,
    MODE_SET_PHONE,
    MODE_BROKEN,
} Mode;

Mode mode = MODE_PASSWORD;
int pass_index;
int input_pass_counter;
string default_password = "1234";
//A: UP
//B: DOWN
//C: Enter
//D: press -> Delete, hold -> clear


bool _isDoorOpen = false;

void setMode(Mode new_mode) {
    mode = new_mode;
    if (new_mode == MODE_PASSWORD) {
        print_password_msg();
    }
}


void main(void) {
    ButtonConfig
            btnToggleLockConfig = {ADDR_DEVICE_IN, PIN_LOCK_TOGGLE_BTN, &onToggleLockButtonClicked};

    // initializing micro and libs
    init();
    initWait(&timers.curTime);
    init_interpreter(&timers.curTime, &onDeepHang, &onNewSms, &onPhoneRinging);
    init_keypad(ADDR_KEYPAD_PORT, ADDR_KEYPAD_PIN, ADDR_KEYPAD_DDR);
    init_menu_builder(&mainMenu, ARRAY_SIZE(mainMenu));
    if (init_master_phone_from_eeprom(master_phone_number, master_phone_temp.value, ARRAY_SIZE(master_phone_number))) {
        master_phone_temp.length = ARRAY_SIZE(master_phone_number);
    }

    setMode(MODE_PASSWORD);

    unlock();

    // testing sim800
    // test_sim800_blocking(); //TODO: uncomment this line
    //delete messages if memory is full
    if (false) deleteAllSMSBlocking(); //TODO: har vaght memory por shod hame ro pak kon



    while (1) {
        char pressed_key = scan_keypad();
        if (pressed_key) {
            onKeyPressed(pressed_key);
        }


        if (wait(&timers.sensor_read_timer, 10)) {
            serial_print_number1(read_mq_sensor());
            serial_send1("\r\n");
        }

        handleButton(&btnToggleLockConfig);


        if (wait(&timers.blink_timer, 1000)) {
            togglePin(ADDR_BLINKING_LED_PORT, PIN_BLINKING_LED);
        }

        if (wait(&timers.check_door_timer, 100)) {
            bool isOpen = isDoorOpen();
            if (isOpen != _isDoorOpen) {
                _isDoorOpen = isOpen;
                onDoorStateChanged(isOpen);
            }
        }


        if (flags.shouldSendSms) {
            JobResult result;
            flags.shouldSendSms = false;
            result = sendSMSBlocking("09362163813", "Hello There!!");
            if (result.type == JOB_OK) {
                serial_send1("\rSMS Sent\r");
            } else {
                serial_send1("\rFailed at index:");
                serial_print_number1(result.failedCommandIndex);
                serial_send1("\r");
            }
        }
        if (flags.readSmsIndex != -1) {
            JobResult result;
            result = readSMSBlocking(flags.readSmsIndex);
            flags.readSmsIndex = -1;
            if (result.type == JOB_OK) {
                char message[50];
                char phone[20];
                char date[20];
                bool extractSuccess;
                //
                extractSuccess = extract_sms_content(message, date, phone);
                if (extractSuccess) {
                    // processing message
                    if (indexOf(phone, master_phone) != -1) {
                        if (indexOf(message, "open:1") != -1) {
                            clearPin(ADDR_DIGITAL_OUT, 6);
                        } else if (indexOf(message, "close:1") != -1) {
                            setPin(ADDR_DIGITAL_OUT, 6);
                        } else {
                            serial_send1("invalid command");
                        }
                    } else {
                        serial_send1("\ruser is not defined:");
                        serial_send1(phone);
                        serial_send1("\r");
                    }
                    //
                    //send info to debug serial port
                    serial_send1("message received from:");
                    serial_send1(phone);
                    putchar1('\r');
                    serial_send1(message);
                    serial_send1("date:");
                    serial_send1(date);
                    putchar1('\r');
                    //
                    //display on lcd
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_puts(
                            message); //khodesh ye \r avalesh dare vase in ke nakhad un ro hazf konam injoori doresh zadim, in dar satr 2 neshun dade mishe , phoen to satr aval
                    lcd_gotoxy(0, 0);
                    lcd_puts(phone);
                    //
                } else {
                    serial_send1("\rfailed to extract message content\r");
                }
                //
                buffer_clear();
            } else {
                serial_send1("\rFailed at index:");
                serial_print_number1(result.failedCommandIndex);
                serial_send1("\r");
            }
        }
        if (flags.phoneRinged) {
            flags.phoneRinged = false;
            //Displaying on LCD
            lcd_clear();
            lcd_gotoxy(0, 0);
            sprintf(lcd_text, "Ringing\n%s", callerID);
            lcd_puts(lcd_text);
            //send to debug serial port
            serial_send1("\rphone ringing, callerID:");
            serial_send1(callerID);
            putchar1('\r');
            //processing
            if (indexOf(callerID, master_phone) != -1) {
                acceptIncomingCallBlocking();
            } else {
                rejectIncomingCallBlocking();
            }
        }
    }
}


//events---------------------------------------------
void onKeyPressed(char key) {
    putchar1(key); // for debug
    switch (mode) {
        case MODE_PASSWORD:
        case MODE_BROKEN: {
            switch (key) {
                case 'A':
                    if (mode == MODE_BROKEN) {
                        print_enter_pass_to_unlock_msg();
                        break;
                    }
                    setMode(MODE_MAIN_MENU);
                    lcd_clear();
                    print_main_menu();
                    break;
                case 'B':
                case 'C':
                    if (mode == MODE_BROKEN) {
                        print_enter_pass_to_unlock_msg();
                        break;
                    }
                    break;
                case 'D':
                    pass_index = 0;
                    input_pass_counter = 0;
                    print_password_msg();
                    break;
                default:
                    if (input_pass_counter < 16) {
                        bool passed = trace_chars(key, &pass_index, default_password, false);
                        lcd_putchar('*'); // print stars instead of real characters of password
                        ++input_pass_counter;
                        if (passed && input_pass_counter == str_len(default_password)) {
                            input_pass_counter = 0;
                            if (mode == MODE_BROKEN) mode = MODE_PASSWORD;
                            unlock();
                        }
                    } else {
                        //TODO: alarm user with a beeb sound, bishtar az 16 char nemitune bezane
                    }
                    break;
            }
            break;
        }
        case MODE_MAIN_MENU: {
            switch (key) {
                case 'A':
                    menu_item_on_click();
                    break;
                case 'B':
                    serial_send1(" up ");
                    menu_up();
                    break;
                case 'C':
                    serial_send1(" down ");
                    menu_down();
                    break;
                case 'D':

                    break;
            }
            break;
        }
        case MODE_SET_PHONE: {
            switch (key) {
                case 'A':
                    if (master_phone_temp.length == ARRAY_SIZE(master_phone_temp.value)) {
                        // TODO: save in eeprom or a Flash, consider an external flash memory due to unpersistency of e2prom memories
                        serial_send1_counted(master_phone_temp.value, ARRAY_SIZE(master_phone_temp.value));
                        lcd_gotoxy(0, 0);
                        lcd_putsf("Saved!!         ");
                        save_master_phone_in_eeprom(master_phone_temp.value, master_phone_number,
                                                    master_phone_temp.length);
                        delay_ms(1000);
                        setMode(MODE_MAIN_MENU);
                        backMenu();
                        print_current_menu();
                    } else {
                        // TODO: beep sound: means phone is invalid
                        lcd_gotoxy(0, 0);
                        lcd_putsf("Invalid phone! ");
                        lcd_gotoxy(master_phone_temp.length, 1);
                    }
                    break;
                case 'D': // backspace
                    if (master_phone_temp.length != 0) {
                        --master_phone_temp.length;
                        lcd_gotoxy(master_phone_temp.length, 1);
                        lcd_putchar(' ');
                        lcd_gotoxy(master_phone_temp.length, 1);
                    }
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '0':
                    if (master_phone_temp.length < ARRAY_SIZE(master_phone_temp.value)) {
                        lcd_putchar(key);
                        master_phone_temp.value[master_phone_temp.length++] = key;
                    } else {
                        // TODO: beep sound, characters exceed its limit
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            serial_send1("onKeyPressed:modeNotImpl");
            break;
    }
}

void onDoorStateChanged(bool isOpen) {
    if (mode == MODE_PASSWORD)
        print_status();
    if (isOpen && isLocked() && mode != MODE_BROKEN) {
        // TODO: door is broken, should send a SMS
        lcd_gotoxy(0, 1);
        lcd_putsf("door is broken");
        mode = MODE_BROKEN;
    }
}

void onToggleLockButtonClicked() {
    if (mode == MODE_BROKEN) {
        print_enter_pass_to_unlock_msg();
    } else {
        if (_isDoorOpen) {
            lcd_clear();
            lcd_putsf("Close the door");
            lcd_gotoxy(0, 1);
            lcd_putsf("first!!");
        } else {
            if (isLocked()) unlock();
            else lock();
            delay_ms(100);
            print_status();
        }
    }
}

void onExitMenuClicked() {
    lcd_clear();
    setMode(MODE_PASSWORD);
    backMenu(); /*harvaght exit ro zad eyn back mimune va bayad in call she ta index ha reset shan*/
}

void onSetPhoneClicked() {
    short i;
    setMode(MODE_SET_PHONE);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putsf("Master Phone >");
    lcd_gotoxy(0, 1);
    for (i = 0; i < master_phone_temp.length; i++) {
        lcd_putchar(master_phone_temp.value[i]);
    }
}

void onDeepHang() {
    // PORTE |= ~(1 << 4); // Turn of Sim800
    // delay_ms(10000);
    // PORTE = 1 << 4;
}

void onPhoneRinging(string caller_ID) { //don't do blocking jobs here
    flags.phoneRinged = true;
    str_copy(caller_ID, callerID);
}

void onNewSms(int messageId) {
    serial_send1("\rnew sms received:");
    serial_print_number1(messageId);
    flags.readSmsIndex = messageId; //readMessageBlocking nabayad inja seda zade beshe chun onNewSms dar routine intrrupt call mishe
}

//sim800 LCD messages-----------------------------------------
void print_welcome_message() {
    serial_send1("Welcome!! Wait for signal..");
    sprintf(lcd_text, "Welcome!\nWait For Signal");
    lcd_puts(lcd_text);
}

void test_sim800_message() {
    serial_send1("\rTesting sim800\r");
    lcd_clear();
    sprintf(lcd_text, "Testing sim800");
    lcd_puts(lcd_text);
}

void sim800_ready_message() {
    serial_send1("\rsim800 is ready\r");
    lcd_clear();
    sprintf(lcd_text, "sim800 Ready");
    lcd_puts(lcd_text);
}

void sim800_failed_message() {
    serial_send1("\rsim800 not responding\r");
    lcd_clear();
    sprintf(lcd_text, "sim800 Failed");
    lcd_puts(lcd_text);;
}

void test_sim800_blocking() {
    print_welcome_message();
    delay_ms(3000);
    test_sim800_message();
    delay_ms(12000); // needed for sim800 load time
    if (startUpCommandBlocking().type == JOB_OK) {
        sim800_ready_message();
    } else {
        sim800_failed_message();
    }
    delay_ms(1000);
    lcd_clear();
}


void print_status() {
    lcd_clear();
    if (isLocked()) {
        lcd_putsf("Locked-");
    } else {
        lcd_putsf("UnLocked-");
    }

    if (_isDoorOpen) {
        lcd_putsf("Open");
    } else {
        lcd_putsf("Close");
    }
}

void unlock() {
    setPin(ADDR_DIGITAL_OUT, PIN_LOCK_CMD);
    print_status();
}

void lock() {
    clearPin(ADDR_DIGITAL_OUT, PIN_LOCK_CMD);
    print_status();
}

bool isLocked() {
    readPin(ADDR_DIGITAL_OUT, PIN_LOCK_CMD) == 1 ? false : true;
}

bool isDoorOpen() {
    return readPin(ADDR_DIGITAL_IN, PIN_DOOR_STATE) == 1 ? true : false;
}

void print_password_msg() {
    lcd_clear();
    lcd_putsf("Locked-Password:");
    lcd_gotoxy(0, 1);
};


void print_enter_pass_to_unlock_msg() {
    lcd_clear();
    lcd_putsf("broken: Enter");
    lcd_gotoxy(0, 1);
    lcd_putsf("pass to unlock");
    delay_ms(1500);
    lcd_clear();
    lcd_putsf("door is broken");
    lcd_gotoxy(0, 1);
}


#pragma clang diagnosti