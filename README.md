# RaspberryPi_project
Distributed Control System over CAN

라즈베리파이를 이용한 간단한 프로젝트

LED, LCD, 초음파 센서 등의 하드웨어 통합 제어

SocektCAN, RPC를 통해 하드웨어 제어

Multithread programming (led, ultrasonic을 thread로 만들어서 동시 실행)


- RPC
    - DisplayText
    rpi 1에서 user에게 text를 받아 rpi 2에 전달.
    rpi 2는 해당 text를 lcd에 띄우고 text의 length를 반환값으로 전달
    - MoveMotor
    rpi 1에서 rpi 2에 원하는 각도를 전달.
    rpi 2에서 해당 각도에 맞게 motor를 회전시키고 성공 시 0 반환.
    rpi 1은 0을 받으면 움직인 각도를 출력
    - TerminateRPC
    rpi 1에서 user가 “quit”을 입력하면 rpi 2에 전달.
    rpi 2는 해당 text를 받으면 RPC, CAN 통신을 종료하고 정상 종료 시 0 반환 후 종료
    rpi 1은 0을 받으면  can, rpc 끄고 종료. (led, ultrasonic thread도 종료해야 함)



https://github.com/user-attachments/assets/12059f70-b3a2-4117-a654-c5cab56d3732


![image](https://github.com/user-attachments/assets/5d757816-9e45-40ad-9760-581917648880)

![image](https://github.com/user-attachments/assets/7143cc94-2980-4a8c-87b3-84b857196917)

![image](https://github.com/user-attachments/assets/10881b42-4e99-4dad-b25c-6cdfa8987ca2)
![image](https://github.com/user-attachments/assets/f324f3fa-ac11-4cc3-a1ce-3201fd397064)
