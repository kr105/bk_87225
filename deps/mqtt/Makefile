LIB = lib
INC=-Iinclude/

OBJS	= MQTTClient.o MQTTLinux.o MQTTConnectClient.o MQTTConnectServer.o MQTTDeserializePublish.o MQTTFormat.o MQTTPacket.o MQTTSerializePublish.o MQTTSubscribeClient.o MQTTSubscribeServer.o MQTTUnsubscribeClient.o MQTTUnsubscribeServer.o
OUT		= libmqtt.a
AR		= $(CROSS_COMPILE)arm-linux-gnueabi-ar
CC		= $(CROSS_COMPILE)arm-linux-gnueabi-gcc
RL		= $(CROSS_COMPILE)arm-linux-gnueabi-ranlib
FLAGS	= -g -c -Wall -DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h
LFLAGS	= -L$(LIB) -lpthread

all: $(OBJS)
	$(AR) ru $(OUT) $(OBJS)
	$(RL) $(OUT)

MQTTClient.o: src/MQTTClient.c
	$(CC) $(FLAGS) $(INC) src/MQTTClient.c

MQTTLinux.o: src/MQTTLinux.c
	$(CC) $(FLAGS) $(INC) src/MQTTLinux.c

MQTTConnectClient.o: src/MQTTPacket/MQTTConnectClient.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTConnectClient.c

MQTTConnectServer.o: src/MQTTPacket/MQTTConnectServer.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTConnectServer.c

MQTTDeserializePublish.o: src/MQTTPacket/MQTTDeserializePublish.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTDeserializePublish.c

MQTTFormat.o: src/MQTTPacket/MQTTFormat.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTFormat.c

MQTTPacket.o: src/MQTTPacket/MQTTPacket.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTPacket.c

MQTTSerializePublish.o: src/MQTTPacket/MQTTSerializePublish.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTSerializePublish.c

MQTTSubscribeClient.o: src/MQTTPacket/MQTTSubscribeClient.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTSubscribeClient.c

MQTTSubscribeServer.o: src/MQTTPacket/MQTTSubscribeServer.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTSubscribeServer.c

MQTTUnsubscribeClient.o: src/MQTTPacket/MQTTUnsubscribeClient.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTUnsubscribeClient.c

MQTTUnsubscribeServer.o: src/MQTTPacket/MQTTUnsubscribeServer.c
	$(CC) $(FLAGS) $(INC) src/MQTTPacket/MQTTUnsubscribeServer.c

clean:
	rm -f $(OBJS) $(OUT)