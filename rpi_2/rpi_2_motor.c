#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include "rpi_2.h"

int moveMotor(int degree)
{
    float dutyCycle;        // the unit is %
    int pulseWidth;         
    
    /////////////////////////////////////////////////////////////////////
    // Insert your code here
    
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
    pwmWrite(PIN,0);
    return 0;
    /////////////////////////////////////////////////////////////////////
}
void init_motor(){
    pinMode(PIN,PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(PWM_CLOCK_DIVISOR);
    pwmSetRange(PWM_RANGE);
    pinMode(PIN,PWM_OUTPUT);
}