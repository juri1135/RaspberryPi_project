#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "rpi_1.h"
int socketCANDescriptor;
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame;
int globalCan=0;
int carry=0;
int pre=-1;
int moreEight(char *buf, int len) {
    // 모든 packet에 rpc 고유 id, last packet 식별자, lineNum 붙여서 보내기 

        //last packet(8byte 이하의 packet)이 존재한다면 +1을 해야 하고 존재하지 않는다면 packetTotal 그대로
        //ex) helloworld가 text라면 rpc 고유 id 2, last packet 식별자 하나, lineNum 1 붙여서
        //201hello, 211world 이렇게 두 개의 packet이 전달되어야 함
        //buf에 211helloworld 상태로 넘어오기 때문에 식별자 3개를 제외하고 실제 data만으로 packetTotal이랑 size 계산
        //이 예에선 전체 packet은 2개, last는 0임
        //ex) helloworld~라면 
        //201hello, 201world, 211~ 이렇게 세 개의 packet 전달
        //전체 packet은 2, last는 1로 계산됨. 
        int packetTotal = (len-3) / 5;
        int lastPacketSize = (len-3) % 5;
        //lineNum 저장 
        char linenum=buf[2];
        for (int i = 0; i < packetTotal; i++) {
            //packetTotal은 모두 8바이트로 꽉 찬 packet들만 전달되기 때문에 dlc은 8로 고정하고 
            //그외 식별자도 다 고정해줌 
            frame.can_dlc = 8;
            frame.can_id =carry++;
            globalCan=frame.can_id;
            frame.data[0]='2';
            frame.data[1]='0';
            frame.data[2]=linenum;
            //초반 3바이트는 고정값이고 그 뒤 5바이트만 복사해서 저장해주면 됨. 
            memcpy(frame.data+3,buf+(i*5)+3,5);
            if(i==packetTotal-1&&lastPacketSize==0) frame.data[1]='1';
            // 패킷 데이터 출력
            printf("frame.data[%d]: ", frame.can_dlc);
            for (int j = 0; j < frame.can_dlc; j++) {
                printf("%c", frame.data[j]);
            }
            printf("\n");
            if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
            }
        }
    
        // 마지막 패킷 처리 (남은 데이터가 있을 때만)
        if (lastPacketSize > 0) {
            //길이는 진짜 data에 고유 식별자 3byte 추가가
            frame.can_dlc = lastPacketSize+3;
            frame.can_id =carry++;
            globalCan=frame.can_id;
            frame.data[0]='2';
            frame.data[1] = '1';  // 마지막 패킷 표시
            frame.data[2]=linenum;
            memcpy(frame.data+3,buf+(packetTotal*5)+3,lastPacketSize);
            
            // 마지막 패킷 데이터 출력
            printf("frame.data[%d] (last packet): ", frame.can_dlc);
            for (int j = 0; j < frame.can_dlc; j++) {
                printf("%c", frame.data[j]);
            }
            printf("\n");
            if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
            }
        }
    
        return 0;
    }

        
    int write_can(char *buf, int len) {
        memset(&frame, 0, sizeof(frame));        

        memcpy(frame.data, buf, len);

        if(len>8){
            moreEight(buf, len);
            return 1;
        }
        frame.can_id =carry++;
        globalCan=frame.can_id;
        frame.can_dlc = len;
        printf("frame.data: ");
        for (int i = 0; i < len; i++) {
                printf("%c", frame.data[i]); // frame.data 내용 출력
        }
        printf("\n");
        if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
        }

}
int read_can(){
        int val;
        char receiveMessage[8];
        int nbytesReceived;
        while (1) {
            memset(&frame,0,sizeof(frame));
                nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
                if (nbytesReceived < 0) {
                        perror("Read failed");
                        return -1;
                }
                
                //중복이면 씹기 (loopback mode)
                if(frame.can_id==globalCan||frame.can_id==pre) continue;
                if(frame.can_dlc!=1) continue;
                pre=frame.can_id; 
                printf("id: %d\n",frame.can_id);
                printf("read frame.data: ");
                for (int i = 0; i < frame.can_dlc; i++) {
                        printf("%c", frame.data[i]); // frame.data 내용 출력
                }
                printf("\n");
                memcpy(receiveMessage, (unsigned char *)(frame.data), frame.can_dlc);
                val=receiveMessage[0]-'0';
                
                 return val;
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
        frame.can_id=0x555;
        if (bind(socketCANDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Bind failed");
                return -1;
        }
        printf("RPi #1 is ready.\n\n");
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