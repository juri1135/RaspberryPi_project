#include <wiringPi.h>
#include <stdio.h>
#include <pthread.h>
#include "rpi_1.h"
extern int trig;
//monte carlo의 simulateMonteCarlo func 참고
void* led_control(void* arg) {
    while (1) {
        //distance가 20보다 클 때만 1초 간격  점멸멸
        if(trig==0){
            digitalWrite(LED_PIN, 1);
            delay(1000);
            digitalWrite(LED_PIN, 0);
            delay(1000);
        }
    }
    return NULL;
}
void led_thread(){
        printf("Started LED.\n\n");
        pthread_t thread;
        pthread_create(&thread,NULL,led_control,NULL);
}