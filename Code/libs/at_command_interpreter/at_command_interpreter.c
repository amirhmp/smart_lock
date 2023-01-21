#include <stdlib.h>
#include <stdio.h>
#include "string_buffer/string_buffer.h"
#include "utils/at_utils.h"
#include <io.h>
#include <delay.h>
#include "mega64a.h"
#include "alcd.h"
#include "at_command_interpreter.h"

unsigned long *_currTime;
#define CURRENT_TIME (*_currTime)


void (*onDeepHangHandler)(void);

void (*onNewSMSHandler)(int msgId);

void (*onNewCallHandler)(string callerID);


const char *DEFAULT_OK_RESPONSE = "OK";
const char *DEFAULT_ERROR_RESPONSE = "ERROR";
const char *SEND_SMS_SET_PHONE_OK_RESPONSE = ">";

#define DATA_REGISTER_EMPTY (1 << UDRE0)
#define RX_COMPLETE (1 << RXC0)
#define FRAMING_ERROR (1 << FE0)
#define PARITY_ERROR (1 << UPE0)
#define DATA_OVERRUN (1 << DOR0)

//command variables
bool hasPendingCommand = false;
Command current_command;
unsigned int ok_index_checker = 0;
unsigned int error_index_checker = 0;
// event variables
unsigned char eventDataBuffer[64];
unsigned int event_data_counter = 0;
int msg_event_checker_index = 0;
int call_event_checker_index = 0;

Command msg_event = {
        NOT_PASSED,         //PassState
        false,              //shouldBufferData
        "+CMTI: \"??\",",   //okResponse -> identifier  ===> BAZI VAGTA IN RO MIFRESTE `+CMTI: \"SM\",` VA BAZI VAGHTA IN RO `+CMTI: \"ME\",`
        "\r"                //errorResponse -> end of event
};
Command call_event = {
        NOT_PASSED,         //PassState
        false,              //shouldBufferData
        "+CLIP: \"",        //okResponse -> identifier
        "\""                //errorResponse -> end of event
};
//

void trace_incoming_chars(Command *command, char incomingChar, int *index, PassState succeedPassState,
                          flash string keyword, bool isQuestionMarkWildCard) {
    int i = *index;
    if (incomingChar == keyword[i] || (isQuestionMarkWildCard && keyword[i] == '?')) { //tracing..
        if (keyword[i + 1] == '\0') { //trace done successfully!
            command->passed = succeedPassState;
            hasPendingCommand = false;
            *index = 0;
        } else { //prepare for next round
            *index = i + 1;
        }
    } else if (incomingChar == keyword[0] ||
               (isQuestionMarkWildCard && keyword[i] == '?')) { //trace failed but new trace start again!
        *index = 1; //prepare for next round
    } else { //trace fully failed
        *index = 0;
    }
}


bool checkEvent(unsigned char incomingChar, Command *event, int *event_checker_index, unsigned char max_check_count,
                int timeout, bool identifierHasWildCard, bool endOfEventHasWildCard) {
    if (event->passed == NOT_PASSED) {
        trace_incoming_chars(
                event,
                incomingChar,
                event_checker_index,
                PASSED_OK,
                event->okResponse,
                identifierHasWildCard
        ); /*TODO: consider timeout*/
        if (event->passed == PASSED_OK) {
            event_data_counter = 0;
        }
    } else if (event->passed ==
               PASSED_OK) { //PASSED_OK dar inja vase daryaft eventIdentifier estefade shode (ex: `+CMTI` vase new sms)
        eventDataBuffer[event_data_counter++] = incomingChar;
        trace_incoming_chars( //TODO: mishe be jaye in 2 ta char bad ro dar nazar begirim yaano vaghto 2 ta char jadid umad va dg nakhad donbal index begardim
                event,
                incomingChar,
                event_checker_index,
                PASSED_ERROR,
                event->errorResponse,
                endOfEventHasWildCard
        );
        if (event->passed ==
            PASSED_ERROR) { //PASSED_ERROR vase ye evenet yaani endOfEvent shenasaee shode va dar asl khata nist
            eventDataBuffer[event_data_counter - 1] = '\0';
            //Reset Event State for finding next event
            event->passed = NOT_PASSED;
            *event_checker_index = 0;
            //
            return true;
        } else if (event_data_counter >= max_check_count) {
            //Reset Event State for finding next event
            event->passed = NOT_PASSED;
            event_data_counter = 0;
            *event_checker_index = 0;
        } else { //else if (CURRENT_TIME - lastCharTime > timeout) {
            //TODO: consider timeout for clearing buffer
        }
    }
    return false;
}

int extract_SMS_ID() {
    int messageId = -1;
    unsigned char firstDigit, secondDigit;
    //
    firstDigit = eventDataBuffer[0] - '0';
    secondDigit = eventDataBuffer[1] - '0';
    if (firstDigit >= 0 && firstDigit <= 9) {
        if (secondDigit >= 0 && secondDigit <= 9)
            messageId = secondDigit + firstDigit * 10;
        else
            messageId = firstDigit;
    }
    return messageId;
}

interrupt[USART0_RXC]

void usart0_rx_isr(void) {
    unsigned char status = UCSR0A;
    unsigned char data = UDR0;
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
        //
        putchar1(data); /*TODO:FIXME: remove this line: debug purpose*/
        //
        //check for events //vaghti yek event mire dar marhale identifier found dg nabayad baghie event ha check beshe, momkene dar mat msg ke ersal mishe identifier etefaghi ya amdi umade bashe va moshkel saz beshe, mishe vase hale in moshkel khundan sms ro gozasht tu akharin else (ya har chizi ke karbar mitune taghir tush ijad kone va bereste)
        if (checkEvent(data, &msg_event, &msg_event_checker_index, 15, 2000, true, false)) {
            int messageId = extract_SMS_ID();
            if (messageId != -1)
                (*onNewSMSHandler)(messageId);
        } else if (checkEvent(data, &call_event, &call_event_checker_index, 25, 2000, false, false)) {
            (*onNewCallHandler)(eventDataBuffer);
        }
        //check for jobs
        if (hasPendingCommand) {
            if (current_command.shouldBufferData) buffer_append_char(data);
            if (current_command.passed == NOT_PASSED)
                trace_incoming_chars(
                        &current_command,
                        data,
                        &ok_index_checker,
                        PASSED_OK,
                        current_command.okResponse,
                        false //TODO: momkene wild card dashte bashim (`?`), bayad yek attribute be current_command ezafe she
                );
            if (current_command.passed == NOT_PASSED)
                trace_incoming_chars(
                        &current_command,
                        data,
                        &error_index_checker,
                        PASSED_ERROR,
                        current_command.errorResponse,
                        false //TODO: momkene wild card dashte bashim (`?`), bayad yek attribute be current_command ezafe she
                );
        }
    }

}


JobResultType sendCommandBlocking(string commandText, flash string okRes, flash string errorRes, unsigned int timeout,
                                  bool shouldBufferData) {
    unsigned long lunchTime;
    PassState _passed;
    //
    current_command.passed = NOT_PASSED;
    current_command.okResponse = okRes;
    current_command.errorResponse = errorRes;
    current_command.shouldBufferData = shouldBufferData;
    //
    serial_send(commandText);
    //
    hasPendingCommand = true;
    lunchTime = CURRENT_TIME;
    //
    while (current_command.passed == NOT_PASSED && CURRENT_TIME - lunchTime < timeout);
    _passed = current_command.passed;
    hasPendingCommand = false;
    if (CURRENT_TIME - lunchTime > timeout) {
        return JOB_TIME_OUT;
    } else {
        if (_passed == PASSED_OK) {
            return JOB_OK;
        } else {
            return JOB_ERROR;
        }
    }
}


JobResult createJobResult(int failedCommandIndex, JobResultType type) {
    JobResult result;
    result.failedCommandIndex = failedCommandIndex;
    result.type = type;
    return result;
}

JobResult sendSMSBlocking(string number, string message) {
    char text[50];
    JobResultType result;

    result = sendCommandBlocking("AT+CMGF=1\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(0, result);

    result = sendCommandBlocking("AT+CSCS=\"GSM\"\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(1, result);

    result = sendCommandBlocking("AT+CSMP=17,167,2,25\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(2, result);

    sprintf(text, "AT+CMGS=\"%s\"\r", number);
    result = sendCommandBlocking(text, SEND_SMS_SET_PHONE_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(3, result);

    sprintf(text, "%s%c", message, 26);
    result = sendCommandBlocking(text, DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 60000, false);
    if (result != PASSED_OK) return createJobResult(4, result);
    return createJobResult(-1, result);
}

JobResult readSMSBlocking(int msgID) {
    char text[20];
    JobResultType result;

    result = sendCommandBlocking("AT+CMGF=1\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(0, result);

    sprintf(text, "AT+CMGR=%d\r", msgID);
    result = sendCommandBlocking(text, DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, true);
    if (result != PASSED_OK) return createJobResult(1, result);
    return createJobResult(-1, result);
}

JobResult startUpCommandBlocking() {
    JobResultType result;

    result = sendCommandBlocking("AT\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(0, result);

    result = sendCommandBlocking("AT\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(1, result);

    result = sendCommandBlocking("AT\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    if (result != PASSED_OK) return createJobResult(2, result);

    result = sendCommandBlocking("AT+CMGF=1\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    return createJobResult(3, result);
}

JobResult rejectIncomingCallBlocking() {
    JobResultType result;
    result = sendCommandBlocking("ATH\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    return createJobResult(0, result);
}

JobResult acceptIncomingCallBlocking() {
    JobResultType result;
    result = sendCommandBlocking("ATA\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    return createJobResult(0, result);
}

JobResult deleteAllSMSBlocking() {
    JobResultType result;
    result = sendCommandBlocking("AT+CMGDA=\"DEL ALL\"\r", DEFAULT_OK_RESPONSE, DEFAULT_ERROR_RESPONSE, 10000, false);
    return createJobResult(0, result);
}

void init_uart_transceiver() {
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
}

void init_interpreter(
        unsigned long *currTime,
        void (*onDeepHang)(void),
        void (*onNewSMS)(int msgId),
        void (*onNewCall)(string callerID)
) {
    _currTime = currTime;
    onDeepHangHandler = onDeepHang;
    onNewSMSHandler = onNewSMS;
    onNewCallHandler = onNewCall;
    init_uart_transceiver();
}

void deepHang() {
    printf1("\rSim800 is in deep hanging should hardware reset");
    (*onDeepHangHandler)();
}

void serial_send(string text) {
    printf("%s", text);
}

bool extract_sms_content(string message, string date, string phone) {
    int phoneStartIndex, phoneLastIndex;
    int dateFirstIndex, dateLastIndex;
    int okIndex;

    phoneStartIndex = buffer_index_of("+CMGR: \"REC", 0);
    if (phoneStartIndex == -1) {
        serial_send1("identifierStartIndex = -1");
        return false;
    }
    phoneStartIndex = buffer_index_of(",", phoneStartIndex + 11);
    if (phoneStartIndex == -1) {
        serial_send1("phoneStartIndex = -1");
        return false;
    }
    phoneStartIndex += 2;
    phoneLastIndex = buffer_index_of(",", phoneStartIndex);
    if (phoneLastIndex == -1) {
        serial_send1("phoneLastIndex = -1");
        return false;
    }
    phoneLastIndex -= 2;
    //
    dateFirstIndex = phoneLastIndex + 7;
    dateLastIndex = buffer_index_of("+", dateFirstIndex);
    if (dateLastIndex == -1) {
        serial_send1("dateLastIndex = -1");
        return false;
    }
    --dateLastIndex;
    //
    // TODO: in bayad enghadr tekrar she ta akharin Ok ro be dash biarim
    okIndex = buffer_index_of("OK", dateLastIndex + 1);
    if (okIndex == -1) {
        serial_send1("okIndex = -1");
        return false;
    }
    //
    buffer_substring2(phone, phoneStartIndex, phoneLastIndex);
    buffer_substring2(date, dateFirstIndex, dateLastIndex);
    buffer_substring2(message, dateLastIndex + 5, okIndex - 1);
//    sprintf(test_txt, "phone:%d, %d,\rdate:%d, %d,\rmsg:%d, %d,\r", phoneStartIndex, phoneLastIndex, dateFirstIndex, dateLastIndex, dateLastIndex + 3, okIndex - 1);
//    serial_send1(test_txt);
    return true;
}
