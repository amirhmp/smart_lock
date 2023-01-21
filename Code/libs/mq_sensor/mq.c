#include "../init/init.h"

#define MAX_AVG_COUNT 100

unsigned long sum = 0;
unsigned int mv_avg_counter = 0;
unsigned int firstValueIndex = 0;
unsigned int values[MAX_AVG_COUNT];

unsigned int read_mq_sensor() { // reading sensor value using moving average algo
    unsigned int newValue;
    newValue = read_adc(MQ_SENSOR_PIN);

    if (mv_avg_counter < MAX_AVG_COUNT) {
        values[mv_avg_counter] = newValue;
        mv_avg_counter++;
        sum += newValue;
        return sum / mv_avg_counter;
    } else {  // array is filled with values, it's time to replace first value with newValue (latest value)
        unsigned int firstValue = values[firstValueIndex];
        values[firstValueIndex] = newValue;
        firstValueIndex++;
        if (firstValueIndex == MAX_AVG_COUNT) firstValueIndex = 0;

        sum = sum - firstValue + newValue;
        return sum / MAX_AVG_COUNT;
    }
}