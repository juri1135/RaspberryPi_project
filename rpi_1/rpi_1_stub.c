#include <stdio.h>
#include <string.h>
#include "rpi_1.h"
//여기서 RPC 함수 정의 
// practice 6의 8bit 이하 can 통신 코드 참고
int moveMotor(int inputValue){
    char buf[8];
    //buf가 실제 can에 보낼 자료고 여기에 내 id랑 inputVal 인자를 넣어줘야 함. id는 buf 대신 send_can 함수에 인자로 넣어주는 게 편할듯 
    //can frame에 id, length 들어감 id에 함수 id, data에 buf, decl에 strlen 넣기
    //write_can에 id, data, length 넘기기
    memcpy(buf,&inputValue, sizeof(inputValue));
    
    if(write_can(0,buf,strlen(buf))<0) return -1;
    //return값은 성공시 0, 실패하면 -1
    int val=read_can();
    printf("Requested RPC moveMotor() and received return value %d\n",inputValue);
    return val;
}

int terminateRPC(char *text){
    char buf[10];
    memcpy(buf,text,sizeof(text));
    write_can(1,buf,sizeof(buf));
    int val=read_can();
    return val;
}
//morethan8byte면 id에 carry 더해서 loopback시 두 번 출력 문제를 해결해야 됨
// 따라서 displayText를 가장 마지막 id로 지정 
int displayText(int lineNum, char *text){
    char buf[100];
    
    //lineNum과 text 사이에 공백 넣어서 구분할 수 있게 분리 
    sprintf(buf, "%d %s", lineNum, text);
    if(write_can(2,buf,strlen(buf))<0) return -1;
    //return값은 문자열 크기기
    int val=read_can();
    printf("Requested RPC displayText() and received return value %d\n",val);
    return val;
}