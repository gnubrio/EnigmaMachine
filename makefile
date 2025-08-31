CXX ?= c++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Werror=return-type -std=c++20
LDFLAGS = -lncurses

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = program

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(TARGET)

debug: CXXFLAGS += -DDEBUG -O0 -g
debug: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)
