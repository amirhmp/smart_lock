

#ifndef SIM800L_SIMULATION_COMMAND_QUEUE_H
#define SIM800L_SIMULATION_COMMAND_QUEUE_H

#include "../at_command_interpreter.h"

#define Type Command *

void command_queue_add_first(Type);

void command_queue_push(Type);

Type command_queue_poll(void);

Type command_queue_peek(void);

bool command_queue_is_empty(void);

unsigned int command_queue_size(void);

#endif
