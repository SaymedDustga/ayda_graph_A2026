# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-unused-parameter
INCLUDES = -I./lib

# Source files and the resulting executable
TARGET = main_tests.out
SRC = main_tests.cpp
LIB = lib/

# Default rule
all: $(TARGET)

# How to build the executable
$(TARGET): $(SRC) $(LIB)graph.hpp $(LIB)graph_algorithms.hpp $(LIB)graph_builders.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRC)

# Rule to clean compiled files
clean:
	rm -f $(TARGET) *.out

# Rule to compile and run immediately
run: all
	./$(TARGET)

.PHONY: all clean run