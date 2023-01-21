#ifndef SIM800L_SIMULATION_UTILS_H
#define SIM800L_SIMULATION_UTILS_H

#include "../../utils/utils.h"

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define MAX(a, b) (a > b ? a: b)
#define MIN(a, b) (a < b ? a: b)

#define NULL ((void *)0)

#define DISABLE_INTERRUPTS #asm("cli")
#define ENABLE_INTERRUPTS #asm("sei")

int str_len(const char *str);

void str_copy(const string src, string dest);

int indexOfFrom(const char *text, const char *pattern, int fromIndex);

int indexOf(const char *text, const char *pattern);

string substring(const string text, int startIndex, int endIndex);

string trim(string text);

int toInt(string str);

string flash2mem(flash string flashStr);

unsigned long diffTime(unsigned long currentTime, unsigned long lastTime);

void putchar1(char c);

void serial_send1(string text);

void serial_send1_counted(string text, unsigned int count);

void serial_print_number1(int number);

void printf1(char flash

*str);

#endif //SIM800L_SIMULATION_UTILS_H
