


unsigned long *curTime;

void initWait(unsigned long *currentTimePtr) {
    curTime = currentTimePtr;
}

/*firstTime yek variable hast ke user bayad tarif konad ==> duration=curTime-firstTime*/
/*in tabe be dard estefade dar loop mikhorad, va bar aval ke ejra mishavad ehtemal darad dorost kar nakonad chun firstTime set nashode*/
/*dar mavaredi ke niaz be estefade kharej az loop va dar event ha darim mesle dar onDeepHang khodemun bayad dasti firstTime ro set konim*/
unsigned char wait(unsigned long *firstTime, int duration) {
    unsigned long diff;
    if ((long) (*curTime - *firstTime) >= 0)
        diff = *curTime - *firstTime;
    else
        diff = ((unsigned long) -1) - *firstTime + *curTime;
    if (diff >= duration) {
        *firstTime = *curTime;
        return 1;
    }
    return 0;
}
/*
 * curTime =    30000
 * firstTime =  0
 * duration =   5000
 * */

