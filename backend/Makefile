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
