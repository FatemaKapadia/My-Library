CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2

SRC_DIR = src
BIN_DIR = bin

TARGET = $(BIN_DIR)/server

SRCS = $(SRC_DIR)/main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -rf $(BIN_DIR)
	rm -rf build

run: all
	./$(TARGET)

cmake-run:
	@mkdir -p build && cd build && cmake .. && make
	./build/bin/server

.PHONY: all clean run cmake-run
