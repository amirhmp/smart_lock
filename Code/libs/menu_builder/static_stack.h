

#ifndef CODE_STATIC_STACK_H
#define CODE_STATIC_STACK_H

#include "menu_builder.h"

#define Stack_Type struct MenuItem *

void stack_push(Stack_Type);
Stack_Type stack_pop(void);
Stack_Type stack_peek(void);

unsigned char stack_is_empty();
unsigned char stack_is_full();

#endif //CODE_STATIC_STACK_H
