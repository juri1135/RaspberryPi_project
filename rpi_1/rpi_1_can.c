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
        printf("buf data (displayText): ");
        for (int i = 0; i < len; i++) {
            printf("%c", buf[i]); 
        }
        printf("\n");
    
        int packetTotal = len / 8;
        int lastPacketSize = len % 8;
    
        for (int i = 0; i < packetTotal; i++) {
            frame.can_dlc = 8;
            frame.can_id = canId++;
    
            memcpy(frame.data, buf + (i * 8), 8);
    
            // 마지막 8바이트 패킷이면 frame.data[1] = 1 설정
            if (i == packetTotal - 1 && lastPacketSize == 0) {
                frame.data[1] = 1;
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
            frame.can_dlc = lastPacketSize;
            frame.can_id = canId++;
    
            memcpy(frame.data, buf + (packetTotal * 8), lastPacketSize);
    
            frame.data[1] = 1;  // 마지막 패킷 표시
    
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
