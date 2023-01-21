#include <stdio.h>
#include <stdlib.h>
#include "../utils/at_utils.h"
#include "alcd.h"
#include "delay.h"

#define BUFFER_SIZE 256
unsigned char string_buffer[BUFFER_SIZE];

unsigned int buff_count = 0;
int buff_rPtr = 0;
int buff_wPtr = -1;

#define QUEUE_IS_NOT_FULL (buff_count < BUFFER_SIZE)
#define QUEUE_IS_NOT_EMPTY (buff_count > 0)

void buffer_append_char(unsigned char ch)
{
    if (QUEUE_IS_NOT_FULL)
    {
        ++buff_count;
        if (++buff_wPtr >= BUFFER_SIZE)
            buff_wPtr = 0;
        string_buffer[buff_wPtr] = ch;
    }
}

void buffer_remove_range(int range)
{
    /*be tedad range az aval string_buffer hazf mikone*/
    if (buff_count >= range)
    {
        buff_count -= range;
        buff_rPtr += range;
        if (buff_rPtr >= BUFFER_SIZE)
            buff_rPtr = buff_rPtr - BUFFER_SIZE - 1;
    }
}

void buffer_remove_range_async(int range)
{
    DISABLE_INTERRUPTS
    buffer_remove_range(range);
    ENABLE_INTERRUPTS
}

void buffer_print()
{
    int i, normalized_index;
    for (i = buff_rPtr; i < buff_rPtr + buff_count; ++i)
    {
        if (i < BUFFER_SIZE)
            normalized_index = i;
        else
            normalized_index = i - BUFFER_SIZE;
        putchar1(string_buffer[normalized_index]);
    }
    putchar1(13);
}

int normalize_output_index(
    int index)
{ /*indexi ke be birun az ketabkhane mire az 0 ta string_buffer
   *.length has, ama dar amal chun yek Queue darim ke ba array piayde sazi shode index ha haman index haye array hast, index haye majazi ma az rPtr=0 ta wPtr=? mibashad, yaani rPtr be har khane ee az array eshare kone un khune index sefre majazi mibashad*/
    if (index == -1)
        return -1;
    else if (buff_rPtr < buff_wPtr)
    {
        return index - buff_rPtr;
    }
    else
    {
        int normalized = index - buff_rPtr;
        if (normalized < 0)
            return normalized + BUFFER_SIZE;
        else
            return normalized;
    }
}

int normalize_input_index(int index)
{ // Baraxe normalize_output_index
    if (index == -1)
        return -1;
    else if (buff_rPtr < buff_wPtr)
    {
        return index + buff_rPtr;
    }
    else
    {
        int normalized = index + buff_rPtr;
        if (normalized < BUFFER_SIZE)
            return normalized;
        else
            return normalized - BUFFER_SIZE;
    }
}

int buffer_index_of(char *pattern, int fromIndex)
{
    int i, j, patternSize, normalized_from_index;
    patternSize = str_len(pattern);
    if (patternSize > buff_count)
        return -1;
    if (fromIndex > normalize_output_index(buff_wPtr))
        return -1;
    normalized_from_index = normalize_input_index(fromIndex);
    if (buff_rPtr < buff_wPtr)
    {
        for (i = normalized_from_index; i <= buff_wPtr - patternSize + 1; ++i)
        {
            for (j = 0; j < patternSize; ++j)
            {
                if (pattern[j] != string_buffer[i + j])
                    break;
            }
            if (j == patternSize)
                return normalize_output_index(i);
        }
        return -1;
    }
    else
    {
        for (i = normalized_from_index; i <= buff_rPtr + buff_count - patternSize; ++i)
        { // todo max rPtr , startPoint
            for (j = 0; j < patternSize; ++j)
            {
                int normalized_index = i + j;
                if (normalized_index >= BUFFER_SIZE)
                    normalized_index = normalized_index - BUFFER_SIZE;
                if (pattern[j] != string_buffer[normalized_index])
                    break;
            }
            if (j == patternSize)
            {
                if (i < BUFFER_SIZE)
                    return normalize_output_index(i);
                else
                    return normalize_output_index(i - BUFFER_SIZE);
            }
        }
    }
    return -1;
}

unsigned int buffer_length()
{
    return buff_count;
}

char buffer_at(int index)
{
    int real_start_index = normalize_input_index(index);
    return string_buffer[real_start_index];
}

string buffer_substring(int startIndex, int endIndex)
{
    int i, real_start_index, real_end_index, j;
    string substr;
    if (startIndex == -1 || endIndex == -1)
        return NULL;
    if (startIndex >= endIndex)
        return NULL;
    endIndex = MIN(endIndex, buff_count);
    substr = malloc(sizeof(char) * (endIndex - startIndex + 2)); //+2 is for \0
    if (substr == NULL)
    {
        serial_send1("error allocating memory in buffer_substring");
        return NULL;
    }
    real_start_index = normalize_input_index(startIndex);
    real_end_index = normalize_input_index(endIndex);
    if (real_end_index > real_start_index)
    {
        for (i = real_start_index; i < real_end_index; ++i)
            substr[i - real_start_index] = string_buffer[i];
        substr[i - real_start_index] = '\0';
    }
    else
    {
        for (i = real_start_index; i < buff_count; ++i)
            substr[i - real_start_index] = string_buffer[i];
        i -= real_start_index;
        for (j = 0; j < real_end_index; ++j)
            substr[j + i] = string_buffer[j];
        substr[j + i] = '\0';
    }
    return substr;
}

/*
void buffer_substring2(char *substr, int startIndex, int endIndex)
{
    int i, real_start_index, real_end_index, j;
    if (startIndex == -1 || endIndex == -1)
    {
        substr[0] = '\0';
        return;
    }
    if (startIndex >= endIndex)
    {
        substr[0] = '\0';
        return;
    }
    endIndex = MIN(endIndex, buff_count);

    real_start_index = normalize_input_index(startIndex);
    real_end_index = normalize_input_index(endIndex);
    if (real_end_index > real_start_index)
    {
        for (i = real_start_index; i < real_end_index; ++i)
            substr[i - real_start_index] = string_buffer[i];
        substr[i - real_start_index] = '\0';
    }
    else
    {
        for (i = real_start_index; i < buff_count; ++i)
            substr[i - real_start_index] = string_buffer[i];
        i -= real_start_index;
        for (j = 0; j < real_end_index; ++j)
            substr[j + i] = string_buffer[j];
        substr[j + i] = '\0';
    }
}
*/

 void buffer_substring2(char *substr, int startIndex, int endIndex)
{
    int i;
    for (i = startIndex; i <= endIndex; ++i)
    {
        char x = buffer_at(i);
        substr[i - startIndex] = x;
    }
    substr[i - startIndex] = '\0';
}

void buffer_clear()
{
    buff_count = 0;
    buff_rPtr = 0;
    buff_wPtr = -1;
}

void buffer_clear_async()
{
    DISABLE_INTERRUPTS
    buffer_clear();
    ENABLE_INTERRUPTS
}