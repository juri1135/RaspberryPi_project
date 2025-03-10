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
int idCheck=0;
int wrongId=-2;
int myId=-1;
//! displayText에서 packet 두 개 보내면 이유는 모르겠지만 두 번씩 받아짐 (미해결)
//! packet 3개 이상으로 쪼개지는 경우엔 dlc은 ㄱㅊ은데 data가 아예 깨져서 받아짐 (미해결)
int read_can(){
    int nbytesReceived;
    int carry=0;
    int index=0;
    
    while (1) {
        memset(&frame,0,sizeof(frame));
        nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
        
        if (nbytesReceived < 0) {
            perror("Read failed");
            return -1;
        }
        //!------pkt 등 거르는 과정--------
        printf("myId: %d, id: %d\n",myId,frame.can_id);
       if(frame.can_dlc==1) continue;
       if(frame.can_id==myId) continue;
        if (frame.can_id == prev) continue;
        prev=frame.can_id;
        //!---rpc type 확인---
        int rpc = frame.data[0] - '0';



    
        //!------------------------moveMotor, terminateRPC----------------------------------------------  
        if(rpc==0||rpc==1){
            //!---rpc의 valid bit 확인--
            if(frame.data[1]!='1') continue;
            memcpy(receivedData, frame.data + 2, frame.can_dlc+1);  
            receivedData[frame.can_dlc - 1] = '\0';

            printf("received data: %s\n", receivedData);
            return rpc;
        }
        

        // printf("\nid: %d\n",frame.can_id);
        // printf("frame.data: ");
        // for (int i = 0; i < frame.can_dlc; i++) {
        //         printf("%c", frame.data[i]); // frame.data 내용 출력
        // }
        // printf("\n");

        //!-------------------------displayText----------------------------------------------  
        if (frame.data[0] == '2') { 
            //만약 loopback으로 인해 잘못 수신된 pkt이라면 (wrong pkt은 정상적인 pkt과 id가 비연속적이지만 잘못된 pkt끼리는 id가 연속적임)
            if(frame.can_id==wrongId+1){
                wrongId++;
                continue;
            }
            
            //!--------------8byte 미만이라 packet이 1개인 경우-----------------------------------------
            //index: n개의 pkt 중 몇 번째 pkt인지 체크, frame.data[1]은 last packet인지 여부 
            if(index==0 && frame.data[1]=='1'){
                //!-----lineNum이랑 제일 첫 번째 pkt의 data 복사 
                //carry는 receivedData에 계속 이어 붙여야 해서 check용도. frame.data[2]는 Line number 
                if (carry == 0) receivedData[0] = frame.data[2];
                // 데이터 이어붙이기 (8byte 중 3byte는 rpc num, last packet 여부, linenum)
                memcpy(receivedData + (carry * 5) + 1, frame.data + 3, frame.can_dlc - 3);
                return 2;
            }

            //!-----------packet이 2개 이상인데 그 중 제일 첫 번째 packet인 경우-----------------------------
            //일단 제일 첫 packet은 id 저장 
            if(index==0){idCheck=frame.can_id;}
            //!---------2개 이상의 packet 중에서 첫 번째가 아닌 경우------------------------------------------
            else{
                //만약 valid pkt이 아니라면 wrongId 저장하고 pass
                if(frame.can_id!=idCheck+1){
                    wrongId=frame.can_id;
                    continue;
                }
                //valid pkt이라면 valid id 저장
                idCheck=frame.can_id;
            }
            //carry는 receivedData에 계속 이어 붙여야 해서 check용도. frame.data[2]는 Line number 
            if (carry == 0) receivedData[0] = frame.data[2];
            // 데이터 이어붙이기 (8byte 중 3byte는 rpc num, last packet 여부, linenum)
            memcpy(receivedData + (carry * 5) + 1, frame.data + 3, frame.can_dlc - 3);

            //!-----2개 이사으마지막 패킷인지 확인
            if (frame.data[1] == '1') return 2;  
            
            //잘못된 pkt은 다 위에서 걸렀으니 이 pkt은 정상 처리됐음을 알리기 위해 index, cary 1씩 증가 
            index++; carry++;
            continue;  // 계속 다음 패킷 받기
        }
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
    //자기가 write한 pkt은 거르기 위해 저장 
    myId=frame.can_id;
    frame.can_dlc = len;
    memcpy(frame.data, (char *)buf, len);
    // printf("id: %d\n",frame.can_id);
    // printf("frame.data (raw bytes): ");
    // for (int i = 0; i < len; i++) {
    //     printf("%c ", (unsigned char)frame.data[i]); // 16진수로 출력
    // }
    // printf("\n");
    if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write failed");
            return -1;
    }
    return 0;
}