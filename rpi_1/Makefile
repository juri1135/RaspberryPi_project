CC = gcc
CFLAGS = -lwiringPi
SRCS = rpi_1_can.c rpi_1_dijkstra.c rpi_1_led.c rpi_1_main.c rpi_1_stub.c rpi_1_ultrasonic.c
OBJS = $(SRCS:.c=.o)
TARGET = rpi_1_can

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
