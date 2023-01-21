
#include "static_stack.h"
#include "stdlib.h"


#define STACK_SIZE 30

Stack_Type stack[STACK_SIZE];
unsigned int lastIndex;

unsigned char stack_is_empty() {
    return lastIndex == 0;
}

unsigned char stack_is_full() {
    return lastIndex == STACK_SIZE - 1;
}


void stack_push(Stack_Type data) {
    if (lastIndex < STACK_SIZE - 1) {
        stack[lastIndex] = data;
        lastIndex++;
    }
}
Stack_Type stack_pop() {
    if (lastIndex > 0) {
        lastIndex--;
        return stack[lastIndex];
    }
    return NULL;
}

Stack_Type stack_peek(){
    if (lastIndex > 0) {
        return stack[lastIndex - 1];
    }
    return NULL;
}
