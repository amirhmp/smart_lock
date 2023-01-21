//
// This Library use [uart_0_isr]
//

#ifndef SIM800L_SIMULATION_AT_COMMAND_INTERPRETER_H
#define SIM800L_SIMULATION_AT_COMMAND_INTERPRETER_H

#include <mega64a.h>
#include "utils/at_utils.h"

typedef enum {
    SEND_SMS,
    READ_SMS,
    USSD_CREDIT,
    START_UP,
    CHECK_HANG,
    SOFTWARE_RESET
} JobType;

typedef enum {
    JOB_TIME_OUT,
    JOB_ERROR,
    JOB_OK
} JobResultType;

typedef struct {
    JobResultType type;
    unsigned char failedCommandIndex;
} JobResult;

typedef enum {
    NOT_PASSED,
    PASSED_ERROR,
    PASSED_OK
} PassState;

typedef struct {
    PassState passed;
    bool shouldBufferData;
    flash string okResponse;
    flash string errorResponse;
} Command;

void deepHang(void);

JobResult sendSMSBlocking(string number, string message);


void init_interpreter(
        unsigned long *currTime,
        void (*onDeepHang)(void),
        void (*onNewSMS)(int msgId),
        void (*onNewCall)(string callerID)
); /*init rx tx and baud rate*/

void serial_send(string text);

JobResult readSMSBlocking(int msgID);

JobResult startUpCommandBlocking();

JobResult rejectIncomingCallBlocking();

JobResult acceptIncomingCallBlocking();

JobResult deleteAllSMSBlocking();

bool extract_sms_content(string message, string date, string phone);

interrupt [USART0_RXC]

void usart0_rx_isr(void);

#endif //SIM800L_SIMULATION_AT_COMMAND_INTERPRETER_H
