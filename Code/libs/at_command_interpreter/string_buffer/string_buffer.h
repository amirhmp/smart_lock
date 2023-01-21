#ifndef SIM800L_SIMULATION_STRING_BUFFER_H
#define SIM800L_SIMULATION_STRING_BUFFER_H

void buffer_append_char(unsigned char ch);
void buffer_remove_range(int range);
void buffer_clear();
char *buffer_substring(int startIndex, int endIndex);
void buffer_substring2(char *substr, int startIndex, int endIndex);
unsigned int buffer_length();
void buffer_print();
int buffer_index_of(char *pattern, int fromIndex);

void buffer_remove_range_async(int range);
void buffer_clear_async();

char buffer_at(int index);
#endif // SIM800L_SIMULATION_STRING_BUFFER_H
