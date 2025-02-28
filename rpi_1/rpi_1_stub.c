#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "rpi_1.h"
//여기서 RPC 함수 정의 
// practice 6의 8byte 이하 can 통신 코드 참고
int moveMotor(int inputValue){
    char buf[100];
    //buf가 실제 can에 보낼 자료고 여기에 내 id랑 inputVal 인자를 넣어줘야 함. id는 buf 대신 send_can 함수에 인자로 넣어주는 게 편할듯 
    //can frame에 id, length 들어감 id에 함수 id, data에 buf, decl에 strlen 넣기
    //write_can에 id, data, length 넘기기
    //data의 첫 번째 byte를 함수 고유 id로 지정
    buf[0]='0';
    buf[1]='1';
    int len = sprintf(buf + 2, "%d", inputValue); // buf에 값 저장

    if (write_can(buf, len + 2) < 0) {
        return -1;
    }
    //return값은 성공시 0
    delay(100); 
    int val=read_can();
    printf("Requested RPC moveMotor() and received return value %d\n\n",inputValue);
    return val;
}

int terminateRPC(char *text){
    char buf[10];
    buf[0]='1';
    buf[1]='1';
    strncpy(buf + 2, text, strlen(text));
    if (write_can(buf, strlen(buf) + 2) < 0) return -1;
    delay(1000); 
    int val=read_can();
    printf("val: %d\n",val);
    return val;
}
//morethan8byte면 id에 carry 더해서 loopback시 두 번 출력 문제를 해결해야 됨
// 따라서 displayText를 가장 마지막 id로 지정 
int displayText(int lineNum, char *text){
    int buf_size = 100 + strlen(text); 
    char buf[100];
    
    buf[0] = '2';
    //마지막 패킷인지 구분하기 위함
    buf[1] = '1';

    int len = snprintf(buf + 2, buf_size - 2, "%d%s", lineNum, text);
\
    //lineNum과 text 사이에 공백 넣어서 구분할 수 있게 분리 
    if (write_can(buf, len + 2) < 0) return -1;
    delay(100); 
    //return값은 문자열 크기기
    //int val=read_can();
    int val=1;
    printf("Requested RPC displayText() and received return value %d\n\n",val);
    return val;
}