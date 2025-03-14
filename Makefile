TARGET_PROJECT = main
TARGET_TEST = tests
CXX = g++
CXXFLAGS = -std=c++17 -I$(GTEST_DIR)/include
LDFLAGS = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread

PROJECT_SRCS = $(wildcard src/*.cpp)
TEST_SRCS = $(wildcard test/*.cpp)
COMMON_SRCS = $(wildcard common/*.cpp)

PROJECT_OBJS = $(PROJECT_SRCS:.cpp=.o) $(COMMON_SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o) $(COMMON_SRCS:.cpp=.o)

SRCS = src/main.cpp src/GameEngine.cpp src/UIManager.cpp src/Character.cpp \
       src/BattleSystem.cpp src/AIController.cpp src/ShopSystem.cpp src/InventorySystem.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = game

all: $(TARGET_PROJECT) $(TARGET_TEST) $(TARGET)

$(TARGET_PROJECT): $(PROJECT_OBJS)
	$(CXX) -o $@ $^

$(TARGET_TEST): $(TEST_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(PROJECT_OBJS) $(TEST_OBJS) $(TARGET_PROJECT) $(TARGET_TEST) $(OBJS) $(TARGET)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

.PHONY: all clean test