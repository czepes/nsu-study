CC = g++
CFLAGS = -Wall -std=c++17
LIBS = `pkg-config --cflags --libs opencv4 libusb-1.0`
SRC = camera.cpp usb.cpp
TARGET_BASE = camera usb

all: camera usb

camera: camera.cpp
	$(CC) $(CFLAGS) $(LIBS) -o $@ $?

usb: usb.cpp
	$(CC) $(CFLAGS) $(LIBS) -o $@ $?

clean:
	rm -f $(TARGET_BASE)

.PHONY: clean
