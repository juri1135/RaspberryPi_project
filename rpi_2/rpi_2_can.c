#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
        if (nbytesReceived < 0) {
            perror("Read failed");
            return -1;
        }
        //printf("Reading frame with can_id: %d\n", frame.can_id);  // 디버깅용 출력
        
        if (frame.can_id == prev) continue;
        
        prev=frame.can_id;
        // displayText라면 여러 패킷이 올 수 있음 (id >= 2)
        if (frame.data[0] == 2) {  
            if(carry==0) memcpy(receivedData, frame.data + 2, 6);  // 2번째부터 저장
            else memcpy(receivedData+(carry*8),frame.data,frame.can_dlc);
            carry++;

            // 마지막 패킷인지 확인
            if (frame.data[1] == 1) {
                receivedData[carry * 8+1] = '\0';  // 문자열 끝 처리
                printf("%s\n", receivedData);
                return 2;  // displayText 호출 완료
            }
            continue;  // 계속 다음 패킷 받기
        }
        
        memcpy(receivedData, frame.data + 1, frame.can_dlc - 1);  // data[1]부터 저장 (data[0]은 함수 ID)
        receivedData[frame.can_dlc - 1] = '\0';  // 문자열 끝 처리
        
        if(frame.data[0]!=0 &&frame.data[0]!=1&&frame.data[0]!=2) return -2;
        printf("received data: %s\n", receivedData);
        return frame.data[0];
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