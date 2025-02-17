#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>

#define Trig    23
#define Echo    24
#define LED     17

float measureDistance(void) {
    struct timeval tv1, tv2;
    long time1, time2;

    // 초음파 펄스 발신
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);

    // echo가 high가 될 때까지 대기 (타임아웃 200ms 추가)
    gettimeofday(&tv1, NULL);
    while (digitalRead(Echo) == 0) {
        struct timeval now;
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - tv1.tv_sec) * 1000000 + (now.tv_usec - tv1.tv_usec);
        if (elapsed > 200000) { // 200ms 초과 시 타임아웃
            return -1.0; // 오류 시 -1 반환
        }
    }

    // echo가 low가 될 때까지 대기
    gettimeofday(&tv1, NULL);
    while (digitalRead(Echo) == 1) {
        struct timeval now;
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - tv1.tv_sec) * 1000000 + (now.tv_usec - tv1.tv_usec);
        if (elapsed > 200000) { // 200ms 초과 시 타임아웃
            return -1.0; // 오류 시 -1 반환
        }
    }

    // 시간 차이로부터 거리 계산
    gettimeofday(&tv2, NULL);
    time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;
    time2 = tv2.tv_sec * 1000000 + tv2.tv_usec;
    return (time2 - time1) / 58.0; // us -> cm
}

void ultrasonic() {
    float distance;

    // WiringPi 초기화
    if (wiringPiSetupGpio() == -1) {
        printf("WiringPi 초기화 실패\n");
        return;
    }

    // 핀 모드 설정
    pinMode(Echo, INPUT);
    pinMode(Trig, OUTPUT);
    pinMode(LED, OUTPUT);

    digitalWrite(Trig, LOW); // Trig 핀 초기화
    delay(30);

    while (1) {
        distance = measureDistance();

        if (distance == -1.0) {
            printf("센서 타임아웃 발생\n");
        } else {
            printf("거리: %.2f cm\n", distance);

            if (distance <= 20.0) {
                // LED를 10Hz로 점멸 (50ms ON, 50ms OFF)
                for (int i = 0; i < 10; i++) {
                    led1(LED, 50); // LED 점멸
                }
            }
        }

        delay(1000); // 1초 간격으로 거리 측정
    }
}
