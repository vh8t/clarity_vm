CXX = g++
CXXFLAGS = -Wall -Wextra -g -O2

INCLUDES = -I./include
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET = $(DIST_DIR)/clarity

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(DIST_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
