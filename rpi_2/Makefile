CC = gcc
CFLAGS = -lwiringPi
SRCS = rpi_2_can.c rpi_2_lcd.c rpi_2_main.c rpi_2_stub.c rpi_2_motor.c 
OBJS = $(SRCS:.c=.o)
TARGET = rpi_2_can

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
