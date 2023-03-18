CROSS_COMPILE = /home/kr105/timbre/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/
LIB = lib

OBJS	= main.o
SOURCE	= main.c
HEADER	= libVideoEnc.h
OUT	= app0
CC	 = $(CROSS_COMPILE)arm-linux-gnueabi-gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -L$(LIB) -lpthread -lcdc_base -lcdc_ve -lcdc_vencoder -lcdc_memory -lVideoEnc-1.4.3 -lasound -lAudioCore -lAudioIn-1.4.3

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c

clean:
	rm -f $(OBJS) $(OUT)