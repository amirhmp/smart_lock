

#ifndef CODE_WAIT_H
#define CODE_WAIT_H

unsigned char wait(unsigned long * firstTime, int duration);
void initWait(unsigned long *);  //Pass the timer variable ptr to lib
#endif //CODE_WAIT_H
