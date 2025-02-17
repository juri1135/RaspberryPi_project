#include <stdio.h>
#include <string.h>
int socketCAN(int id, char data[], int dlc);

int sendCommand(const char *command, const char *data1, const char *data2){
    char serialData[64];
    if(strcmp(command,"quit")==0){
        terminateRPC("quit");
    }
}