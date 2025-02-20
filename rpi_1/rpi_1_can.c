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
int moreEight(int id, char *buf, int len){     
        int packetTotal=len/8;                
        int lastPacketSize=len%8;  
        int carry=0;            
        frame.can_id=id;

        for (int i = 0; i < packetTotal; i++) {
            frame.can_dlc = 8;
            frame.can_id+=carry;
            memcpy(frame.data, buf + (i * 8), 8);
            
            if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
            }
            carry++;
        }
        //8byte씩 전달하고 남은 게 있다면 그거 처리
        if(lastPacketSize>0){
                frame.can_dlc = lastPacketSize;
                frame.can_id+=carry;
            memcpy(frame.data, buf + (packetTotal * 8), 8);
            
            if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
            }
        }

        return 0;
}
int write_can(int id, char *buf, int len){
        frame.can_id = id;
        //moveMotor랑 terminate는 8 넘을 일 없음 
        if(len>8){
                moreEight(id,buf,len);
                return 0;
        }
        frame.can_dlc = len;
        memcpy(frame.data, (char *)buf, 8);
        if (write(socketCANDescriptor, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
                perror("Write failed");
                return -1;
        }

}
int read_can(){
        int val;
        char receiveMessage[8];
        int nbytesReceived;
        int prev = -1;
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

