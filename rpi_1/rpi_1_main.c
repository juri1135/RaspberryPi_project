#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include "rpi_1.h"
#include <pthread.h>
int trig=0;
int main(){
  //coucurrent execution을 위해 multithread programming... 
  //실습3 monte carlo code 참고 
    //wiringPi, LED initialization
    wiringPiSetupGpio();
    pinMode(LED_PIN,1);
    
    // step 1 
    led_thread();
    ultrasonic_thread();
    can_init();

    // step 2 
    int source = 2; // 시작 노드 (S)
    int destination = 5; // 도착 노드 (D)
    char buf[128]="";
    int len;
    findShortestPath(source, destination, buf, &len);
    
    displayText(1,buf);

    // step 3
    ultrasonic();
    
    // step 4
    if(moveMotor(180)==-1) printf("error\n");
    
    
		// step 5
		char text[128];
    while(1){
      printf("Enter your text to display on RPI #2's LCD: ");
      fgets(text,sizeof(text),stdin);
      if(strcmp(text,"quit")==0){
        if(sendCommand("quit","","")==-1) break;
        else printf("Terminating Rpi #1.\n");
      }
      else sendCommand("LCD","1",text);
	  }
    
}
