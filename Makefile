CXX      = g++
CXXFLAGS = -std=c++17 -I./src/cpp -I./src/h -I./common -I$(GTEST_DIR)/include
LDFLAGS  = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread

DOXYFILE = Doxyfile

SRC_DIR    = src/cpp
HDR_DIR    = src/h
COMMON_DIR = common
TEST_DIR   = test

GAME_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
GAME_OBJS = $(GAME_SRCS:.cpp=.o)
TARGET    = game

PROJECT_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
TEST_SRCS    = $(wildcard $(TEST_DIR)/*.cpp) $(PROJECT_SRCS)
TEST_OBJS    = $(TEST_SRCS:.cpp=.o)
TEST_TARGET  = tests


all: $(TARGET) $(TEST_TARGET) doc

$(TARGET): $(GAME_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


doc:
	doxygen $(DOXYFILE)

doc-clean:
	rm -rf doc/html doc/latex


clean: doc-clean
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(TARGET) $(TEST_TARGET)


test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean test doc doc-clean