# Root Makefile for MyLibrary project

all: build

build:
	@mkdir -p build && cd build && cmake .. && make

run: build
	./build/bin/server

clean:
	rm -rf build
	@make -C backend clean

.PHONY: all build run clean
