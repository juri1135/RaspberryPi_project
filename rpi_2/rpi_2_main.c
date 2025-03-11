#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include "rpi_2.h"
#include <pthread.h>
int main(){
    wiringPiSetupGpio();
    init_can();
    initializeLCD();
    init_motor();
    init_stub();
    return 0;
}
