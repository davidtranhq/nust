CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -I/opt/homebrew/Cellar/googletest/1.16.0/include
LDFLAGS = -L/opt/homebrew/Cellar/googletest/1.16.0/lib -lgtest -lgtest_main -pthread

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test
TARGET = nust
TEST_TARGET = nust_test

# Main program sources (excluding main.cpp)
LIB_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
LIB_OBJS = $(LIB_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Main program executable source
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ = $(BUILD_DIR)/main.o

# Test sources
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all clean directories test

all: directories $(TARGET)

test: directories $(TEST_TARGET)
	./$(TEST_TARGET)

directories:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET) 