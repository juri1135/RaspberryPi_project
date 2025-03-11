

#define PIN 18
#define PWM_CLOCK_DIVISOR 384   // This will generate 50,000 Hz from 19.2 MHz clock source
#define PWM_RANGE 1024          // The total number of clock ticks during a period
                       

//can.c
int write_can(char *buf, int len);
int read_can();
int init_can();
int terminate_can();

//motor.c
int moveMotor(int degree);
void init_motor();

//stub.c
int init_stub();

//lcd.c
int displayText(int lineNum, char *text);
void initializeLCD();
void sendBitsToLCD(int eightBits, int mode);
void toggleLCDEnable(int eightBits);
