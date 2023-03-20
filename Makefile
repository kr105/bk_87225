CROSS_COMPILE = /home/kr105/timbre/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/
LIB = -Llib/ -Ldeps/mqtt
INC=-Iinclude/ -Ideps/mqtt/include/

OBJS	= main.o gpio.o mqtt.o
HEADER	= libVideoEnc.h
OUT	= app0
CC	 = $(CROSS_COMPILE)arm-linux-gnueabi-gcc
FLAGS	 = -g -c -Wall -DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h
LFLAGS	 = $(LIB) -lmqtt -lpthread -lcdc_base -lcdc_ve -lcdc_vencoder -lcdc_memory -lVideoEnc-1.4.3 -lasound -lAudioCore -lAudioIn-1.4.3

all: $(OBJS) libmqtt.a
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

libmqtt.a:
	CROSS_COMPILE=$(CROSS_COMPILE) make -C deps/mqtt/

main.o: main.c
	$(CC) $(FLAGS) $(INC) main.c

gpio.o: src/gpio.c
	$(CC) $(FLAGS) $(INC) src/gpio.c

mqtt.o: src/mqtt.c
	$(CC) $(FLAGS) $(INC) src/mqtt.c

clean:
	rm -f $(OBJS) $(OUT)
	CROSS_COMPILE=$(CROSS_COMPILE) make -C deps/mqtt/ clean