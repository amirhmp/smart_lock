
#include "command_queue.h"

#define BUFFER_SIZE 32
Type command_queue_array[BUFFER_SIZE];

unsigned int command_count = 0;
int command_rPtr = 0;
int command_wPtr = -1;

#define QUEUE_IS_NOT_FULL (command_count < BUFFER_SIZE)
#define QUEUE_IS_NOT_EMPTY (command_count > 0)

void command_queue_add_first(Type data) {
    if (QUEUE_IS_NOT_FULL) {
        ++command_count;
        if (--command_rPtr < 0)
            command_rPtr = BUFFER_SIZE - 1;
        command_queue_array[command_rPtr] = data;
    }
}

void command_queue_push(Type data) {
    if (QUEUE_IS_NOT_FULL) {
        ++command_count;
        if (++command_wPtr >= BUFFER_SIZE)
            command_wPtr = 0;
        command_queue_array[command_wPtr] = data;
    }
}


Type command_queue_poll() {
    if (QUEUE_IS_NOT_EMPTY) {
        Type data;
        --command_count;
        data = command_queue_array[command_rPtr++];
        if (command_rPtr >= BUFFER_SIZE)
            command_rPtr = 0;
        return data;
    }
    return (Type) NULL;
}

Type command_queue_peek() {
    if (QUEUE_IS_NOT_EMPTY) {
        return command_queue_array[command_rPtr];
    }
    return (Type) NULL;
}

bool command_queue_is_empty() {
    return (short)(command_count == 0);
}

unsigned int command_queue_size() {
    return command_count;
}