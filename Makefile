.PHONY: build clean run

all: build

build:
	$(MAKE) -C backend build

clean:
	$(MAKE) -C backend clean

run:
	$(MAKE) -C backend run
