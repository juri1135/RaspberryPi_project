#define LED_PIN 17
#define N 10
#define Trig    23
#define Echo    24
#define Detected_DEVICE_ID_BY_I2C       0x27        // Device ID detected by I2C 
                                                    // Seems that it maps to the device's address
#define LCD_BACKLIGHT_ON                0x08        // On             "0000 1000"
#define LCD_BACKLIGHT_OFF               0x00        // Off            "0000 0000"
#define LCD_ENABLE                      0x04        // Enable         "0000 0100"
#define LCD_DISABLE                     0x00        // Disable        "0000 0000"
#define LCD_RW_READ                     0x02        // Read           "0000 0010" 
#define LCD_RW_WRITE                    0x00        // Write          "0000 0000"
#define LCD_RS_DATA                     0x01        // Data           "0000 0001"
#define LCD_RS_INST                     0x00        // Instruction    "0000 0000"
#include <pthread.h>


// extern pthread_t ledthread;
// extern pthread_t ultrathread;
//led.c
void* led_control(void* args);
void led_thread();
//ultrasonic.c
float measureDistance();
void* ultrasonic_control(void* args);
void ultrasonic_thread();
//stub.c
int displayText(int lineNum,char *text);
int terminateRPC(char *text);
int moveMotor(int inputValue);
//can.c
int moreEight(char *buf, int len);
int write_can(char *buf, int len);
int read_can();
int init_can();
int terminate_can();
//dijkstra.c
void findShortestPath(char source, char destination, char buffer[], int* len);
void build_path(int dest, char buffer[], int* len);
int main();