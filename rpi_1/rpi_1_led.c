#include <wiringPi.h>
#include <stdio.h>
#include <pthread.h>
#include "rpi_1.h"
extern volatile int trig;
extern pthread_t ledthread;
//monte carlo의 simulateMonteCarlo func 참고
void* led_control(void* arg) {
    while (1) {
        //distance가 20보다 클 때만 1초 간격  점멸멸
        if(trig==0){printf("LED 0.\n\n");
            digitalWrite(LED_PIN, 1);
            delay(1000);
            digitalWrite(LED_PIN, 0);
            delay(1000);
        }
        else{
            printf("1 LED.\n\n");
            digitalWrite(LED_PIN, 1);
            delay(100);
            digitalWrite(LED_PIN, 0);
            delay(100);
        }
    }
    return NULL;
}
void led_thread(){
    pinMode(LED_PIN,1);
    printf("Started LED.\n\n");
    pthread_create(&ledthread,NULL,led_control,NULL);
}