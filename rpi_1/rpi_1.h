#define LED_PIN 17
#define time 1000
#define N 10
void* led_control(void* args);
int led_thread();

int displayText(int lineNum,char *text);
int terminateRPC(char *text);
int moveMotor(int inputValue);
void ultrasonic();
void findShortestPath(int source,int destination, char buffer[], int*  len);
int sendCan(const char *command, const char *data1, const char *data2);