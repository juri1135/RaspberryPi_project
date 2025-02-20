#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include "rpi_1.h"
#include <pthread.h>
int trig=0;
pthread_t ledthread;
pthread_t ultrathread;
int main(){
  //coucurrent execution을 위해 multithread programming... 
  //실습3 monte carlo code 참고 
    //wiringPi, CAN initialization
    wiringPiSetupGpio();
    init_can();

    // step 1 LED 점멸 시작
    led_thread();
    //step 3 초음파 센서 
    ultrasonic_thread();
    // step 2 
    int source = 2; // 시작 노드 (S)
    int destination = 5; // 도착 노드 (D)
    char buf[128]="";
    int len;
    findShortestPath(source, destination, buf, &len);
    displayText(1,buf);

    
    
    // step 4
    // rotate Motor (180) and rerotate (0) using RPC 
    if(moveMotor(180)==-1) printf("error moveMotor\n");
    delay(1000);
    if(moveMotor(0)==-1) printf("error moveMotor\n");
    
    //step 5
    while(1){
      char text[128];

      printf("Enter your text to display on RPI #2's LCD: ");
      fgets(text,sizeof(text),stdin);
      if(strcmp(text,"quit")==0){
        if(terminateRPC(text)==0){
          printf("Terminating RPi #1.\n");
          terminate_can();
          break;
        }
      }
      else displayText(1,text);
     }
    pthread_join(ledthread,NULL);
    pthread_join(ultrathread,NULL);
    //wiringpi 종료료
    return 0;
}
