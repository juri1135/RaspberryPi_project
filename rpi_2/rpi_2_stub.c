#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rpi_2.h"
extern char receivedData[128];
int init_stub(){
    printf("RPi #2 is ready to accept RPC requests.\n\n");
    int val;
    while(1){
        int id = read_can();
        if (id == -1) {
            perror("Read failed");
            continue;  // 오류 발생 시 루프 계속 돌기
        }
        else{
            // read_can에서 id를 받고 여기서 적절한 함수 호출해서 처리한다.
            //처리하고 정상 처리 됐으면 여기서 write_can 호출해서 return값 보내주기 
            //case문 써서 적절한 함수 호출하게 하면 될듯 
            switch (id)
            {
            //motor
            case 0:
            int degree = atoi(receivedData);
            printf("RPC request 'moveMotor(%d)' received and processed.\n",degree);
                
                val = moveMotor(degree);
                
                if (val == -1) {
                    perror("moveMotor failed");
                    continue;
                }

                // 정상 처리 후 return 값 전송
                int data = 0;  // 응답 데이터
                char buf[8];
                int len = sprintf(buf , "%d",data); 
                write_can(buf, sizeof(int));
                break;
            //terminate
            case 1:
                printf("RPC request 'QUIT' command received.\n\n");
                val = terminate_can();

                if (val == -1) {
                    perror("terminate_can failed");
                    continue;
                }

                printf("Terminating RPi #2.\n");
                return 0;
            //lcd
            case 2:
            //!아 linenum이랑 data를 분리해야 함 공백으로 분리해뒀음... 
                int lineNum;
                char text[20];
                char *pos=strchr(receivedData,' ');
                if(pos!=NULL){
                    *pos='\0';
                    lineNum=atoi(receivedData);
                    strcpy(text,pos+1);
                }
                printf("PRC request 'displayText(%d, %s) received and processed.\n\n",lineNum,text);
                printf("%d\n",displayText(lineNum, text));
                break;
                //!wirte can 작성하기!!!!!!!!!!! 
            case -2:
                break;
            default:
                printf("Unknown ID received: %d\n", id);
                break;
            }
        }   
    }
    return 0;
}