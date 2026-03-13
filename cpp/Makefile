# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-unused-parameter
INCLUDES = -I./lib

# Source files and the resulting executables
TARGET_TESTS = main_tests.out
SRC_TESTS = main_tests.cpp

TARGET_EXAMPLES = examples.out
SRC_EXAMPLES = examples.cpp

LIB = lib/

# Default rule
all: $(TARGET_TESTS) $(TARGET_EXAMPLES)

# How to build the test executable
$(TARGET_TESTS): $(SRC_TESTS) $(LIB)graph.hpp $(LIB)graph_algorithms.hpp $(LIB)graph_builders.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET_TESTS) $(SRC_TESTS)

# How to build the examples executable
$(TARGET_EXAMPLES): $(SRC_EXAMPLES) $(LIB)graph.hpp $(LIB)graph_algorithms.hpp $(LIB)graph_builders.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET_EXAMPLES) $(SRC_EXAMPLES)

# Rule to clean compiled files
clean:
	rm -f $(TARGET_TESTS) $(TARGET_EXAMPLES) *.out

# Rule to compile and run tests
run-tests: $(TARGET_TESTS)
	./$(TARGET_TESTS)

# Rule to compile and run examples
run-examples: $(TARGET_EXAMPLES)
	./$(TARGET_EXAMPLES)

# Rule to compile and run both
run: all
	@echo "=== Running Tests ==="
	./$(TARGET_TESTS)
	@echo ""
	@echo "=== Running Examples ==="
	./$(TARGET_EXAMPLES)

.PHONY: all clean run run-tests run-examples