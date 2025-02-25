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
static int canId;
static int prev=-1;
int moreEight(char *buf, int len) {
    //! 계속 data[0]에 2 넣고 data[1]은 last엔 1, 그 외는 0 넣어서 보내기 
        printf("buf data (displayText): ");
        for (int i = 0; i < len; i++) {
            printf("%c", buf[i]); 
        }
        printf("\n");
    //! 여기서 초기 buf에서 진짜 data를 잘라내야 됨 
        //last packet(8byte 이하의 packet)이 존재한다면 +1을 해야 하고 존재하지 않는다면 packetTotal 그대로
        int packetTotal = (len-3) / 5;
        int lastPacketSize = (len-3) % 5;
        printf("packetTotal: %d, lastSize:%d\n",packetTotal,lastPacketSize);
        char linenum=buf[2];
        printf("linenum: %d\n",linenum);
        for (int i = 0; i < packetTotal; i++) {
            frame.can_dlc = 8;
            frame.can_id = canId++;
            frame.data[0]='2';
            frame.data[1]='0';
            frame.data[2]=linenum;
            memcpy(frame.data+3,buf+(i*5)+3,5);
        
            if(i==packetTotal-1&&lastPacketSize==0){
                frame.data[1]='1';
                printf("frame.data[%d] (last packet): ", frame.can_dlc);
                for (int j = 0; j < frame.can_dlc; j++) {
                    printf("%c", frame.data[j]);
                }
                printf("\n");
        
                if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                    perror("Write failed");
                    return -1;
                }
                return 0;
            }
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
            frame.can_dlc = lastPacketSize+3;
            frame.can_id = canId++;
    
            memcpy(frame.data+3,buf+(packetTotal*5)+3,lastPacketSize);
            frame.data[0]='2';
            frame.data[1] = '1';  // 마지막 패킷 표시
            frame.data[2]=linenum;
            //snprintf(frame.data + 2, 90, "%d %s", linenum, buf+(i*6)+1);
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
int write_can(char *buf, int len){
        
        frame.can_id = canId++;
        //moveMotor랑 terminate는 8 넘을 일 없음 
        

        frame.can_dlc = len;
        // printf("buf (write_can): ");
        // for (int i = 0; i < len; i++) {  // buf의 내용 출력
        //         printf("%c", buf[i]);
        // }
        // printf("\n");

        memcpy(frame.data, buf, len);
        if(len>8) return moreEight(buf, len);
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
                nbytesReceived = read(socketCANDescriptor, &frame, sizeof(struct can_frame));
                if (nbytesReceived < 0) {
                        perror("Read failed");
                        return -1;
                }
                //중복이면 씹기 (loopback mode)
                if(frame.can_id==prev) continue;
                prev=frame.can_id;   

                memcpy(receiveMessage, (unsigned char *)(frame.data), frame.can_dlc);
                receiveMessage[frame.can_dlc] = '\n';
                //printf("%s", receiveMessage);
                val=atoi(receiveMessage);
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

        if (bind(socketCANDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Bind failed");
                return -1;
        }
        printf("RPi #1 is ready.\n\n");
        return 0;
}
int terminate_can(){
        if (close(socketCANDescriptor) < 0) {
            perror("Close failed");
            return -1;
        }
        return 0;
    }
