
#
#	Makefile for ipc directory
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoppcbe
#TARGET = -V3.3.5,gcc_ntoarmle

CFLAGS += $(DEBUG) $(TARGET)
LDFLAGS+= $(DEBUG) $(TARGET)

BINS = sendrec sigsleeper shmemcreator shmemuser \
		pulse_server pulse_client sigsleeper_fixed

all: $(BINS)

clean:
	rm -f *.o *.tmp $(BINS)

shmemuser.o: shmemuser.c shmem.h

shmemcreator.o: shmemcreator.c shmem.h

pulse_server.o: pulse_server.c pulse_server.h

pulse_client.o: pulse_client.c pulse_server.h
