#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "rpi_2.h"
int socketCANDescriptor;
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame;
char receivedData[128];
static int prev=-1;
static int cId=0x55;
//! displayText에서 packet 두 개 보내면 이유는 모르겠지만 두 번씩 받아짐 (미해결)
//! packet 3개 이상으로 쪼개지는 경우엔 dlc은 ㄱㅊ은데 data가 아예 깨져서 받아짐 (미해결)
int read_can(){
    int nbytesReceived;
    int carry=0;
    int check=0;
    int prePkt=-1;
    
    while (1) {
        memset(&frame,0,sizeof(frame));
        nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
        
        if (nbytesReceived < 0) {
            perror("Read failed");
            return -1;
        }
       if(frame.can_dlc==1) continue;
        //loopback mode로 인한 중복 데이터 방지지
        if (frame.can_id == prev) continue;
        prev=frame.can_id;
        
        int rpc = frame.data[0] - '0';
        //유효한 값인지 검사
        if(rpc==0&&frame.data[1]!='1') continue;
        printf("\nid: %d\n",frame.can_id);
        printf("nbytes: %d, dlc: %d\n",nbytesReceived,frame.can_dlc);
        printf("frame.data: ");
        for (int i = 0; i < frame.can_dlc; i++) {
                printf("%c", frame.data[i]); // frame.data 내용 출력
        }
        printf("\n");

        //displayText 
        //!어차피 0, 1번째는 다 확인 용도라서 무조건 index 2부터 저장하면 되고 길이는 마지막이면 dlc-2였나면 되고 그 외는 다 5인가 6인가임 
        
        if (frame.data[0] == '2') { 
            // if(check==0) prePkt=frame.can_id-1;
            // else if((prePkt+1)!=frame.can_id) continue;
            // prePkt=frame.can_id;


            //만약 packet이 1개짜리라면 그냥 data 받아서 return 
            if(check==0 && frame.data[1]=='1'){
                // Line number 저장
                if (carry == 0) receivedData[0] = frame.data[2];
                
                // 데이터 이어붙이기
                memcpy(receivedData + (carry * 5) + 1, frame.data + 3, frame.can_dlc - 3);
            
                printf("%s\n",receivedData);
                printf("text: %s\n", receivedData);
                
                return 2;
            }
            check++;
            printf("check: %d\n",check);
            
            if(check%2==1) continue;
            // Line number 저장
            if (carry == 0) receivedData[0] = frame.data[2];
            
            // 데이터 이어붙이기
            memcpy(receivedData + (carry * 5) + 1, frame.data + 3, frame.can_dlc - 3);
           
            printf("%s\n",receivedData);
            // 마지막 패킷인지 확인
            if (frame.data[1] == '1') {
                
                printf("text: %s\n", receivedData);
                
                return 2;  // displayText 호출 완료
            }
            carry++;
            continue;  // 계속 다음 패킷 받기
        }
        

        memcpy(receivedData, frame.data + 2, frame.can_dlc+1);  // data[1]부터 저장 (data[0]은 함수 ID)
        receivedData[frame.can_dlc - 1] = '\0';  // 문자열 끝 처리
        
        printf("received data: %s\n", receivedData);
        return rpc;
    }
    
}
int init_can(){
        
    if ((socketCANDescriptor = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Socket creation failed.");
            return -1;
    }

    strcpy(ifr.ifr_name, "can0" );
    ioctl(socketCANDescriptor, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    
    addr.can_family = AF_CAN;        
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketCANDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Bind failed");
            return -1;
    }

    return 0;
}
int terminate_can() {
    if (socketCANDescriptor < 0) {  // 이미 닫혔거나 유효하지 않은 경우
        printf("CAN socket already closed or invalid.\n");
        return -1;
    }

    if (close(socketCANDescriptor) < 0) {
        perror("Close failed");
        return -1;
    }

    // 닫은 후에는 descriptor를 -1로 설정해서 재사용 방지
    socketCANDescriptor = -1;

    return 0;
}
int write_can(char *buf, int len){
    frame.can_id = cId++;
    frame.can_dlc = len;
    memcpy(frame.data, (char *)buf, len);
    printf("frame.data (raw bytes): ");
    for (int i = 0; i < len; i++) {
        printf("%c ", (unsigned char)frame.data[i]); // 16진수로 출력
    }
    printf("\n");
    if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write failed");
            return -1;
    }
    return 0;
}