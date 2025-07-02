# Makefile for ESP-IDF using idf.py

PROJECT_NAME := your_project_name

PORT ?= /dev/ttyUSB0
BAUD ?= 115200

.PHONY: all menuconfig build flash monitor erase clean

all: build

menuconfig:
	idf.py menuconfig

build:
	idf.py build

flash:
	idf.py -p $(PORT) -b $(BAUD) flash

monitor:
	idf.py -p $(PORT) -b $(BAUD) monitor

erase:
	idf.py -p $(PORT) erase_flash

clean:
	idf.py fullclean
