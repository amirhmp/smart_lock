#include <stdlib.h>
#include "at_utils.h"
#include <mega64a.h>

int str_len(const char *str) {
    int i;
    for (i = 0; str[i] != '\0'; ++i);
    return i;
}

void str_copy(const string src, string dest) {
    unsigned int i;
    for (i = 0; src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

int indexOfFrom(const char *text, const char *pattern, int fromIndex) {
    int i, j, textSize, patternSize;
    textSize = str_len(text);
    patternSize = str_len(pattern);
    if (patternSize > textSize)
        return -1;
    for (i = fromIndex; i <= textSize - patternSize; ++i) {
        for (j = 0; j < patternSize; ++j) {
            if (pattern[j] != text[i + j])
                break;
        }
        if (j == patternSize) return i;
    }
    return -1;
}

int indexOf(const char *text, const char *pattern) {
    return indexOfFrom(text, pattern, 0);
}


string substring(const string text, int startIndex, int endIndex) {
    int length, i;
    char *substr;
    if (startIndex == -1 || endIndex == -1)
        return NULL;
    if (startIndex > endIndex)
        return NULL;

    length = str_len(text);
    endIndex = MIN(endIndex, length);
    substr = malloc(sizeof(char) * (endIndex - startIndex + 1)); //+1 is for \0
    for (i = startIndex; i < endIndex; ++i) {
        substr[i - startIndex] = text[i];
    }
    substr[i - startIndex] = '\0';
    return substr;
}

bool is_space(char ch) {
    return ch == ' ' ||
           ch == '\n' ||
           ch == '\v' ||
           ch == '\f' ||
           ch == '\r' ||
           ch == '\t';
}

string trim(string text) {
    int size;
    int i, j;
    size = str_len(text);
    if (size == 0)
        return text;
    for (i = 0; is_space(text[i]); ++i) {
        if (i >= size) { /*yaani koleseh space bude*/
            return flash2mem("");
        }
    }
    for (j = size - 1; is_space(text[j]); --j) {
        if (j < 0)
            return flash2mem("");
    }
    return substring(text, i, j + 1);
}


int toInt(string str) {
    int len, factor = 1, i, res = 0, digit;
    len = str_len(str);
    for (i = len - 1; i >= 0; --i) {
        digit = str[i] - '0';
        if (digit >= 0 && digit <= 9) {
            res += (digit * factor);
            factor *= 10;
        } else {
            //TODO Exception
            return -1;
        }
    }
    return res;
}

string flash2mem(flash string flashStr) {
    int length, i;
    string result;
    for (length = 0; flashStr[length] != '\0'; ++length);
    ++length; //because \0
    result = malloc(sizeof(char) * length);

    for (i = 0; i < length; ++i) {
        result[i] = flashStr[i];
    }
    return result;
}


unsigned long diffTime(unsigned long currentTime, unsigned long lastTime) {
    if ((long) (currentTime - lastTime) >= 0)
        return currentTime - lastTime;
    else
        return ((unsigned long) -1) - lastTime + currentTime;
}


//UART1 Utils------------------------------------
// Write a character to the UART1 Transmitter
#define DATA_REGISTER_EMPTY1 (1<<UDRE1)
#pragma used+

void putchar1(char c) {
    while ((UCSR1A & DATA_REGISTER_EMPTY1) == 0);
    UDR1 = c;
}

#pragma used-

void serial_send1(string text) {
    int i;
    for (i = 0; text[i] != '\0'; ++i) {
        putchar1(text[i]);
    }
}

void serial_send1_counted(string text, unsigned int count) {
    int i;
    for (i = 0; i < count; ++i) {
        putchar1(text[i]);
    }
}

void serial_print_number1(int number) {
    int remain;
    remain = number % 10;
    number /= 10;
    if (number != 0)
        serial_print_number1(number);
    putchar1(remain + '0');
}

void printf1(char flash

*str) {
int i;
for (
i = 0;
str[i] != '\0'; ++i) {
putchar1(str[i]);
}
}


