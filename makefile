CXX = clang++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Werror=return-type -std=c++20 
LDFLAGS = -lncurses

DEBUG = 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -O0 -g
endif

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/program

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

debug: DEBUG = 1
debug: all
