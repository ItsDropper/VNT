# Copyright (c) 2026 ItsDropper
# Licensed under the MIT License.
CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
TARGET := build/vnx
.PHONY: all clean test
all: $(TARGET)
$(TARGET): src/vnx.c
	mkdir -p build
	$(CC) $(CFLAGS) src/vnx.c -o $(TARGET)
test: $(TARGET)
	sh tests/smoke.sh ./$(TARGET)
clean:
	rm -rf build
