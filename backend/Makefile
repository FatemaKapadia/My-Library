all: build

build:
	@mkdir -p build && cd build && cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.0 .. && make

clean:
	rm -rf build bin

run: build
	@if [ -f configs/.env ]; then export $$(cat configs/.env | xargs) && ./build/bin/server; else ./build/bin/server; fi

.PHONY: all build clean run
