CC = gcc -c
LD = gcc
CFLAGS = -O0 -g
LIBS = -lpaho-mqtt3c

all: hytech_can.so # receiver

hytech_can.so: ../Libraries/HyTech_CAN/can_python.c XBTools.o
	gcc -shared -fPIC $(shell python3-config --includes) $(CFLAGS) -o $@ $^ $(shell python3-config --ldflags)

receiver: telemetry.o XBTools.o
	$(LD) -o $@ $^ $(LIBS)

telemetry.o: telemetry.c
	$(CC) $(CFLAGS) $< -o $@

XBTools.o: ../Libraries/XBTools/XBTools.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -vf *.o receiver
