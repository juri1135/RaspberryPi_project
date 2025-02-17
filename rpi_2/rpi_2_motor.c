#include <stdio.h>
#include <wiringPi.h>
#include <time.h>

#define PIN 18
#define PWM_CLOCK_DIVISOR 384   // This will generate 50,000 Hz from 19.2 MHz clock source
#define PWM_RANGE 1024          // The total number of clock ticks during a period
                                // This is equivalent to a period of 20 ms

int moveMotor(int degree)
{
    float dutyCycle;        // the unit is %
    int pulseWidth;         
    pinMode(PIN,PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(PWM_CLOCK_DIVISOR);
    pwmSetRange(PWM_RANGE);
    /////////////////////////////////////////////////////////////////////
    // Insert your code here
    pinMode(PIN,PWM_OUTPUT);
    // Insert your code here
    dutyCycle=5.0+((float)degree/180)*5;
    float mul=PWM_RANGE/100;
    pulseWidth=(int)(mul*dutyCycle);

    if(pulseWidth<0 || pulseWidth >PWM_RANGE) return -1;
    pwmWrite(PIN,pulseWidth);
    delay(1000);

    dutyCycle = 5.0; 
    pulseWidth = (int)((PWM_RANGE / 100.0) * dutyCycle);
    pwmWrite(PIN, pulseWidth);
    delay(1000);
    pwnWrite(PIN,0);
    return 0;
    /////////////////////////////////////////////////////////////////////
}
