# Root Makefile for MyLibrary project

all: build

build:
	@mkdir -p build && cd build && cmake .. && make

run: build
	@if [ -f .env ]; then export $$(cat .env | xargs) && ./build/bin/server; else ./build/bin/server; fi

clean:
	rm -rf build
	@make -C backend clean

.PHONY: all build run clean
