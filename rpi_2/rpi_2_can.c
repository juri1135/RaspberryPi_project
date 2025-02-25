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
static int canId;
int read_can(){
    int nbytesReceived;
    int carry=0;
    memset(receivedData,0,128);
    while (1) {
        memset(&frame,0,sizeof(frame));
        nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
        
        if (nbytesReceived < 0) {
            perror("Read failed");
            return -1;
        }
       
        //loopback mode로 인한 중복 데이터 방지지
        if (frame.can_id == prev) continue;
        
        prev=frame.can_id;
        
        int rpc = frame.data[0] - '0';
        //유효한 값인지 검사
        if(frame.data[1]!='1') continue;

        printf("nbytes: %d, dlc: %d\n",nbytesReceived,frame.can_dlc);
        printf("RPC num: %c\n",frame.data[0]);
        printf("frame.data: ");
        for (int i = 0; i < frame.can_dlc; i++) {
                printf("%c", frame.data[i]); // frame.data 내용 출력
        }
        printf("\n");

        //displayText 
        //!어차피 0, 1번째는 다 확인 용도라서 무조건 index 2부터 저장하면 되고 길이는 마지막이면 dlc-2였나면 되고 그 외는 다 5인가 6인가임 
        if (frame.data[0] == '2') { 
            if(carry==0) memcpy(receivedData, frame.data + 2, 5);  // 2번째부터 저장
            else memcpy(receivedData+(carry*8),frame.data,frame.can_dlc);
            carry++;

            // 마지막 패킷인지 확인
            if (frame.data[1] == '1') {
                receivedData[carry * 8+1] = '\0';  // 문자열 끝 처리
                printf("%s\n", receivedData);
                return 2;  // displayText 호출 완료
            }
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
int terminate_can(){
    if (close(socketCANDescriptor) < 0) {
            perror("Close failed");
            return -1;
    }

    return 0;
}
int write_can(char *buf, int len){
    frame.can_id = canId++;
    frame.can_dlc = len;
    memcpy(frame.data, (char *)buf, len);
    // printf("frame.data: ");
        // for (int i = 0; i < len; i++) {
        //         printf("%c", frame.data[i]); // frame.data 내용 출력
        // }
        // printf("\n");
    if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write failed");
            return -1;
    }
    return 0;
}