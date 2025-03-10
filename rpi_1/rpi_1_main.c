#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include "rpi_1.h"
#include <pthread.h>
int trig=0;
int thread=1;
pthread_t ledthread;
pthread_t ultrathread;
int socketCANDescriptor;
int main(){
  //coucurrent execution을 위해 multithread programming... 
  //실습3 monte carlo code 참고 
    //wiringPi, CAN initialization
    wiringPiSetupGpio();
    init_can();

    // step 1 LED 점멸 시작
    led_thread();
    
    // step 2 
    int source = 2; // 시작 노드 (S)
    int destination = 5; // 도착 노드 (D)
    char buf[128]="";
    int len;
    findShortestPath(source, destination, buf, &len);

     //step 3 초음파 센서 
     ultrasonic_thread();

    displayText(1,buf);
    
    // step 4
    // rotate Motor (180) and rerotate (0) using RPC 
    if(moveMotor(180)==-1) printf("error moveMotor\n");
    delay(2000);
    if(moveMotor(0)==-1) printf("error moveMotor\n");
    
    //step 5
    while(1){
      char text[128];

      printf("Enter your text to display on RPI #2's LCD: ");
      fgets(text,sizeof(text),stdin);
      if (text[strlen(text) - 1] == '\n') {
        text[strlen(text) - 1] = '\0';
    }
      if(strcmp(text,"quit")==0){
        if(terminateRPC(text)==0){
          printf("Terminating RPi #1.\n");
          
            thread=0;
            //cancel: thread를 종료시키는 신호 전달
            pthread_cancel(ultrathread);
            //join 종료를 기다렸다가 종료된 thread의 자원 회수 및 반환값 받음 
            pthread_join(ultrathread, NULL);
            pthread_cancel(ledthread);
            pthread_join(ledthread, NULL);
             terminate_can();

          return 0;
        }
      }
      else displayText(1,text);
     }
     
}
